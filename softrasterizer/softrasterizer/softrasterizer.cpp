#include "tgaimage.h"
#include "model.h"
#include <algorithm>
#include <cassert>
#include <iostream>
//#include "geometry.h"


#define pi 3.14151926

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);


const int width = 1024;
const int height = 1024;


void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) {

	//利用贝塞尔曲线的方式画出直线
	bool steep = false;
	if (std::abs(x1 - x0) < std::abs(y1 - y0))
	{
		std::swap(x0, y0);
		std::swap(x1, y1);
		steep = true;
	}
	if (x0 > x1)
	{
		std::swap(x0, x1);
		std::swap(y0, y1);
	}
	for (int x = x0; x < x1; x++)
	{
		float t = (x - x0) / float(x1 - x0);
		int y = y0 * (1.0 - t) + y1 * t;
		if (steep)
		{
			image.set(y, x, color);
		}
		else
		{
			image.set(x, y, color);
		}
	}
	


	/*for (float t = 0.; t < 1; t += 0.01)
	{
		int x = x0 + t * (x1 - x0);
		int y = y0 + t * (y1 - y0);

		image.set(x, y, color);
	}*/
}

void line(Vec2i t0, Vec2i t1, TGAImage &image, TGAColor color)
{
	bool steep = false;
	int x0, y0, x1, y1;
	/*if (t1.x > t0.x)
	{
		x0 = t1.x;
	}*/
	x0 = t0.x;
	y0 = t0.y;
	x1 = t1.x;
	y1 = t1.y;
	line(x0, y0, x1, y1, image, color);

}



void triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color)
{
	line(t0, t1, image, color);
	line(t1, t2, image, color);
	line(t2, t0, image, color);
}




Vec3f barycentric(Vec2i *pts, Vec2i P) {


	//计算在屏幕空间中的重心坐标
	Vec3f u = Vec3f(pts[2].x - pts[0].x, pts[1].x - pts[0].x, pts[0].x - P.x) ^ Vec3f(pts[2].y - pts[0].y, pts[1].y - pts[0].y, pts[0].y - P.y);
	/* `pts` and `P` has integer value as coordinates
	   so `abs(u[2])` < 1 means `u[2]` is 0, that means
	   triangle is degenerate, in this case return something with negative coordinates */
	if (std::abs(u.z) < 1) return Vec3f(-1, 1, 1);
	return Vec3f(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
}


Vec3f barycentric(Vec3f *pts, Vec3i P)
{
	Vec3f uv1 = Vec3f(pts[2].x - pts[0].x, pts[1].x - pts[0].x, pts[0].x - P.x) ^ Vec3f(pts[2].y - pts[0].y, pts[1].y - pts[0].y, pts[0].y - P.y);
	

	if (std::abs(uv1.z) < 1) return Vec3f(-1, 1, 1);
	return Vec3f(1.f - (uv1.x + uv1.y) / uv1.z, uv1.y / uv1.z, uv1.x / uv1.z);

}

bool  P_in_triangle(Vec2i *pts, Vec2i p)
{
	/*int cross_value_1 = pts[0].x * p.y + p.x * pts[0].y;
	int cross_value_2 = */
	//有溢出的问题

	Vec2i vec_tri[3];
	int cross_value[3];
	Vec2i vec_p[3];
	for (int i = 0; i < 3; i++)
	{
		vec_tri[i] = pts[(i + 1)%3] - pts[i];
		vec_p[i] = p - pts[i];

		cross_value[i] = vec_tri[i].x * vec_p[i].y - vec_p[i].x * vec_tri[i].y;

	}
	if (cross_value[0] * cross_value[1] < 0 || cross_value[0] * cross_value[2] < 0 || cross_value[1] * cross_value[2] < 0)
	{
		return false;
	}
	/*for (int i = 1; i < 3; i++)
	{
	
	}*/
	return true;
}

void fullfill_triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color)
{
	if (t0.y > t1.y) std::swap(t0, t1);
	if (t0.y > t2.y) std::swap(t0, t2);
	if (t1.y > t2.y) std::swap(t2, t1);

	int total_height = t2.y - t0.y;
	int segment_height_bottom = t1.y - t0.y + 1;
	int segment_height_upper = t2.y - t1.y + 1;

	for (int y = t0.y; y <=t2.y; y++)
	{
		if (y < t1.y)
		{
			float alpha = float( y - t0.y) / total_height;
			float beta = float( y - t0.y) / segment_height_bottom;

			auto A = t0 + (t2 - t0) * alpha;
			auto B = t0 + (t1 - t0) * beta;

			//line(A.x, y, B.x, y, image, color);
			if (A.x > B.x)std::swap(A, B);
			for (int j = A.x; j <= B.x; j++)
			{
				image.set(j, y, color);
			}

		}
		else
		{
			float alpha = float(y - t0.y) / total_height;
			float beta = float( y - t1.y) / segment_height_upper;

			auto A = t0 + (t2 - t0) * alpha;
			auto B = t1 + (t2 - t1) * beta;

			//line(A.x, y, B.x, y, image, color);
			if (A.x > B.x)std::swap(A, B);
			for (int j = A.x; j <= B.x; j++)
			{
				image.set(j, y, color);
			}
		}
	}

	//triangle(t0, t1, t2, image, red);


}

