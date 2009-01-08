#pragma once

#include "math3.h"
#include "MS3D_ASCII.h"


class Vis
{
public:
	math3::Vec3d pos, goalPos, finalGoalPos, velocity;
	bool usingTempGoal;

	Model *model;
	double swimDirAngle;
	double desired_speed;
	double speed;
	double turn_speed;
	double bending;
	double pitch;

	//instellingen
	double max_speed;
	double min_speed;
	
	double max_turn_speed;///radians per second
	double turn_acceleration;///radians per second per second
	
	double wiggle_factor;/// amount of wiggle displacement (controls amplitude vs speed), larger, fish bends more for wiggling
	double wiggle_freq;///larger = means more wiggle waves on fish.
	//instellingen end

	double scale;

	double myWaitTime;

	double wiggle_phase, wiggle_amplitude;

public:
	//enum visModel{model1, model2} //this right?:S
	Vis(Model *model, const std::string &propertiesFile); //hihi
	~Vis(void);

	void Update(double dt);
	void Draw();
	void LoadProperties(const std::string &propertiesFile);
	void newGoal();
	void setGoal(const math3::Vec3d &final_goal);/// use that to set goal
	void setTemporaryGoal(const math3::Vec3d &temp_goal);/// use that to set temporary goal when avoiding collision or something.

};

void TestDrawAquarium();

float my_random();
