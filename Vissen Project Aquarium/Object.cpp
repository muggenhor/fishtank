#include "AquariumController.h"
#include "Object.h"
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <string>
using namespace math3;
using namespace std;

Object::Object(Model *model, const std::string &propertiesFile, const math3::Vec3d &position)
{
	this->model = model;

	pos = position;

	wiggle_phase = 0;
	wiggle_freq = 0;

	LoadProperties(propertiesFile);
}

Object::~Object(void)
{
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
	object_height  = object_height  + my_random() * n;

	if(model){
		scale = object_height / ( model->bb_h.y - model->bb_l.y );
	}else{
		scale = object_height;
		std::cerr<<"Error: trying to load properties for object that has no model, cant determine scaling"<<std::endl;
	}

	//wiggle
	getline(input_file, s);
	wiggle_freq = (40.0/object_height) * atof(s.c_str()) / 100;
}

void Object::Draw()
{
	glPushMatrix();
	glTranslatef(pos.x,pos.y,pos.z);

	glScalef(scale,scale,scale);
	glEnable(GL_NORMALIZE);
	model->render(Vec3f(0,0.1 * scale,0), Vec3f(2.0 / scale,0,0),wiggle_phase, 0);

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

	if(wiggle_phase>2*pi)wiggle_phase-=2*pi;
}