void fullfill_triangle_bc(Vec2i *tri, TGAImage &image, TGAColor color)
{
	Vec2i bboxmax(0, 0);
	Vec2i bboxmin(image.get_width() - 1, image.get_height() - 1);

	
	for (int i = 0; i < 3; i++)
	{
		if (tri[i].x > bboxmax.x) bboxmax.x = tri[i].x;
		if (tri[i].x < bboxmin.x) bboxmin.x = tri[i].x;
		if (tri[i].y > bboxmax.y) bboxmax.y = tri[i].y;
		if (tri[i].y < bboxmin.y) bboxmin.y = tri[i].y;
	}

	Vec2i P;
	for (P.x = bboxmin.x; P.x < bboxmax.x; P.x++)
	{
		for (P.y = bboxmin.y; P.y < bboxmax.y; P.y++)
		{
			auto u = barycentric(tri, P);
			if (u.x < 0 || u.y < 0 || u.z < 0) continue;
			image.set(P.x, P.y, color);
		}
	}
}

void fullfill_triangle_zbuffer(Vec3f *tri, float *zbuffer, TGAImage &image, TGAColor color)
{
	Vec2i bboxmax(0, 0);
	Vec2i bboxmin(image.get_width() - 1, image.get_height() - 1);
	int width = image.get_width();


	for (int i = 0; i < 3; i++)
	{
		if (tri[i].x > bboxmax.x) bboxmax.x = tri[i].x;
		if (tri[i].x < bboxmin.x) bboxmin.x = tri[i].x;
		if (tri[i].y > bboxmax.y) bboxmax.y = tri[i].y;
		if (tri[i].y < bboxmin.y) bboxmin.y = tri[i].y;
	}

	Vec3i P;
	for (P.x = bboxmin.x; P.x < bboxmax.x; P.x++)
	{
		for (P.y = bboxmin.y; P.y < bboxmax.y; P.y++)
		{
			float Pz = 0;
			Vec3f interpo_coefficient = barycentric(tri, P);
			if (interpo_coefficient.x < 0 || interpo_coefficient.y < 0 || interpo_coefficient.z < 0) continue;

			for (int i = 0; i < 3; i++)
			{
				Pz += interpo_coefficient[i] * tri[i].z;
			}
			if (zbuffer[P.x + P.y * width] < Pz)
			{
				zbuffer[P.x + P.y * width] = Pz;
				image.set(P.x, P.y, color);
			}
		}
	}
}


void fullfill_triangle_zbuffer_texture(Vec3f *tri, float *zbuffer,Vec3f *texture_cords, TGAImage &image, TGAImage &texture_image,float intensity)
{
	Vec2i bboxmax(0, 0);
	Vec2i bboxmin(image.get_width() - 1, image.get_height() - 1);
	int width = image.get_width();

	int texture_width = texture_image.get_width();
	int texture_height = texture_image.get_height();

	for (int i = 0; i < 3; i++)
	{
		if (tri[i].x > bboxmax.x) bboxmax.x = tri[i].x;
		if (tri[i].x < bboxmin.x) bboxmin.x = tri[i].x;
		if (tri[i].y > bboxmax.y) bboxmax.y = tri[i].y;
		if (tri[i].y < bboxmin.y) bboxmin.y = tri[i].y;
	}

	Vec3i P;
	for (P.x = bboxmin.x; P.x < bboxmax.x; P.x++)
	{
		for (P.y = bboxmin.y; P.y < bboxmax.y; P.y++)
		{
			float Pz = 0;
			Vec2f P_texture;

			Vec3f interpo_coefficient = barycentric(tri, P);

			if(interpo_coefficient.x < 0 || interpo_coefficient.y < 0 || interpo_coefficient.z < 0) continue;

			for (int i = 0; i < 3; i++)
			{
				Pz += interpo_coefficient[i] * tri[i].z;
				P_texture.x  +=  texture_cords[i].x * interpo_coefficient[i];
				P_texture.y  +=  texture_cords[i].y * interpo_coefficient[i];
			}

			if (zbuffer[P.x + P.y * width] < Pz)
			{
				zbuffer[P.x + P.y * width] = Pz;
				
				TGAColor color = texture_image.get((int) (P_texture.x * texture_width), (int)(P_texture.y * texture_height));
				//TGAColor color2(255, 255, 255);
				image.set(P.x, P.y, color * intensity);


			}
		}
	}
}




