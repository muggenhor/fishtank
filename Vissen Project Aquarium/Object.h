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
	double wiggle_freq;

	Object(Model *model, const std::string &propertiesFile, const math3::Vec3d &position);
	~Object(void);

	void LoadProperties(const std::string &propertiesFile);
	void Update(double dt);
	void Draw();
};
