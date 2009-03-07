#ifndef __INCLUDED_OBJECT_H__
#define __INCLUDED_OBJECT_H__

#include <boost/shared_ptr.hpp>
#include <Eigen/Core>
#include "MS3D_ASCII.h"

//een object in een aquarium (plantje, steen of iets dergelijks)
class Object
{
	public:
		Eigen::Vector3d pos;
		//het model
		boost::shared_ptr<Model> model;

		//de hoogte
		double scale;

		//voor de vibratie
		double wiggle_phase;
		double wiggle_freq;

		//de botsarea
		int sphere;

		Object(boost::shared_ptr<Model> model, const std::string& propertiesFile, const Eigen::Vector3d& position);
		//haalt info uit de file, gegeven als path
		void LoadProperties(const std::string &propertiesFile);
		//update voor de wiggle
		void Update(double dt);
		//teken het object
		void Draw() const;
};

#endif // __INCLUDED_OBJECT_H__
