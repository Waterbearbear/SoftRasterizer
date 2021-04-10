#pragma once
#include "tgaimage.h"
#include "geometry.h"
#include "model.h"

#define pi 3.1415926


extern Model *model;

extern Matrix Modeling_mat;
extern Matrix view_mat;
extern Matrix projection_mat;
extern Matrix viewport_mat;
extern Matrix light_view_mat;



void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color);
void line(Vec2i t0, Vec2i t1, TGAImage &image, TGAColor color);
void triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color);


Vec3f barycentric(Vec2i *pts, Vec2i P);
Vec3f barycentric(Vec2f *pts, Vec2i P);
Matrix rotation(const float angle, const int axis);
Matrix translate(const float x, const float y, const float z);
Matrix scale(const float x, const float y, const float z);
Matrix view(Vec3f camera_position, Vec3f camera_lookat, Vec3f camera_up);
Matrix perspective_projection(const float n, const float f, const float fov, const float aspect_ratio);
Matrix Otho_projection(float r, float l, float t, float b, float n, float f);

Matrix viewport(const size_t width, const size_t height);
Mat3 calulateTBN(Vec3f *tri, Vec3f *uv, Vec3f normal);



class Shader {
public:
	virtual ~Shader();
	virtual Vec3f vertex(int iface, int nthvert) = 0;
	virtual bool fragment(Vec3f bar, TGAColor &color) = 0;
};

void rasterizer(Vec3f *screen_tri, Shader &ishader, float *zbuffer, TGAImage &image, TGAColor &color);

//
//class Pipline {
//public:
//	Pipline();
//	virtual ~Pipline() = 0;
//	virtual Vec4f vertext(int iface, int nvert) = 0;
//	virtual void rasterizer() = 0;
//	virtual bool fragment(const float bary, TGAColor &color) = 0;
//
//protected:
//	Model *m_model;
//	TGAImage *m_image;
//	TGAImage *m_zbuffer;
//};
//
//class GrauondShade :public Pipline {
//public:
//	GrauondShade(Model *model,Vec3f &light ,TGAImage &image, TGAImage &zbuffer)
//	{
//		m_model = model;
//		m_image = &image;
//		m_zbuffer = &zbuffer;
//		m_light = light;
//	}
//
//	virtual Vec4f vertext(int iface, int nvert) override
//	{
//		for(int i =0;i<m_model->)
//	}
//
//	virtual void rasterizer() 
//	{
//		Vec2i bboxmax(0, 0);
//		Vec2i bboxmin(image.get_width() - 1, image.get_height() - 1);
//		int width = image.get_width();
//
//		int texture_width = texture_image.get_width();
//		int texture_height = texture_image.get_height();
//
//		for (int i = 0; i < 3; i++)
//		{
//			if (tri[i].x > bboxmax.x) bboxmax.x = tri[i].x;
//			if (tri[i].x < bboxmin.x) bboxmin.x = tri[i].x;
//			if (tri[i].y > bboxmax.y) bboxmax.y = tri[i].y;
//			if (tri[i].y < bboxmin.y) bboxmin.y = tri[i].y;
//		}
//
//		Vec3i P;
//		for (P.x = bboxmin.x; P.x < bboxmax.x; P.x++)
//		{
//			for (P.y = bboxmin.y; P.y < bboxmax.y; P.y++)
//			{
//				float Pz = 0;
//				Vec2f P_texture;
//
//				Vec3f interpo_coefficient = barycentric(tri, P);
//
//				if (interpo_coefficient.x < 0 || interpo_coefficient.y < 0 || interpo_coefficient.z < 0) continue;
//
//				for (int i = 0; i < 3; i++)
//				{
//					Pz += interpo_coefficient[i] * tri[i].z;
//					P_texture.x += texture_cords[i].x * interpo_coefficient[i];
//					P_texture.y += texture_cords[i].y * interpo_coefficient[i];
//				}
//
//				if (zbuffer[P.x + P.y * width] < Pz)
//				{
//					zbuffer[P.x + P.y * width] = Pz;
//
//					TGAColor color = texture_image.get((int)(P_texture.x * texture_width), (int)(P_texture.y * texture_height));
//					//TGAColor color2(255, 255, 255);
//					image.set(P.x, P.y, color * intensity);
//
//
//				}
//			}
//		}
//	}
//
//	virtual bool fragment(const float bary, TGAColor &color)
//	{
//
//	}
//
//private:
//	Vec3f m_light;
//	Vec3f varying_intensity;
//
//};


//void rasterizer(Vec4f *screen_tri, Pipline &ishader, int *zbuffer, TGAImage &image, TGAColor &color);


