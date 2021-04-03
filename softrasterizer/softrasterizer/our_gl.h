#pragma once
#include "tgaimage.h"
#include "geometry.h"


#define pi 3.1415926

extern Matrix Modeling_mat;
extern Matrix view_mat;
extern Matrix projection_mat;
extern Matrix viewport_mat;


Matrix rotation(const float angle, const int axis);
Matrix translate(const float x, const float y, const float z);
Matrix scale(const float x, const float y, const float z);
Matrix view(Vec3f camera_position, Vec3f camera_lookat, Vec3f camera_up);
Matrix perspective_projection(const float n, const float f, const float fov, const float aspect_ratio);
Matrix viewport(const size_t width, const size_t height);


class Shader {
public:
	virtual ~Shader();
	virtual Vec4f vertext(int iface, int nvert) = 0;
	virtual bool fragment(const float bary, TGAColor &color) = 0;
};

void rasterizer(Vec4f *screen_tri, Shader ishader, int *zbuffer, TGAImage &image, TGAColor &color);