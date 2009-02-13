#pragma once

#include "math3.h"
#include "MS3D_ASCII.h"

//een object in een aquarium (plantje, steen of iets dergelijks)
class Object
{
	public:
		math3::Vec3d pos;
		//het model
		Model *model;

		//de hoogte
		double scale;

		//voor de vibratie
		double wiggle_phase;
		double wiggle_freq;

		//de botsarea
		int sphere;

		Object(Model *model, const std::string &propertiesFile, const math3::Vec3d &position);
		~Object(void);

		//haalt info uit de file, gegeven als path
		void LoadProperties(const std::string &propertiesFile);
		//update voor de wiggle
		void Update(double dt);
		//teken het object
		void Draw();
};
