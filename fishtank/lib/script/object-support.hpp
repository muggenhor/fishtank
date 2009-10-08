#ifndef __INCLUDED_LIB_SCRIPT_OBJECT_SUPPORT_HPP__
#define __INCLUDED_LIB_SCRIPT_OBJECT_SUPPORT_HPP__

#include <luabind/scope.hpp>
#include <luabind/wrapper_base.hpp>
#include "../../object.hpp"

class Object_lua_wrapper : public Object, public luabind::wrap_base
{
	public:
		static luabind::scope register_with_lua();

	public:
		Object_lua_wrapper(boost::shared_ptr<const Model> model = boost::shared_ptr<const Model>(),
		                   const Eigen::Vector3f& pos = Eigen::Vector3f::Zero());

		virtual void draw() const;
		static  void default_draw(const Object* base);
		virtual void drawCollisionSphere() const;
		static  void default_drawCollisionSphere(const Object* base);

		virtual bool collidingWith(const Object& object) const;
		static  bool default_collidingWith(const Object* base, const Object& object);
		virtual void update(double dt);
		static  void default_update(Object* base, double dt);

		// Required to allow calls to this function from Lua (i.e. circumvent the 'protected' state)
		void doDrawCollisionSphere(const Eigen::Vector4f& colour) const;

		// Required because of the Property and ScalarProperty classes
		static float           get_collisionRadius(const Object* o);
		static void            set_collisionRadius(Object* o, float rhs);
		static Eigen::Vector3f& get_pos(Object* o);
		static void            set_pos(Object* o, const Eigen::Vector3f& rhs);
		static float           get_scale(const Object* o);
		static void            set_scale(Object* o, float rhs);
};

#endif // __INCLUDED_LIB_SCRIPT_OBJECT_SUPPORT_HPP__
