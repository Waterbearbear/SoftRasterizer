#include "our_gl.h"


int round_float(float number)
{
	return (number > 0.0) ? floor(number + 0.5) : ceil(number - 0.5);
}

void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color)
{
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
	//*pts  三角形的三个点在屏幕空间的坐标
	//P		屏幕空间的 boundingBox里的一个坐标
	//返回重心坐标  (1- u -v , u , v) 进行插值

	Vec3f u = cross(Vec3f(pts[2].x - pts[0].x, pts[1].x - pts[0].x, pts[0].x - P.x), Vec3f(pts[2].y - pts[0].y, pts[1].y - pts[0].y, pts[0].y - P.y));
	/* `pts` and `P` has integer value as coordinates
	   so `abs(u[2])` < 1 means `u[2]` is 0, that means
	   triangle is degenerate, in this case return something with negative coordinates */
	if (std::abs(u.z) < 1) return Vec3f(-1, 1, 1);


	return Vec3f(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
}

Vec3f barycentric(Vec3f *pts, Vec3i P)
{
	//计算在屏幕空间中的重心坐标
	//*pts  三角形的三个点在屏幕空间的坐标
	//P		屏幕空间的 boundingBox里的一个坐标
	//返回重心坐标  (1- u -v , u , v) 进行插值

	Vec3f pf(P.x, P.y, P.z);
	Vec3f x_vector(pts[2].x - pts[0].x, pts[1].x - pts[0].x, pts[0].x - pf.x);
	Vec3f y_vector(pts[2].y - pts[0].y, pts[1].y - pts[0].y, pts[0].y - pf.y);


	Vec3f uv1 = cross(x_vector,y_vector);
	float uv1z = uv1.z;

	if (std::abs(uv1.z) < 1.0f ) return Vec3f(-1.0f, 1.0f, 1.0f);


	return Vec3f(1.f - (uv1.x + uv1.y) / uv1z, uv1.y / uv1z, uv1.x / uv1z);

}

Matrix translate(const float x, const float y, const float z)
{
	Matrix translation_mat;
	translation_mat = translation_mat.identity();

	translation_mat[0][3] = x;
	translation_mat[1][3] = y;
	translation_mat[2][3] = z;

	return translation_mat;
}

Matrix view(Vec3f camera_position, Vec3f camera_lookat, Vec3f camera_up)
{
	Matrix Tview;
	//Matrix Rview;
	
	Vec3f z = (camera_position - camera_lookat).normalize();
	//g.normalize();
	/*Vec3f t = camera_up;*/
	Vec3f x = (cross(camera_up,z).normalize());
	Vec3f y = cross(z,x).normalize();

	Tview = Tview.identity();
	Tview[0][3] = -camera_position.x;
	Tview[1][3] = -camera_position.y;
	Tview[2][3] = -camera_position.z;

	Matrix Rview;
	Rview = Rview.identity();

	for (int i = 0; i < 3; i++) {
		Rview[0][i] = x[i];
		Rview[1][i] = y[i];
		Rview[2][i] = z[i];
	}
	Rview[3][3] = 1.0;

	return Rview * Tview;
}

Matrix perspective_projection(const float n, const float f, const float aspect_ratio, const float fov )
{
	Matrix perspective;
	perspective = perspective.identity();

	//assert(n > f);

	/*if (r < l) std::swap(r, l);
	if (t < b)std::swap(t, b);
	if (n < f)std::swap(n, f);*/

	float arc_half_fov = pi * (fov / 2) / 180;
	float t = std::abs(n)  * std::tan(arc_half_fov);
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

Matrix Otho_projection(float r, float l, float t, float b, float n, float f)
{
	Matrix scale;

	scale = scale.identity();

	scale[0][0] = 2 / (r - l);
	scale[1][1] = 2 / (t - b);
	scale[2][2] = 2 / (n - f);
	scale[3][3] = 1;

	Matrix trans;
	trans = trans.identity();
	
	trans[0][3] = -(r + l) / 2.f;
	trans[1][3] = -(t + b) / 2.f;
	trans[2][3] = - (n + f) / 2.f;


	return scale * trans;
}

Matrix viewport(const size_t width, const size_t height)
{
	Matrix viewport_mat;
	viewport_mat = viewport_mat.identity();


	viewport_mat[0][0] = width / 2.0;
	viewport_mat[1][1] = height / 2.0;
	viewport_mat[2][2] = 1.f;
	viewport_mat[3][3] = 1.f;


	viewport_mat[0][3] = (width) / 2.0;
	viewport_mat[1][3] = (height) / 2.0;

	return viewport_mat;
}

Shader::~Shader()
{

}



void rasterizer(Vec3f *screen_tri, Shader &shader, float *zbuffer, TGAImage &image, TGAColor &color)
{
	Vec2i bboxmax(0.f, 0.f);
	Vec2i bboxmin(image.get_width() - 1, image.get_height() - 1);
	Vec2i screen_tri_int[3];

	for (int i = 0; i < 3; i++)
	{
		screen_tri_int[i].x = static_cast<int>(screen_tri[i].x);
		screen_tri_int[i].y = static_cast<int>(screen_tri[i].y);
	}

	int width = image.get_width();

	/*int texture_width = texture_image.get_width();
	int texture_height = texture_image.get_height();*/

	//找出bounding Box
	for (int i = 0; i < 3; i++)
	{
		if (screen_tri_int[i].x > bboxmax.x) bboxmax.x = screen_tri_int[i].x;
		if (screen_tri_int[i].x < bboxmin.x) bboxmin.x = screen_tri_int[i].x;
		if (screen_tri_int[i].y > bboxmax.y) bboxmax.y = screen_tri_int[i].y;
		if (screen_tri_int[i].y < bboxmin.y) bboxmin.y = screen_tri_int[i].y;
	}

	Vec2i P;

	for (P.x = bboxmin.x; P.x < bboxmax.x; P.x++)
	{
		for (P.y = bboxmin.y; P.y < bboxmax.y; P.y++)
		{
			float Pz = 0;
			Vec2f P_texture;


			Vec3f interpo_coefficient = barycentric(screen_tri_int, P);
			if (interpo_coefficient.x < 0 || interpo_coefficient.y < 0 || interpo_coefficient.z < 0) continue;

			for (int i = 0; i < 3; i++)
			{
				//插值的深度
				Pz += interpo_coefficient[i] * screen_tri[i].z;
				/*P_texture.x += texture_cords[i].x * interpo_coefficient[i];
				P_texture.y += texture_cords[i].y * interpo_coefficient[i];*/
			}


			if (zbuffer[P.x + P.y * width] < Pz)
			{
				zbuffer[P.x + P.y * width] = Pz;
				//std::cerr << "px: " << P.x << "," << "py: " << P.y << "," << "pz: " << Pz << std::endl;

				//TGAColor color = texture_image.get((int)(P_texture.x * texture_width), (int)(P_texture.y * texture_height));
				//TGAColor color2(255, 255, 255);
				//image.set(P.x, P.y, color * intensity);
				bool discard = shader.fragment(interpo_coefficient, color);

				if (!discard)
				{
					image.set(P.x, P.y, color);
				}
			}
		}
	}
}

Mat3 calulateTBN(Vec3f *tri, Vec3f *uv,Vec3f normal)
{

	//tri : 世界坐标
	//uv  : 贴图坐标
	//normal : 世界坐标里插值来的normal
	Vec3f edge1 = tri[1] - tri[0];
	Vec3f edge2 = tri[2] - tri[0];
	Vec3f deltaUV1 = uv[1] - uv[0];
	Vec3f deltaUV2 = uv[2] - uv[0];

	float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
	Vec3f tangent(0.0f, 0.0f, 0.0f);
	Vec3f bitangent(0.f, 0.f, 0.f);
	
	tangent[0] = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
	tangent[1] = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
	tangent[2] = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

	bitangent[0] = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
	bitangent[1] = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
	bitangent[2] = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

	Mat3 TBN;
	for (int i = 0; i < 3; i++)
	{
		TBN[i][0] = tangent[i];
		TBN[i][1] = bitangent[i];
		TBN[i][2] = normal[i];
	}

	return TBN;
}
