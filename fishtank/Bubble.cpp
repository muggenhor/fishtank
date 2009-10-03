#include "AquariumController.h"
#include "Bubble.h"
#include <framework/resource.hpp>

Bubble::Bubble(const Eigen::Vector3f& startpos, float radius, bool wiggle) :
	radius(radius),
	wiggleStartX(my_random() * 100),
	wiggleStartZ(my_random() * 100),
	wiggle(wiggle),
	velocity(-3 + my_random() * 6, 10, -3 + my_random() * 6),
	pos(startpos),
	pop(1 + my_random() * 0.8),
	model(loadModel("", "icosphere-2"))
{
}

void Bubble::Update(double dt)
{
	pos += velocity * dt;
	if (pos.y() == aquariumSize.y() / 2)
	{
		pop -= dt;
	}
	if (pos.y() > aquariumSize.y() / 2)
	{
		pos.y() = aquariumSize.y() / 2;
		velocity.y() = 0;
	}
	else
	{
		if (wiggle)
		{
			velocity.x() = sin(pos.y() / 10 + wiggleStartX) * 20;
			velocity.z() = sin(pos.y() / 10 + wiggleStartZ) * 20;
			velocity.y() += (radius * dt) * 10;
		}
		else
		{
			velocity.y() += (radius * dt) * 5;
		}
	}
}

void Bubble::Draw() const
{
	glPushMatrix();
	glTranslatef(pos.x(), pos.y(), pos.z());
	glScalef(radius, radius, radius);

	glDisable(GL_TEXTURE_2D);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(1, 1, 1, 0.5);

	model->render();

	glDisable(GL_BLEND);

	glPopMatrix();
}
