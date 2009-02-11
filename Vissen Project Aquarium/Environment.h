#pragma once

#include <vector>
#include <string>
#include "math3.h"
#include "Main.h"

class Environment
{
public:
	GLuint texture_id;
	math3::Vec3d pos1, pos2, pos3, pos4;
public:
	Environment(const math3::Vec3d &pos1, const math3::Vec3d &pos2, const math3::Vec3d &pos3, const math3::Vec3d &pos4, const std::string &texturename="");/// use no texture name if want no texture
	~Environment(void);

	void Draw();
};