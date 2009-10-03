#ifndef __INCLUDED_LIB_FRAMEWORK_RESOURCE_HPP__
#define __INCLUDED_LIB_FRAMEWORK_RESOURCE_HPP__

#include <boost/shared_ptr.hpp>
#include <Eigen/Core>

// Forward declaration to allow pointers to this type
class Model;

boost::shared_ptr<const Model>
  loadModel(const std::string& dir,
            const std::string& model_name,
            const Eigen::Matrix4f& transform = Eigen::Matrix4f::Identity());

#endif // __INCLUDED_LIB_FRAMEWORK_RESOURCE_HPP__
