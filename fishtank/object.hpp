#ifndef __INCLUDED_OBJECT_HPP__
#define __INCLUDED_OBJECT_HPP__

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <framework/property.hpp>
#include <Eigen/Core>
#include "MS3D_ASCII.h"

class Object
{
	public:
		Object(boost::shared_ptr<const Model> model, const Eigen::Vector3f& pos = Eigen::Vector3f::Zero());
		virtual ~Object() {}

		virtual void draw() const;
		virtual void drawCollisionSphere() const;

		virtual bool collidingWith(const Object& object) const;
		virtual void update(double dt);

	protected:
		void doDrawCollisionSphere(const Eigen::Vector4f& colour) const;

	private:
		template <typename T>
		const T& setRenderVar(T& var, const T& newVal)
		{
			var = newVal;
			updateRenderTransformation();
			return var;
		}
		void updateRenderTransformation();

	public:
		/**
		 * 3D model that's rendered by this @c Object
		 */
		const boost::shared_ptr<const Model> model;

		/**
		 * Collision radius.
		 */
		float collisionRadius;

		/**
		 * Position of this @c Object in space.
		 */
		Property<Eigen::Vector3f, Object, &Object::setRenderVar> pos;

		/**
		 * Scale factor to apply to the model.
		 */
		ScalarProperty<float, Object, &Object::setRenderVar> scale;

	private:
		// Cached data used for rendering
		mutable boost::shared_ptr<const Model> collisionModel;
		mutable Eigen::Matrix4f renderTransformation;
};

#endif // __INCLUDED_OBJECT_HPP__
