#ifndef __INCLUDED_LIB_FRAMEWORK_RESOURCE_HPP__
#define __INCLUDED_LIB_FRAMEWORK_RESOURCE_HPP__

#include <boost/filesystem/path.hpp>
#include <boost/shared_ptr.hpp>
#include <Eigen/Core>

// Forward declaration to allow pointers to this type
class Model;
class Texture;

boost::shared_ptr<const Texture>
  loadTexture(const boost::filesystem::path& dir,
              const boost::filesystem::path& filename);

boost::shared_ptr<const Model>
  loadModel(const boost::filesystem::path& dir,
            const std::string& model_name);

boost::shared_ptr<const Model>
  loadModel(const boost::filesystem::path& dir,
            const std::string& model_name,
            const Eigen::Matrix4f& transform);

#endif // __INCLUDED_LIB_FRAMEWORK_RESOURCE_HPP__
