#include "resource.hpp"
#include <boost/weak_ptr.hpp>
#include <map>
#include <string>

// FIXME: #includes from top level dir
#include "../../main.hpp"
#include "../../MS3D_ASCII.h"

using namespace boost;
using namespace std;

static map<string, weak_ptr<Model> > models;

boost::shared_ptr<const Model> loadModel(const std::string& dir, const std::string& model_name, const Eigen::Matrix4f& transform)
{
	string model_path;
	if (dir.substr(0, 1) == "/"
	 || dir.substr(0, 2) == "./")
		model_path = dir + "/" + model_name + ".txt";
	else
		model_path = datadir + "/" + dir + "/" + model_name + ".txt";

	map<string, weak_ptr<Model> >::iterator model_iterator = models.find(model_path);
	shared_ptr<Model> model;

	if (model_iterator != models.end())
	{
		model = model_iterator->second.lock();
		if (model)
			return model;
	}

	model.reset(new Model);
	model->loadFromMs3dAsciiFile(model_path.c_str(), transform);
	models[model_path] = model;

	return model;
}
