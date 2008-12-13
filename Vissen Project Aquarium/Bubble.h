#pragma once

#include "math3.h"
#include "MS3D_ASCII.h"

class Bubble
{
private:
	double size;
	double wiggleStartX, wiggleStartZ;
	bool wiggle;
	math3::Vec3d velocity;
public:
	Bubble(math3::Vec3d startpos, double size, bool wiggle);
	~Bubble(void);

	math3::Vec3d pos;
	double pop;

	void Update(double dt);
	void Draw();
};
