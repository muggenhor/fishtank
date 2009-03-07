#include "AquariumController.h"
#include "Object.h"
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <fstream>
#include <string>

using namespace std;

Object::Object(boost::shared_ptr<Model> model, const std::string& propertiesFile, const Eigen::Vector3d& position) :
	pos(position),
	model(model),
	wiggle_phase(0),
	wiggle_freq(0)
{
	LoadProperties(propertiesFile);
}

void Object::LoadProperties(const string &propertiesFile)
{
	string s;
	ifstream input_file(("./Data/Objecten/" + propertiesFile + ".oif").c_str());

	//scaling
	getline(input_file, s);
	double object_height = atof(s.c_str());

	getline(input_file, s);/// randomize
	float n = atof(s.c_str());
	object_height = object_height + my_random() * n;

	sphere = (model->bb_h - model->bb_l).norm();

	if(model){
		scale = object_height / (model->bb_h.y() - model->bb_l.y());
	}else{
		scale = object_height;
		std::cerr<<"Error: trying to load properties for object that has no model, cant determine scaling"<<std::endl;
	}

	//wiggle
	getline(input_file, s);
	wiggle_freq = (40.0/object_height) * atof(s.c_str()) / 100;
}

void Object::Draw() const
{
	glPushMatrix();
	glTranslatef(pos.x(), pos.y(), pos.z());

	glScalef(scale,scale,scale);
	glEnable(GL_NORMALIZE);
	model->render(Eigen::Vector3f(0.f, 0.1 * scale, 0), Eigen::Vector3f(2.0 / scale, 0.f, 0.f), wiggle_phase, 0);

	glPopMatrix();
}

void Object::Update(double dt)
{
	if (wiggle_freq == 0)
	{
		return;
	}
	wiggle_phase += wiggle_freq * dt * 20;

	/// wraparound not to lose precision over time.

	if (wiggle_phase > 2 * M_PI)
		wiggle_phase -= 2 * M_PI;
}
