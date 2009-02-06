#pragma once

#include <vector>
#include <string>
#include "math3.h"
#include "Main.h"

class Ground
{
public:
	std::vector<int> ground;
	int widthAmount, lengthAmount, maxHeight;
	GLuint texture_id;
	std::string file;
public:
	Ground(const std::string &filename, int maxHeight, const std::string &texturename="");/// use no texture name if want no texture
	~Ground(void);

	double HeightAt(int x, int y);/// note: y here is actually along "length".
	math3::Vec3d PosAt(int x, int y);
	math3::Vec3d NormalAt(int x, int y);

	void Draw();
	void GenerateGroundFromImage(const std::string &filename);
};
