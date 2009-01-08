#pragma once

#include "math3.h"
#include "MS3D_ASCII.h"

class Object
{
public:
	math3::Vec3d pos;
	Model *model;

	double scale;

	double wiggle_phase;

	Object(Model *model, double scale, const math3::Vec3d &position);
	~Object(void);

	void Update(double dt);
	void Draw();
};
