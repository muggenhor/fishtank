#include "resource.hpp"
#include <boost/weak_ptr.hpp>
#include <map>
#include <string>
#include "vfs.hpp"

// FIXME: #includes from top level dir
#include "../../image.hpp"
#include "../../main.hpp"
#include "../../MS3D_ASCII.h"
#include "../../textures.hpp"

using namespace boost;
using namespace boost::gil;
using namespace std;
namespace fs = boost::filesystem;
using boost::system::error_code;
using boost::system::get_system_category;

static map<string, weak_ptr<Model> > models;
static map<string, weak_ptr<Texture> > textures;

boost::shared_ptr<const Texture>
  loadTexture(const boost::filesystem::path& dir,
              const boost::filesystem::path& filename)
{
	fs::path path;
	if (dir.has_root_path()
	 || dir.relative_path().string().substr(0, 2) == "./")
		path = dir / filename;
	else
		path = datadir / dir / filename;

	map<string, weak_ptr<Texture> >::iterator texture_iterator = textures.find(path.file_string());
	shared_ptr<Texture> texture;

	if (texture_iterator != textures.end())
	{
		texture = texture_iterator->second.lock();
		if (texture)
			return texture;
	}

	if (!vfs::allow_read(path))
		throw fs::filesystem_error("Cannot open file for reading", path, error_code(EACCES, get_system_category()));

	rgb8_image_t img;
	read_image(path, img);

	texture.reset(new Texture(flipped_up_down_view(const_view(img))));
	textures[path.file_string()] = texture;

	return texture;
}

boost::shared_ptr<const Model> loadModel(const boost::filesystem::path& dir, const std::string& model_name)
{
	return loadModel(dir, model_name, Eigen::Matrix4f::Identity());
}

boost::shared_ptr<const Model> loadModel(const boost::filesystem::path& dir, const std::string& model_name, const Eigen::Matrix4f& transform)
{
	fs::path model_path;
	if (dir.has_root_path()
	 || dir.relative_path().string().substr(0, 2) == "./")
		model_path = dir / (model_name + ".txt");
	else
		model_path = datadir / dir / (model_name + ".txt");

	map<string, weak_ptr<Model> >::iterator model_iterator = models.find(model_path.file_string());
	shared_ptr<Model> model;

	if (model_iterator != models.end())
	{
		model = model_iterator->second.lock();
		if (model)
			return model;
	}

	if (!vfs::allow_read(model_path))
		throw fs::filesystem_error("Cannot open file for reading", model_path, error_code(EACCES, get_system_category()));

	model.reset(new Model);
	model->loadFromMs3dAsciiFile(model_path, transform);
	models[model_path.file_string()] = model;

	return model;
}
