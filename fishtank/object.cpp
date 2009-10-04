#include "AquariumController.h"
#include "object.hpp"
#include <framework/debug.hpp>
#include <framework/resource.hpp>
#include "main.hpp"

using namespace boost;
using namespace std;

Object::Object(boost::shared_ptr<const Model> model, const Eigen::Vector3f& pos) :
	model(model),
	collisionRadius(this, (model->bb_h - model->bb_l).norm() * .5f),
	pos(this, pos),
	scale(this, 1.f)
{
}

void Object::draw() const
{
	glPushMatrix();
	glMultMatrixf(renderTransformation.data());

	glEnable(GL_NORMALIZE);

	model->render();

	glPopMatrix();
}

void Object::drawCollisionSphere() const
{
	const Eigen::Vector4f colour(1.f, 1.f, 1.f, .5f);
	return doDrawCollisionSphere(colour);
}

void Object::doDrawCollisionSphere(const Eigen::Vector4f& colour) const
{
	if (!model)
		return;

	if (!collisionModel)
		collisionModel = loadModel("", "icosphere-4");

	glPushMatrix();

	glMultMatrixf(collisionModelTransformation.data());

	glEnable(GL_NORMALIZE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glColor4fv(colour.data());
	collisionModel->render();

	glColor4f(1.f, 1.f, 1.f, 1.f);
	glDisable(GL_BLEND);
	glPopMatrix();
}

void Object::updateRenderTransformation()
{
	// Recreate the transformation matrix for the model if necessary
	const float s = scale;
	renderTransformation <<
		s, 0, 0, pos.x(),
		0, s, 0, pos.y(),
		0, 0, s, pos.z(),
		0, 0, 0, 1;

	if (!drawCollisionSpheres)
		return;

	// collisionModelTransformation is renderTransformation matrix scaled by collisionRadius
	const float r = collisionRadius;
	collisionModelTransformation = renderTransformation * (Eigen::Matrix4f() <<
		r, 0, 0, 0,
		0, r, 0, 0,
		0, 0, r, 0,
		0, 0, 0, 1).finished();
}

bool Object::collidingWith(const Object& object) const
{
	float const objectDistance = (object.pos - this->pos).norm();

	return objectDistance < (this->collisionRadius * this->scale + object.collisionRadius * object.scale);
}

void Object::update(const double /* dt */)
{
}
