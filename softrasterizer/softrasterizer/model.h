#ifndef __MODEL_H__
#define __MODEL_H__

#include <vector>
#include "geometry.h"
#include "tgaimage.h"

class Model {
private:

	//根据索引值记录所有的v , vt , vn
	std::vector<Vec3f> verts_;
	std::vector<Vec3f> textures_;
	std::vector<Vec3f> normals_;

	//记录所有面的v , vt, vn的索引 : 即记录第i个面的第n个点的索引
	std::vector<std::vector<int> > faces_;
	std::vector<std::vector<int> > texture_faces_;
	std::vector<std::vector<int> > normal_faces_;

	TGAImage texture_image;
	int texture_width;
	int texture_height;


	TGAImage glow_image;
	TGAImage nm_image;
	TGAImage nm_tangent_image;
	TGAImage spec_image;


public:
	Model(const char *model_filepath);
	~Model();


	int nverts();
	int nfaces();
	int ntextures();


	//Vec3f vert(int i);
	Vec3f texture(int i);
	std::vector<int> face(int idx);
	std::vector<int> texture_face(int idx);

	//vt  0.377 0.573 0.000
	//vt  0.383 0.575 0.000
	//texture返回第iface个面的第nvert个点的 texture u ,v 坐标
	Vec3f texture(int iface, int nvert);
	Vec3f vert(int iface, int nvert);
	Vec3f noraml(int iface, int nvert);


	TGAColor texture_value(float u, float v);
	TGAColor glow_value(float u, float v);
	TGAColor nm_value(float u, float v);
	TGAColor nm_tangent_value(float u, float v);
	TGAColor spec_value(float u, float v);



	

};

#endif //__MODEL_H__
