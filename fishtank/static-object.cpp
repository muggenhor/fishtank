#include "AquariumController.h"
#include <boost/bind.hpp>
#include <framework/debug.hpp>
#include <framework/resource.hpp>
#include <fstream>
#include "main.hpp"
#include "static-object.hpp"
#include <string>

using namespace boost;
using namespace std;

StaticObject::StaticObject(boost::shared_ptr<const Model> model, const std::string& propertiesFile, const Eigen::Vector3f& position) :
	Object(model, position)
{
	LoadProperties(propertiesFile);
}

void StaticObject::LoadProperties(const string& propertiesFile)
{
	string s;
	ifstream input_file((datadir + "/Objecten/" + propertiesFile + ".oif").c_str());

	//scaling
	getline(input_file, s);
	float object_height = atof(s.c_str());

	getline(input_file, s);/// randomize
	float n = atof(s.c_str());
	object_height = object_height + my_random() * n;

	if(model){
		scale = object_height / (model->bb_h.y() - model->bb_l.y());
	}else{
		scale = object_height;
		debug(LOG_ERROR) << "Trying to load properties for object that has no model, cant determine scaling";
	}
}
