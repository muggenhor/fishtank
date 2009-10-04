#ifndef __INCLUDED_OBJECT_H__
#define __INCLUDED_OBJECT_H__

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

		virtual void draw() const = 0;
		virtual void drawCollisionSphere() const;

		virtual bool collidingWith(const Object& object) const;
		virtual void update(double dt);

	protected:
		void doDrawCollisionSphere(const Eigen::Vector4f& colour) const;

	private:
		template <typename T>
		const T& setCollisionVar(T& var, const T& newVal)
		{
			var = newVal;
			updateCollisionModelTransformation();
			return var;
		}
		void updateCollisionModelTransformation();

	public:
		/**
		 * 3D model that's rendered by this @c Object
		 */
		const boost::shared_ptr<const Model> model;

		/**
		 * Collision radius.
		 */
		ScalarProperty<float, Object, &Object::setCollisionVar> collisionRadius;

		/**
		 * Position of this @c Object in space.
		 */
		Property<Eigen::Vector3f, Object, &Object::setCollisionVar> pos;

		/**
		 * Scale factor to apply to the model.
		 */
		ScalarProperty<float, Object, &Object::setCollisionVar> scale;

	private:
		// Cached data used for rendering the collision model
		mutable boost::shared_ptr<const Model> collisionModel;
		mutable Eigen::Matrix4f collisionModelTransformation;
};

//een object in een aquarium (plantje, steen of iets dergelijks)
class StaticObject : public Object
{
	public:
		StaticObject(boost::shared_ptr<const Model> model, const std::string& propertiesFile, const Eigen::Vector3f& position);

		void LoadProperties(const std::string &propertiesFile);
		virtual void draw() const;
};

#endif // __INCLUDED_OBJECT_H__
