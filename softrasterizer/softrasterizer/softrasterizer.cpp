#include "tgaimage.h"
#include "model.h"
#include <algorithm>
#include <cassert>
#include <iostream>
#include "our_gl.h"



#define pi 3.14151926

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);

const float ka = 0.1;
const float kd = 1.0;
const float ks = 0.5;

const int width = 1024;
const int height = 1024;

float l = -1.5;
float r = 1.5;

float t = 1.5;
float b = -1.5;

float n = -0.1f;
float f = -200.0f;

float aspect_ratio = 1.0;
float fov = 80;

Vec3f camera_position(0.0f, 0.0f , 2.0f);
Vec3f camera_lookat(0., 0., .0);
Vec3f camera_up(0., 1., 0.);

Vec3f light_position( -2.0f, 4.0f, 1.0f);
Vec3f light_lookat(0.f, 0.f, 0.f);


Vec3f tranlate(0.f, 0.f, -2.f);

Matrix projection_mat = perspective_projection(n, f, aspect_ratio, fov);
//Matrix light_projection_mat = perspective_projection(n, f, aspect_ratio, fov);

Matrix light_projection_mat = Otho_projection(r, l, t, b, -1.5, -3.5);

/*Matrix projection_mat = Otho_projection(r, l, t, b, n, f);*/

//Matrix tranlate_mat = translate(0.f, tranlate, 0.f);

Matrix view_mat = view(camera_position, camera_lookat, camera_up);
Matrix light_view_mat = view(light_position, light_lookat, camera_up);

Matrix viewport_mat = viewport(width, height);

//const char *path = "D:\\project\\zjx\\GameDevoplement\\tinyrenderer\\obj\\diablo3_pose\\diablo3_pose.obj";
const char *path = "D:\\project\\zjx\\GameDevoplement\\SoftRasterizer\\softrasterizer\\softrasterizer\\obj\\african_head\\african_head.obj";


Vec3f light_dir = light_lookat - light_position;
Vec3i ambient_light(0, 100, 200);

Vec3f light(255.f, 255.f, 255.f);

Matrix MVP = projection_mat * view_mat;
Matrix MVP_it = MVP.invert_transpose();
Matrix light_MVP =  light_projection_mat * light_view_mat;


//Model *model = nullptr;
Model *model = new Model(path);

Vec3f h2v(Vec4f mat)
{
	//homogeneous to vector
	return Vec3f(mat[0] / mat[3], mat[1] / mat[3], mat[2] / mat[3]);
}

Vec4f v2h(const Vec3f &vec)
{
	//Vect3f to homogeneous(Vec4f)
	Vec4f temp;

	temp[0] = vec.x;
	temp[1] = vec.y;
	temp[2] = vec.z;
	temp[3] = 1.f;

	return temp;
}

class ShadowShader :public Shader
{
public:
	//ShadowShader
	ShadowShader() = default;
	ShadowShader(float vd = 1.f):view_depth(vd) {};

	virtual Vec3f vertex(int iface, int nvert)
	{
		Vec3f world_coords = model->vert(iface, nvert);

		Vec3f light_mvp_world_coords = h2v(light_MVP *v2h(world_coords));
		/*if (iface == 0 && nvert == 0) view_depth = light_mvp_world_coords.z;*/
		/*else if(std::abs(light_mvp_world_coords.z) > std::abs(view_depth))
		{
			view_depth = light_mvp_world_coords.z;
		}*/

		//此时z在[-1,1]之间
		varying_depth[nvert] = light_mvp_world_coords.z;

		return  h2v(viewport_mat * v2h(light_mvp_world_coords));
	}

