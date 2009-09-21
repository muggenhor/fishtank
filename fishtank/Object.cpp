#include "AquariumController.h"
#include "Object.h"
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <framework/debug.hpp>
#include <fstream>
#include "main.hpp"
#include <string>

using namespace std;

Object::Object(boost::shared_ptr<Model> model, const std::string& propertiesFile, const Eigen::Vector3d& position) :
	model(model),
	pos(position)
{
	LoadProperties(propertiesFile);
}

void Object::LoadProperties(const string& propertiesFile)
{
	string s;
	ifstream input_file((datadir + "/Objecten/" + propertiesFile + ".oif").c_str());

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
		debug(LOG_ERROR) << "Trying to load properties for object that has no model, cant determine scaling";
	}
}

void Object::Draw() const
{
	glPushMatrix();
	glTranslatef(pos.x(), pos.y(), pos.z());

	glScalef(scale, scale, scale);
	glEnable(GL_NORMALIZE);

	model->render();

	glPopMatrix();
}

void Object::Update(const double /* dt */)
{
}
