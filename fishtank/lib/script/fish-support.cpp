#include "fish-support.hpp"
#include <luabind/luabind.hpp>
#include <string>

using namespace std;

luabind::scope Fish_lua_wrapper::register_with_lua()
{
	using namespace luabind;

	return class_<Vis, Object, boost::shared_ptr<Vis>, Fish_lua_wrapper >("Fish")
		.def(constructor< boost::shared_ptr<const Model>, const string&, int >())
		.def("draw", &Vis::draw, &Fish_lua_wrapper::default_draw)
		.def("drawCollisionSphere", &Vis::drawCollisionSphere, &Fish_lua_wrapper::default_drawCollisionSphere)
		.def("collidingWith", &Vis::collidingWith, &Fish_lua_wrapper::default_collidingWith)
		.def("update", &Vis::update, &Fish_lua_wrapper::default_update)
		.def("Evade", &Vis::Avade)
		.def("IsGoingTowards", &Vis::IsGoingTowards)
		.def("RandomPos", &Vis::RandomPos)
		.def("LoadProperties", &Vis::LoadProperties)
		.def("newGoal", &Vis::newGoal)
		.def("setGoal", &Vis::setGoal)
		.def("setTemporaryGoal", &Vis::setTemporaryGoal)
		;
}

Fish_lua_wrapper::Fish_lua_wrapper(boost::shared_ptr<const Model> model, const boost::filesystem::path& propertiesFile, int maxFloorHeight) :
	Vis(model, propertiesFile, maxFloorHeight)
{
}

void Fish_lua_wrapper::draw() const
{
	call<void>("draw");
}

void Fish_lua_wrapper::default_draw(const Vis* base)
{
	base->Vis::draw();
}

void Fish_lua_wrapper::drawCollisionSphere() const
{
	call<void>("drawCollisionSphere");
}

void Fish_lua_wrapper::default_drawCollisionSphere(const Vis* base)
{
	base->Vis::drawCollisionSphere();
}

bool Fish_lua_wrapper::collidingWith(const Vis& fish) const
{
	return call<bool>("collidingWith", fish);
}

bool Fish_lua_wrapper::default_collidingWith(const Vis* base, const Vis& fish)
{
	return base->Vis::collidingWith(fish);
}

void Fish_lua_wrapper::update(double dt)
{
	call<void>("update", dt);
}

void Fish_lua_wrapper::default_update(Vis* base, double dt)
{
	base->Vis::update(dt);
}
