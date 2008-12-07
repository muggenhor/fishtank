#pragma once

#include "math3.h"
#include "MS3D_ASCII.h"


class Vis
{
public:
	math3::Vec3d pos, goalPos, velocity;
	Model *model;
	double swimDirAngle;
	double speed;

public:
	//enum visModel{model1, model2} //this right?:S
	Vis(Model *model); //hihi
	~Vis(void);

	void Update(double dt);
	void Draw();
};
