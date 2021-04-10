#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "model.h"

Model::Model(const char *model_filepath) : verts_(), faces_() ,textures_(),texture_faces_(){

	std::ifstream in;
	in.open(model_filepath, std::ifstream::in);

	std::string model_path(model_filepath);
	std::string map_path(model_path.begin() ,model_path.end() - 4);

	std::string diffuse_path(map_path + "_diffuse.tga");
	std::string glow_path(map_path + "_glow.tga");
	std::string nm_path(map_path + "_nm.tga");
	std::string nm_tangent_path(map_path + "_nm_tangent.tga");
	std::string spec_path(map_path + "_spec.tga");
	

	texture_image.read_tga_file(diffuse_path);
	glow_image.read_tga_file(glow_path);
	nm_image.read_tga_file(nm_path);
	nm_tangent_image.read_tga_file(nm_tangent_path);
	spec_image.read_tga_file(spec_path);


	texture_image.flip_vertically();
	glow_image.flip_vertically();
	nm_image.flip_vertically();
	nm_tangent_image.flip_vertically();
	spec_image.flip_vertically();

	//texture_image.flip_horizontally();

	texture_width = texture_image.get_width();
	texture_height = texture_image.get_height();


	if (in.fail()) return;
	std::string line;
	while (!in.eof()) {
		std::getline(in, line);
		std::istringstream iss(line.c_str());
		char trash;
		if (!line.compare(0, 2, "v ")) {
			iss >> trash;
			Vec3f v;
			for (int i = 0; i < 3; i++) iss >> v[i];
			verts_.push_back(v);
		}
		else if(!line.compare(0, 3, "vt ")){
			iss >> trash >> trash;
			Vec3f vt;
			for (int i = 0; i < 3; i++) iss >> vt[i];
			textures_.push_back(vt);
		}

		else if (!line.compare(0, 3, "vn ")) {
			iss >> trash >> trash;
			Vec3f vn;
			for (int i = 0; i < 3; i++) iss >> vn[i];
			normals_.push_back(vn);
		}

		else if (!line.compare(0, 2, "f ")) {

			//# f 24/1/24 25/2/25 26/3/26
			std::vector<int> f; //vec f = [24,25,26]
			std::vector<int> f_texture;
			std::vector<int> f_normal;

			int normal_idx, idx;
			int texture_id;
			iss >> trash;
			while (iss >> idx >> trash >> texture_id >> trash >> normal_idx) {
				idx--; // in wavefront obj all indices start at 1, not zero
				texture_id--;
				normal_idx--;

				f.push_back(idx);
				f_texture.push_back(texture_id);
				f_normal.push_back(normal_idx);

			}

			faces_.push_back(f);
			texture_faces_.push_back(f_texture);
			normal_faces_.push_back(f_normal);
		}
	}
	std::cerr << "# v# " << verts_.size() << " f# " << faces_.size() << " vt#" << textures_.size() << " vn#" << normal_faces_.size() << std::endl;
}

Model::~Model() {

}

int Model::nverts() {
	return (int)verts_.size();
}

int Model::nfaces() {
	return (int)faces_.size();
}

int Model::ntextures() {
	return (int)textures_.size();
}

std::vector<int> Model::face(int idx) {
	return faces_[idx];
}

std::vector<int> Model::texture_face(int idx) {
	return texture_faces_[idx];
}

//Vec3f Model::vert(int i) {
//	return verts_[i];
//}

Vec3f Model::texture(int i) {
	return textures_[i];
}

TGAColor Model::texture_value(float u, float v)
{
	int x_index = (int)(u * texture_width);
	int y_index = (int)(v * texture_height);

	return texture_image.get(x_index, y_index);

}

TGAColor Model::glow_value(float u, float v)
{
	int x_index = (int)(u * glow_image.get_width());
	int y_index = (int)(v * glow_image.get_height());

	return glow_image.get(x_index, y_index);

}

TGAColor Model::nm_value(float u, float v)
{
	int x_index = (int)(u * nm_image.get_width());
	int y_index = (int)(v * nm_image.get_height());

	return nm_image.get(x_index, y_index);

}

TGAColor Model::nm_tangent_value(float u, float v)
{
	int x_index = (int)(u * nm_tangent_image.get_width());
	int y_index = (int)(v * nm_tangent_image.get_height());

	return nm_tangent_image.get(x_index, y_index);

}
TGAColor Model::spec_value(float u, float v)
{
	int x_index = (int)(u * spec_image.get_width());
	int y_index = (int)(v * spec_image.get_height());

	return spec_image.get(x_index, y_index);

}


Vec3f Model::vert(int iface, int nvert)
{
	return verts_[faces_[iface][nvert]];
}

Vec3f Model::noraml(int iface, int nvert)
{
	return normals_[normal_faces_[iface][nvert]];
}

Vec3f Model::texture(int iface, int nvert)
{
	return textures_[texture_faces_[iface][nvert]];
}