Vec3f worldtoscreen(Vec3f world_cords)
{
	return Vec3f(int((world_cords.x + 1.)*width / 2), int((world_cords.y + 1.) * height / 2), world_cords.z);
}

Vec3f To_Homo_vec(Vec3f &vec , int x)
{
	Vec3f Homo_vec;

	for (int i = 0; i < 4; i++)
	{
		if (i < 3)
		{
			Homo_vec[i] = vec[i];
		}
		else
		{
			Homo_vec[i] = x;
		}
	}

	return Homo_vec;

}


Matrix rotate(const float &rotation,int axis)
{
	Matrix temp(4,4);

	temp = temp.identity(4);

	float arc = rotation / 180 * 3.14159;



	return temp;
}

Matrix translate(const float &scale,const Vec3f &translation , const float &shear = 0)
{
	//Matrix rotation_mat(4, 4);
	Matrix translation_mat;
	translation_mat = translation_mat.identity(4);


	translation_mat[0][3] = translation.x;
	translation_mat[1][3] = translation.y;
	translation_mat[2][3] = translation.z;




	return translation_mat;

}

Matrix view(Vec3f camera_position,Vec3f camera_lookat,Vec3f camera_up)
{
	Matrix Tview;
	//Matrix Rview;
	

	//g不是应该取反方向吗？
	Vec3f z =    (camera_position - camera_lookat).normalize();
	//g.normalize();
	/*Vec3f t = camera_up;*/
	Vec3f x = (camera_up ^ z).normalize();
	Vec3f y = (z ^ x).normalize();
	

	//t.normalize();

	//e.normalize();


	Tview = Tview.identity(4);
	Tview[0][3] = -camera_position.x;
	Tview[1][3] = -camera_position.y;
	Tview[2][3] = -camera_position.z;

	Matrix Rview;
	Rview = Rview.identity(4);

	for (int i = 0; i < 3; i++) {
		Rview[0][i] = x[i];
		Rview[1][i] = y[i];
		Rview[2][i] = z[i];
		//Rview[i][3] = -Rview[i];
	}

	Rview[3][3] = 1.0;
	

	return Rview * Tview;
}


Matrix Otho_projection(float r, float l, float t,  float b,  float n,  float f)
{
	Matrix Otho_mat(4, 4);

	Otho_mat = Otho_mat.identity(4);


	/*if (r < l) std::swap(r,l);
	if (t < b)std::swap(t, b);
	if (n < f)std::swap(n, f);*/

	Otho_mat[0][0] = 2 / (r - l);
	Otho_mat[1][1] = 2 / (t - b);
	Otho_mat[2][2] = 2 / (n - f);
	Otho_mat[3][3] = 1;

	Otho_mat[0][3] = -(r + l) / (r - l);
	Otho_mat[1][3] = -(t + b) / (t - b);
	Otho_mat[2][3] = -(n + f) / (n - f);

	return Otho_mat;
}