	virtual bool fragment(Vec3f bc, TGAColor &color)
	{
		
		float depth = 0.f;
		Vec3f view_direction(0.f, 0.f, 0.f);
		Vec3f normal(0.f, 0.f, 0.f);

		//TBN.invert_transpose();
		TGAColor c(0.f, 0.f, 0.f);
		
		for (int i = 0; i < 3; i++)
		{
			depth += varying_depth[i] * bc[i];
		}

		c = TGAColor(255, 255, 255) *  (( depth * 0.5 + 0.5));
		//for (int i = 0; i < 3; i++) c[i] = std::min<float>(ka * ambient_light[i] + kd * diffuse_color[i] * intensity, 255.0);


		color = c;
		//color = texture_color * intensity;
		return false;
	}

private:
	float varying_depth[3];
	float view_depth;
	//float max_depth;
};

class GouraudShader :public Shader
{
public:
	Vec3f varying_intensity;
	Vec3f vary_uv[3];
	//Vec3f varying_view_vector[3];
	float varying_light_distance[3];
	Vec3f varying_view_direction[3];
	Vec3f varying_normal[3];
	Vec3f varying_world_coords[3];
	//Vec3f screen_tri[3];

	//Vec3f  light_dir = h2v(viewport_mat * v2h(light_lookat - light_position));

	float ka;
	float kd;
	float ks;
	//Vec3f varying_texture;
	GouraudShader() = default;
	GouraudShader(float ambient,float *shadowbuffer) :ka(ambient),m_shadowbuffer(shadowbuffer)
	{
		
	}

	virtual Vec3f vertex(int iface, int nvert)
	{

		Vec3f n = model->noraml(iface, nvert);
		/*TGAColor nTGA = model->nm_value(uv[0], uv[1]);
		Vec3f n(nTGA[0], nTGA[1], nTGA[2]);*/

		//uv 0.12452 0.45234 0.0000
		Vec3f uv = model->texture(iface, nvert);
		Vec3f world_coords = model->vert(iface, nvert);
		Vec3f mvp_world_coords = h2v(viewport_mat * projection_mat * view_mat * v2h(world_coords));

		//TGAColor texture_value = model->texture_value(uv.x, uv.y);
		


		n.normalize();
		light_dir.normalize();

		varying_world_coords[nvert] = world_coords;
		//varying_intensity[nvert] = std::max(0.0f,  - (n * light_dir));
		varying_light_distance[nvert] = ( world_coords - light_position).norm();

		//相机空间中的方向,还是说应该在世界坐标下做变换
		varying_view_direction[nvert] = mvp_world_coords;
		varying_normal[nvert] = n;
		vary_uv[nvert].x = uv.x;
		vary_uv[nvert].y = uv.y;
		vary_uv[nvert].z = 0.0;

		/*varying_view_vector[nvert] = camera_position - */
		return mvp_world_coords;
	}

