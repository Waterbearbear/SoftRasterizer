#ifndef __MODEL_H__
#define __MODEL_H__

#include <vector>
#include "geometry.h"

class Model {
private:
	std::vector<Vec3f> verts_;
	std::vector<Vec3f> textures_;
	std::vector<std::vector<int> > faces_;
	std::vector<std::vector<int> > texture_faces_;
public:
	Model(const char *filename);
	~Model();


	int nverts();
	int nfaces();
	int ntextures();


	Vec3f vert(int i);
	Vec3f texture(int i);
	std::vector<int> face(int idx);
	std::vector<int> texture_face(int idx);


};

#endif //__MODEL_H__