Matrix Perspective_projection(float n, float f,float aspect_ratio,float fov)
{
	Matrix perspective(4,4);
	perspective = perspective.identity(4);

	//assert(n > f);

	/*if (r < l) std::swap(r, l);
	if (t < b)std::swap(t, b);
	if (n < f)std::swap(n, f);*/

	float arc_half_fov = pi * (fov / 2) / 180;
	float t = std::abs(n)  * std::tan(arc_half_fov) ;
	float r = aspect_ratio * t;

	float b = -t;
	float l = -r;


	/*perspective[0][0] = 2 * n / (r - l);
	perspective[1][1] = 2 * n / (t - b);
	perspective[2][2] = (f + n) / (n - f);
	perspective[3][3] = 0;


	perspective[0][2] = (l + r) / (l - r);
	perspective[1][2] = (b + t) / (b - t);
	perspective[2][2] = (2 * f*n) / (f - n);
	perspective[3][2] = 1;*/

	/*perspective[0][0] = n;
	perspective[1][1] = n;
	perspective[2][2] = n + f;
	perspective[2][3] = -f * n;
	perspective[3][2] = 1;
	perspective[3][3] = 0;
	
	Matrix otho_perject_mat = Otho_projection(r, l, t, b, n, f);*/

	perspective[0][0] = 2 * n / (r - l);
	perspective[0][2] = (l + r) / (l - r);
	perspective[1][1] = 2 * n / (t - b);
	perspective[1][2] = (b + t) / (b - t);
	perspective[2][2] = (f + n) / (n - f);
	perspective[2][3] = (2 * f * n) / (f - n);
	perspective[3][2] = 1.0;
	perspective[3][3] = 0.0;

	//perspective[3][2] = -1.f / 1;

	//为什么是这个顺序???
	//return perspective * otho_perject_mat;
	//return otho_perject_mat * perspective;
	return perspective;


}

Matrix viewport(const float &width,const float &height)
{
	Matrix viewport_mat(4, 4);
	viewport_mat = viewport_mat.identity(4);


	viewport_mat[0][0] = width / 2.0;
	viewport_mat[1][1] = height / 2.0;
	viewport_mat[2][2] = 1.f;
	viewport_mat[3][3] = 1.f;


	viewport_mat[0][3] = (width )/ 2.0;
	viewport_mat[1][3] = (height )/ 2.0;

	return viewport_mat;
}

Vec3f m2v(Matrix mat)
{
	return Vec3f(mat[0][0]/mat[3][0], mat[1][0]/mat[3][0], mat[2][0]/mat[3][0]);
}

Matrix v2m(const Vec3f &vec)
{
	Matrix temp(4, 1);

	temp[0][0] = vec.x;
	temp[1][0] = vec.y;
	temp[2][0] = vec.z;
	temp[3][0] = 1.f;

	return temp;
}



