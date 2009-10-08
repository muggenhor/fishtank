#include "object-support.hpp"
#include <luabind/luabind.hpp>

luabind::scope Object_lua_wrapper::register_with_lua()
{
	using namespace luabind;

	return class_<Object, boost::shared_ptr<Object>, Object_lua_wrapper >("Object")
		.def(constructor<>())
		.def(constructor< boost::shared_ptr<const Model> >())
		.def(constructor< boost::shared_ptr<const Model>, const Eigen::Vector3f& >())
		.def("draw", &Object::draw, &Object_lua_wrapper::default_draw)
		.def("drawCollisionSphere", &Object::drawCollisionSphere, &Object_lua_wrapper::default_drawCollisionSphere)
		.def("collidingWith", &Object::collidingWith, &Object_lua_wrapper::default_collidingWith)
		.def("update", &Object::update, &Object_lua_wrapper::default_update)
		.def("doDrawCollisionSphere", &Object_lua_wrapper::doDrawCollisionSphere)
		.def_readonly("model", &Object::model)
		.property("collisionRadius", &Object_lua_wrapper::get_collisionRadius, &Object_lua_wrapper::set_collisionRadius)
		.property("pos", &Object_lua_wrapper::get_pos, &Object_lua_wrapper::set_pos)
		.property("scale", &Object_lua_wrapper::get_scale, &Object_lua_wrapper::set_scale)
		;
}

Object_lua_wrapper::Object_lua_wrapper(boost::shared_ptr<const Model> model, const Eigen::Vector3f& pos) :
	Object(model, pos)
{
}

void Object_lua_wrapper::draw() const
{
	call<void>("draw");
}

void Object_lua_wrapper::default_draw(const Object* base)
{
	base->Object::draw();
}

void Object_lua_wrapper::drawCollisionSphere() const
{
	call<void>("drawCollisionSphere");
}

void Object_lua_wrapper::default_drawCollisionSphere(const Object* base)
{
	base->Object::drawCollisionSphere();
}

bool Object_lua_wrapper::collidingWith(const Object& object) const
{
	return call<bool>("collidingWith", object);
}

bool Object_lua_wrapper::default_collidingWith(const Object* base, const Object& object)
{
	return base->Object::collidingWith(object);
}

void Object_lua_wrapper::update(double dt)
{
	call<void>("update", dt);
}

void Object_lua_wrapper::default_update(Object* base, double dt)
{
	base->Object::update(dt);
}

// Required to allow calls to this function from Lua (i.e. circumvent the 'protected' state)
void Object_lua_wrapper::doDrawCollisionSphere(const Eigen::Vector4f& colour) const
{
	return Object::doDrawCollisionSphere(colour);
}

// Required because of the Property and ScalarProperty classes
float Object_lua_wrapper::get_collisionRadius(const Object* o)
{
	return o->collisionRadius;
}

void  Object_lua_wrapper::set_collisionRadius(Object* o, float rhs)
{
	o->collisionRadius = rhs;
}

Eigen::Vector3f& Object_lua_wrapper::get_pos(Object* o)
{
	return o->pos;
}

void  Object_lua_wrapper::set_pos(Object* o, const Eigen::Vector3f& rhs)
{
	o->pos = rhs;
}

float Object_lua_wrapper::get_scale(const Object* o)
{
	return o->scale;
}

void  Object_lua_wrapper::set_scale(Object* o, float rhs)
{
	o->scale = rhs;
}
