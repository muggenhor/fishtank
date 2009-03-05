#include "AquariumController.h"
#include "Bubble.h"

using namespace math3;

GLUquadric* TheQuadric()
{
 	static GLUquadric* result=gluNewQuadric();
 	return result;
}

Bubble::Bubble(Vec3d startpos, double size, bool wiggle)
{
	pos = startpos;
	this->size = size;
	velocity = Vec3d(-3 + my_random() * 6, 10, -3 + my_random() * 6);
	this->wiggle = wiggle;
	if (wiggle)
	{
		wiggleStartX = my_random() * 100;
		wiggleStartZ = my_random() * 100;
	}
	pop = 1 + my_random() * 0.8;
}

void Bubble::Update(double dt)
{
	pos += velocity*dt;
	if (pos.y == aquariumSize.y / 2)
	{
		pop -= dt;
	}
	if (pos.y > aquariumSize.y / 2)
	{
		pos.y = aquariumSize.y / 2;
		velocity.y = 0;
	}
	else
	{
		if (wiggle)
		{
			velocity.x = sin(pos.y / 10 + wiggleStartX) * 20;
			velocity.z = sin(pos.y / 10 + wiggleStartZ) * 20;
			velocity.y += (size * dt) * 10;
		}
		else
		{
			velocity.y += (size * dt) * 5;
		}
	}
}

void Bubble::Draw() const
{
	glPushMatrix();
	glTranslatef(pos.x,pos.y,pos.z);
	//glColor3f(1,1,1);

	glDisable(GL_TEXTURE_2D);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(1,1,1,0.5);

	gluSphere(TheQuadric(), size / 2, 20, 20);

	glDisable(GL_BLEND);

	glPopMatrix();
}