int main(int argc, char** argv) {



	//image坐标系以左下为原点

	const char *path = "D:\\project\\zjx\\GameDevoplement\\SoftRasterizer\\softrasterizer\\softrasterizer\\obj\\african_head.obj" ;
	const char *texture_path = "D:\\project\\zjx\\GameDevoplement\\SoftRasterizer\\softrasterizer\\softrasterizer\\obj\\african_head_diffuse.tga";

	TGAImage image(width, height, TGAImage::RGB);
	TGAImage texture_image;
	
	texture_image.read_tga_file(texture_path);

	texture_image.flip_vertically();

	float *zbuffer = new float[width * height];
	

	Model obj(path);
	Model *model = &obj;
	float l = -1.0;
	float r = 1.0;

	float t = 2.0;
	float b = -2.0;

	float n = -0.1f;
	float f = -200.0f;

	float aspect_ratio = 1.0;
	float fov = 80;

	Vec3f camera_position(0.25, 0.5 , 1.25);
	Vec3f camera_lookat(0., 0., .0);
	Vec3f camera_up(0., 1., 0.);
	

	Vec3f light(0., 0., -1.0);
	Vec3f tranlate(0.f, 0.f, -2.f);


	Vec2i up_left(256, 256);
	Vec2i up_right(512, 256);

	Vec2i bottom_left(256, 512);
	Vec2i bottom_right(512, 512);


	Matrix perspective_mat = Perspective_projection(n,f,aspect_ratio,fov);

	Matrix otho_mat = Otho_projection(r, l, t, b, n, f);

	Matrix tranlate_mat = translate(0.f, tranlate, 0.f);

	Matrix view_mat = view(camera_position, camera_lookat, camera_up);

	Matrix viewport_mat = viewport(width, height);


//	Model model(path);


	//image.set(52, 41, red);
	/*line(13, 20, 80, 40, image, white);
	line(20, 13, 40, 80, image, red);
	line(80, 40, 13, 20, image, red);*/

	//1. display a front view of model with all triangle line
	/*for (int i = 0; i < model->nfaces(); i++)
	{
		std::vector<int> face = model->face(i);

		for (int j = 0; j < 3; j++)
		{
			Vec3f v0 = model->vert(face[j]);
			Vec3f v1 = model->vert(face[(j + 1) % 3]);

			int x0 = (v0.x + 1.0)  * width / 2;
			int y0 = (v0.y + 1.0)  * height / 2;

			int x1 = (v1.x + 1.0)  * width / 2;
			int y1 = (v1.y + 1.0)  * height / 2;

			line(x0, y0, x1, y1, image, white);

		}


		
	}*/

	//triangle()
	//triangle(a, b, c, image, white);

	

	//fullfill a triangle with giving three points

	/*fullfill_triangle(t0[0], t0[1], t0[2], image, white);
	fullfill_triangle(t1[0], t1[1], t1[2], image, white);
	fullfill_triangle(t2[0], t2[1], t2[2], image, white);*/



	//2. fullfill a triangle with barycentric coords
	/*fullfill_triangle_bc(t0, image, white);
	fullfill_triangle_bc(t1, image, red);
	fullfill_triangle_bc(t2, image, green);*/


	//3. display a model front size with random color
	/*for (int i = 0; i < model->nfaces(); i++)
	{
		std::vector<int> face = model->face(i);
		Vec2i screen_coords[3];
		for (int j = 0; j < 3; j++)
		{
			Vec3f world_coords = model->vert(face[j]);
			screen_coords[j] = Vec2i((world_coords.x + 1.0) * width / 2, (world_coords.y + 1.0) * height / 2);

		}
		fullfill_triangle_bc(screen_coords, image, TGAColor(std::rand() % 255, std::rand() % 255, std::rand() % 255, 255));
	}*/

	//4.Render with light
	//Vec3f light(0., 0.,   1.);

	//for (int i = 0; i < model->nfaces(); i++)
	//{
	//	std::vector<int> face = model->face(i);
	//	Vec3f world_coords[3];
	//	Vec2i screen_coords[3];
	//	for (int j = 0; j < 3; j++)
	//	{
	//		world_coords[j] = model->vert(face[j]);
	//		Vec3f v = world_coords[j];
	//		screen_coords[j] = Vec2i((v.x + 1.0) * width / 2, (v.y + 1.0) * height / 2);

	//	}

	//	Vec3f n = cross((world_coords[1] - world_coords[0]),(world_coords[2] - world_coords[0]));
	//	n.normalize();
	//	auto intensity = n * light;
	//	if (intensity > 0)
	//	{
	//		fullfill_triangle_bc(screen_coords, image, TGAColor(intensity * 255, intensity * 255, intensity * 255, 255));
	//	}

	//	//vec3f world_coords = model->vert(face[])
	//}

	//5. Z-buffer

	std::cerr << view_mat << std::endl;
	std::cerr << perspective_mat << std::endl;
	std::cerr << viewport_mat << std::endl;
	Matrix z = (viewport_mat*perspective_mat*view_mat);
	std::cerr << z << std::endl;


	for(int i = 0; i < model->nfaces(); i++)
	{
		std::vector<int> face = model->face(i);
		std::vector<int> texture_face = model->texture_face(i);
		Vec3f world_coords[3];
		Vec3f texture_coords[3];
		Vec3f screen_coords[3];

		bool flag = false;
		for (int j = 0; j < 3; j++)
		{
			Vec3f v = model->vert(face[j]);
			texture_coords[j] = model->texture(texture_face[j]);

			world_coords[j] = v;


			screen_coords[j] = m2v(viewport_mat * perspective_mat * view_mat  * v2m(v));


			if (screen_coords[j].x > width) screen_coords[j].x = width;
			if (screen_coords[j].x < 0)		screen_coords[j].x = 0;
			if (screen_coords[j].y > height) screen_coords[j].y = height;
			if (screen_coords[j].y < 0)		screen_coords[j].y = 0;

			/*if (screen_coords[j].x > width ||
				(screen_coords[j].x < 0) ||
				(screen_coords[j].y > height) ||
				(screen_coords[j].y < 0))
			{
				flag = true;
			}*/
		}


		Vec3f n = (world_coords[2] - world_coords[0]) ^ (world_coords[1] - world_coords[0]);
		//Vec3f n = (screen_coords[2] - screen_coords[0]) ^ (screen_coords[1] - screen_coords[0]);
		n.normalize();
		auto intensity = n * light;

		if (intensity > 0 && !flag )
		{

			/*fullfill_triangle_bc(screen_coords, image, tgacolor(intensity * 255, intensity * 255, intensity * 255, 255));*/
			//fullfill_triangle_zbuffer(screen_coords, zbuffer, image, tgacolor(intensity * 255, intensity * 255, intensity * 255, 255));
			fullfill_triangle_zbuffer_texture(screen_coords, zbuffer, texture_coords, image, texture_image, intensity);

		}

		//vec3f world_coords = model->vert(face[])

	}


	//image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("perspective rendering_2.tga");
	return 0;

}