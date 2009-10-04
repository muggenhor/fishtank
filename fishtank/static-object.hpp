#ifndef __INCLUDED_OBJECT_H__
#define __INCLUDED_OBJECT_H__

#include <boost/shared_ptr.hpp>
#include <Eigen/Core>
#include "MS3D_ASCII.h"
#include "object.hpp"
#include <string>

/**
 * An object in an aquarium (plant, rock, etc.)
 */
class StaticObject : public Object
{
	public:
		StaticObject(boost::shared_ptr<const Model> model, const std::string& propertiesFile, const Eigen::Vector3f& position);

		void LoadProperties(const std::string &propertiesFile);
		virtual void draw() const;
};

#endif // __INCLUDED_OBJECT_H__
