#ifndef __INCLUDED_LIB_SCRIPT_FISH_SUPPORT_HPP__
#define __INCLUDED_LIB_SCRIPT_FISH_SUPPORT_HPP__

#include <luabind/scope.hpp>
#include <luabind/wrapper_base.hpp>
#include "../../Vis.h"

class Fish_lua_wrapper : public Vis, public luabind::wrap_base
{
	public:
		static luabind::scope register_with_lua();

	public:
		Fish_lua_wrapper(boost::shared_ptr<const Model> model,
		                 const boost::filesystem::path& propertiesFile,
		                 int maxFloorHeight);

		virtual void draw() const;
		static  void default_draw(const Vis* base);
		virtual void drawCollisionSphere() const;
		static  void default_drawCollisionSphere(const Vis* base);

		virtual bool collidingWith(const Vis& fish) const;
		static  bool default_collidingWith(const Vis* base, const Vis& fish);
		virtual void update(double dt);
		static  void default_update(Vis* base, double dt);
};

#endif // __INCLUDED_LIB_SCRIPT_FISH_SUPPORT_HPP__
