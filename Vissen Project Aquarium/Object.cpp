#include "AquariumController.h"
#include "Object.h"
#include <cstdlib>
#include <ctime>
using namespace math3;

Object::Object(Model *model, double scale, const math3::Vec3d &position)
{
	this->model = model;
	this->scale = scale;

	pos = position;

	wiggle_phase = 0;
}

Object::~Object(void)
{
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
	wiggle_phase += 2.0 * dt;

	/// wraparound not to lose precision over time.

	if(wiggle_phase>2*pi)wiggle_phase-=2*pi;
}