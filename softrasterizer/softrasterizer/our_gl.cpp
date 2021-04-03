#include "our_gl.h"


Matrix view(Vec3f camera_position, Vec3f camera_lookat, Vec3f camera_up)
{
	Matrix Tview;
	//Matrix Rview;
	
	Vec3f z = (camera_position - camera_lookat).normalize();
	//g.normalize();
	/*Vec3f t = camera_up;*/
	Vec3f x = (cross(camera_up,x).normalize());
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


Matrix perspective_projection(const float n, const float f, const float fov, const float aspect_ratio)
{
	Matrix perspective;
	perspective = perspective.identity(4);

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


Matrix viewport(const size_t width, const size_t height);