	virtual bool fragment(Vec3f bc, TGAColor &color)
	{
		/*Vec3f u(0.0,0.0,0.0);
		Vec3f v(1.0, 1.0, 1.0);*/
		float u = 0.f;
		float v = 0.f;
		float distance = 0.f;
		Vec3f view_direction(0.f ,0.f ,0.f );
		Vec3f normal(0.f, 0.f, 0.f);
		Vec3f screenCoords(0.f, 0.f, 0.f);
		Vec3f world_Coords(0.f, 0.f, 0.f);

		//插值
		for (int i = 0; i < 3; i++)
		{
			u += vary_uv[i].x * bc[i];
			v += vary_uv[i].y * bc[i];
			distance += varying_light_distance[i] * bc[i];
			view_direction = varying_view_direction[i] * bc[i] + view_direction;
			normal = varying_normal[i] * bc[i] + normal;
			screenCoords = varying_view_direction[i] * bc[i] + screenCoords;
			world_Coords = varying_world_coords[i] * bc[i] + world_Coords;
		}

		
		Mat3 TBN = calulateTBN(varying_world_coords, vary_uv, normal);
		//TBN.invert_transpose();
		TGAColor tangent_nm_TGA = model->nm_tangent_value(u, v);
		Vec3f tangent_nm;

		tangent_nm[0] = (tangent_nm_TGA[2] / 255.0) * 2.0 - 1.0;
		tangent_nm[1] = (tangent_nm_TGA[1] / 255.0) * 2.0 - 1.0;
		tangent_nm[2] = (tangent_nm_TGA[0] / 255.0) * 2.0 - 1.0;

		/*for (int i = 0; i < 3; i++)
		{
			tangent_nm[i] = (tangent_nm_TGA[i] / 255.0) * 2.0 - 1.0;
		}*/

		tangent_nm = TBN * tangent_nm;
		//float intensity = varying_intensity * bc;
		//normal map in world
		/*TGAColor normalTGA = model->nm_value(u, v);
		Vec3f normal(normalTGA[0], normalTGA[1], normalTGA[2]);*/
		tangent_nm.normalize();
		normal = tangent_nm;

		//float intensity_test = normal * light_dir;

		float intensity = std::max(0.f,  - (normal * light_dir));

		//normal.normalize();

		/*if (intensity > 0.85) intensity = 1;
		else if (intensity > 0.60) intensity = 0.8;
		else if (intensity > 0.45) intensity = 0.6;
		else if (intensity > 0.30) intensity = 0.45;
		else if (intensity > 0.15) intensity = 0.30;
		else intensity = 0;*/


		Vec3f all_light(1.0, 1.0, 1.0);
		//Vec3f spcular_light(1.0, 1.0, 1.0);
		Vec3f spcular_light(2.0f, 2.f, 2.0f);

		Vec3f ambinet_light(0, 55, 155);
		Vec3f glow_light(0, 25, 25);
		Vec3f h = (light_dir + view_direction).normalize() * -1 ;

		TGAColor diffuse_color = model->texture_value(u, v);
		TGAColor glow_color = model->glow_value(u, v);
		TGAColor specular_color = model->spec_value(u, v);
		TGAColor c;

		Vec3f shadow_point(0.f, 0.f, 0.f);

		
		shadow_point = h2v(viewport_mat * light_MVP * v2h(world_Coords));
		//shadow_point = h2v(light_MVP * MVP_it * v2h(screenCoords));

		int idx = int(shadow_point[0] + shadow_point[1] * width);

		float bias = std::max(0.5f * (1.0f - normal * light_dir),0.05f);


		float shadow = 1.0  -  0.8 * (shadow_point.z - bias > m_shadowbuffer[idx]);

		/*if (shadow_point.z < m_shadowbuffer[idx])
		{
			std::cerr << "在这停顿!" << std::endl;
		}
*/

		for(int i = 0; i < 3; i++)
		{
			//c[i] = std::min<float>(ka * ambient_light[i] +  diffuse_color[i] * (light[i]/distance)/255.0f * intensity +  (light[i] / distance) / 255.0f * specular_color[i] * std::pow(std::max(0.0f, normal * h),10), 255.0);

			c[i] = std::min<float>(ka * ambient_light[i] + (diffuse_color[i] * intensity * all_light[i] + specular_color[0] * std::pow(std::max(0.0f, - (normal * h)), 5) * spcular_light[i]) * shadow + glow_color[i] * glow_light[i], 255.0);
		}
		//for (int i = 0; i < 3; i++) c[i] = std::min<float>(ka * ambient_light[i] + kd * diffuse_color[i] * intensity, 255.0);


		color = c;
		//color = texture_color * intensity;
		return false;
	}

private:
	float *m_shadowbuffer;


};

