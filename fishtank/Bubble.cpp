#include "aquarium.hpp"
#include "Bubble.h"
#include <framework/resource.hpp>

Bubble::Bubble(const Aquarium& aquarium, const Eigen::Vector3f& startpos, float radius, bool wiggle) :
	Object(loadModel("", "icosphere-2"), startpos),
	wiggleStartX(my_random() * 100),
	wiggleStartZ(my_random() * 100),
	wiggle(wiggle),
	velocity(-3 + my_random() * 6, 10, -3 + my_random() * 6),
	pop(1 + my_random() * 0.8),
	aquarium(aquarium)
{
	scale = radius;
}

void Bubble::update(double dt)
{
	pos += velocity * dt;
	if (pos.y() >= aquarium.size().y() / 2.f)
	{
		pop -= dt;
		pos.y() = aquarium.size().y() / 2.f;
		velocity.y() = 0.f;
		return;
	}

	if (wiggle)
	{
		velocity.x() = sin(pos.y() / 10 + wiggleStartX) * 20;
		velocity.z() = sin(pos.y() / 10 + wiggleStartZ) * 20;
		velocity.y() += scale * dt * 10.f;
	}
	else
	{
		velocity.y() += scale * dt * 5.f;
	}
}

void Bubble::draw() const
{
	glDisable(GL_TEXTURE_2D);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(1.f, 1.f, 1.f, .5f);

	Object::draw();

	glDisable(GL_BLEND);
}