//void fullfill_triangle_zbuffer_texture(Vec3f *tri, TGAImage &zbuffer,Vec3f *texture_cords, TGAImage &image, TGAImage &texture_image,float intensity)
//{
//	Vec2i bboxmax(0, 0);
//	Vec2i bboxmin(image.get_width() - 1, image.get_height() - 1);
//	int width = image.get_width();
//
//	int texture_width = texture_image.get_width();
//	int texture_height = texture_image.get_height();
//
//	for (int i = 0; i < 3; i++)
//	{
//		if (tri[i].x > bboxmax.x) bboxmax.x = tri[i].x;
//		if (tri[i].x < bboxmin.x) bboxmin.x = tri[i].x;
//		if (tri[i].y > bboxmax.y) bboxmax.y = tri[i].y;
//		if (tri[i].y < bboxmin.y) bboxmin.y = tri[i].y;
//	}
//
//	Vec3i P;
//	for (P.x = bboxmin.x; P.x < bboxmax.x; P.x++)
//	{
//		for (P.y = bboxmin.y; P.y < bboxmax.y; P.y++)
//		{
//			float Pz = 0;
//			Vec2f P_texture;
//
//			Vec3f interpo_coefficient = barycentric(tri, P);
//
//			if(interpo_coefficient.x < 0 || interpo_coefficient.y < 0 || interpo_coefficient.z < 0) continue;
//
//			for (int i = 0; i < 3; i++)
//			{
//				Pz += interpo_coefficient[i] * tri[i].z;
//				P_texture.x  +=  texture_cords[i].x * interpo_coefficient[i];
//				P_texture.y  +=  texture_cords[i].y * interpo_coefficient[i];
//			}
//
//			if (zbuffer[P.x + P.y * width] < Pz)
//			{
//				zbuffer[P.x + P.y * width] = Pz;
//				
//				TGAColor color = texture_image.get((int) (P_texture.x * texture_width), (int)(P_texture.y * texture_height));
//				//TGAColor color2(255, 255, 255);
//				image.set(P.x, P.y, color * intensity);
//
//
//			}
//		}
//	}
//}

int main(int argc, char** argv) {
	//image坐标系以左下为原点
	float *zbuffer = new float[width * height];
	float *shadowBuffer = new float[width * height];

	GouraudShader shader(ka, shadowBuffer);
	ShadowShader shadowshader(f);

	TGAImage image(width, height, TGAImage::RGB);
	TGAImage depth_image(width, height, TGAImage::RGB);

	
	TGAColor color(255, 255, 255);
	TGAColor shadow_color(255, 255, 255);


	


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
	std::cerr << projection_mat << std::endl;
	std::cerr << viewport_mat << std::endl;
	std::cerr << "------------" << std::endl;
	Matrix z = (viewport_mat*projection_mat*view_mat);
	std::cerr << z << std::endl;
	//fullfill shadow

	std::cerr << "shadow vertexing" << std::endl;
	for (int i = 0; i < model->nfaces(); i++)
	{
		Vec3f light_screen_coords[3];
		for (int j = 0; j < 3; j++)
		{
			light_screen_coords[j] = shadowshader.vertex(i, j);
			if (light_screen_coords[j].x > width) light_screen_coords[j].x = width;
			if (light_screen_coords[j].x < 0)		light_screen_coords[j].x = 0;
			if (light_screen_coords[j].y > height) light_screen_coords[j].y = height;
			if (light_screen_coords[j].y < 0)		light_screen_coords[j].y = 0;
		}
		rasterizer(light_screen_coords, shadowshader, shadowBuffer, depth_image, shadow_color);
	}
	std::cerr << "shadow finished" << std::endl;


	depth_image.write_tga_file("african_shadow.tga");



	std::cerr << "fragment vertexing" << std::endl;
	for(int i = 0; i < model->nfaces(); i++)
	{
		Vec3f screen_coords[3];

		for (int j = 0; j < 3; j++)
		{
			screen_coords[j] = shader.vertex(i, j);
			if (screen_coords[j].x > width) screen_coords[j].x = width;
			if (screen_coords[j].x < 0)		screen_coords[j].x = 0;
			if (screen_coords[j].y > height) screen_coords[j].y = height;
			if (screen_coords[j].y < 0)		screen_coords[j].y = 0;
		}
		//fullfill_triangle_zbuffer_texture(screen_coords, zbuffer, texture_coords, image, texture_image, intensity);
		rasterizer(screen_coords, shader, zbuffer, image, color);
	}
	std::cerr << "fragment finished" << std::endl;
	//image.flip_vertically();
	image.write_tga_file("african_shadow_fragement.tga");
	return 0;

}