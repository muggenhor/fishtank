#include <boost/gil/image.hpp>
#include <boost/gil/typedefs.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>
#include <boost/regex.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include "camera.hpp"
#include "GL/GLee.h"
#include <GL/glfw.h>
#include <GL/gl.h>
#include <Eigen/Core>
#include "framerate.hpp"
#include "main.hpp"
#include "Vis.h"
#include "AquariumController.h"
#include "MS3D_ASCII.h"
#include <fstream>
#include <string>

#include <cmath>
#include <cstdlib>
#include <ctime>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>
#include <map>

#include "imagereceiver.h"
#include "glexcept.hpp"

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

// Allow for easy adding of translations
#define _(string) (string)

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

using namespace std;
namespace po = boost::program_options;

/// List of data directories to select from
static const char* datadirs[] =
{
	"./Data", // Default fallback
#if defined(DATADIR)
	DATADIR,
#endif
};

/// List of configuration derectories to select from
static const char* sysconfdirs[] =
{
	"./Settings", // Default fallback
#if defined(SYSCONFDIR)
	SYSCONFDIR,
#endif
};

std::string datadir;

bool use_vbos = true;

/* The amount of additional samples per pixel to generate. Higher amounts will
 * generally produce better anti aliasing results.
 */
static unsigned int multi_sample = 4;

/**
 * The camera number to select.
 */
static int cameraIndex = -1;

static std::vector<Transformation> cameraTransformations;

/**
 * The camera resolution to use.
 */
static Eigen::Vector2i cameraResolution(320, 240);

/**
 * Controls the framerate and contains the framerate setting.
 */
static FrameRateManager fps(30);

//scherm resolutie
static int win_width=0, win_height=0;
//de schermpositie
static int win_move_x = 5, win_move_y = 30;

//oogafstand van het aquarium
static float eye_distance=300;

namespace boost { namespace program_options {

/**
 * Enables passing @c Eigen::Vector2i as a command line argument.
 */
template <>
void validate<Eigen::Vector2i, char>(boost::any& v, const std::vector<std::string>& values, Eigen::Vector2i*, long)
{
	using boost::lexical_cast;
	using boost::regex;
	using boost::regex_match;
	using boost::smatch;

	// Make sure no previous assignment to 'a' was made.
	po::validators::check_first_occurrence(v);
	// Extract the first string from 'values'. If there is more than
	// one string, it's an error, and exception will be thrown.
	const std::string& s = po::validators::get_single_string(values);

	static const regex r("(-?\\d+)[,xX](-?\\d+)");
	smatch match;
	if (regex_match(s, match, r))
	{
		v = Eigen::Vector2i(lexical_cast<int>(match[1]), lexical_cast<int>(match[2]));
	}
	else
	{
		throw po::validation_error("invalid value");
	}
}
}}

static void ParseOptions(int argc, char** argv, std::istream& config_file)
{
	// Group of generic options that are only allowed on the command line
	po::options_description generic("Generic options");
	generic.add_options()
	    ("version", _("Show version information and exit"))
	    ("help,h", _("Show this help message and exit"))
	;

	/* Group of options that will be allowed both on the command line and
	 * in the config file.
	 */
	po::options_description config("Configuration");
	config.add_options()
	    ("anti-aliasing", po::value<unsigned int>(&multi_sample)->default_value(multi_sample),
	      _("Set the level of anti aliasing to use (0 disables anti aliasing)."))
	    ("cam-flip-up-down", _("Flip the camera's image up-down."))
	    ("cam-rotate-90ccw", _("Rotate the camera's image 90 degrees counter clockwise."))
	    ("camera", po::value<int>(&cameraIndex)->default_value(cameraIndex),
	      _("The camera number to use (-1 uses the first available camera)."))
	    ("camera-resolution", po::value<Eigen::Vector2i>(&cameraResolution)->default_value(cameraResolution),
	      _("The resolution to use for the camera."))
	    ("fps", po::value<unsigned int>()->default_value(fps.targetRate()),
	      _("Set the target framerate, the program will not exceed a rendering rate of this amount in Hz."))
	    ("window-size", po::value<Eigen::Vector2i>(),
	      _("Dimensions to use for the window."))
	    ("window-position", po::value<Eigen::Vector2i>(),
	      _("Top-left position to use for the window."))
	    ("vbo", po::value<bool>(&use_vbos)->default_value(use_vbos),
	      _("Enable or disable the use of VBOs for rendering."))
	;

	/* Hidden options, will be allowed both on the command line and in the
	 * config file, but will not be shown to the user.
	 */
	po::options_description hidden("Hidden options");
	hidden.add_options()
	;

	po::options_description cmdline_options;
	cmdline_options.add(generic).add(config).add(hidden);

	po::options_description config_file_options;
	config_file_options.add(config).add(hidden);

	po::options_description visible("Allowed options");
	visible.add(generic).add(config);

	po::variables_map vm;
	if (argv != NULL)
		po::store(po::parse_command_line(argc, argv, cmdline_options), vm);
	po::store(po::parse_config_file(config_file, cmdline_options), vm);
	po::notify(vm);

	if (vm.count("help"))
	{
		cerr << visible;
		throw exit_exception(EXIT_FAILURE);
	}

	if (vm.count("version"))
	{
		cerr << "Fishtank - Version <UNSPECIFIED>\n"
		        "Created by Jasper Lammers and Dmytry Lavrov\n";
		throw exit_exception(EXIT_FAILURE);
	}

	if (vm.count("fps"))
	{
		fps.targetRate(vm["fps"].as<unsigned int>());
	}

	if (vm.count("window-size"))
	{
		win_width = vm["window-size"].as<Eigen::Vector2i>().x();
		win_height = vm["window-size"].as<Eigen::Vector2i>().y();
	}

	if (vm.count("window-position"))
	{
		win_move_x = vm["window-position"].as<Eigen::Vector2i>().x();
		win_move_y = vm["window-position"].as<Eigen::Vector2i>().y();
	}

	if (vm.count("cam-flip-up-down"))
	{
		cameraTransformations.push_back(Transformation(Transformation::FLIP_UP_DOWN));
	}

	if (vm.count("cam-rotate-90ccw"))
	{
		cameraTransformations.push_back(Transformation(Transformation::ROTATE_90CCW));
	}
}

//laad de settings uit het opgegeven bestand
static void LoadSettings(std::istream& input_file)
{
	string s;

	getline(input_file, s);
	win_width = atoi(s.c_str());
	getline(input_file, s);
	win_height = atoi(s.c_str());

	getline(input_file, s);
	win_move_x = atoi(s.c_str());
	getline(input_file, s);
	win_move_y = atoi(s.c_str());

	getline(input_file, s);
	int xx = atoi(s.c_str());
	getline(input_file, s);
	int yy = atoi(s.c_str());
	getline(input_file, s);
	int zz = atoi(s.c_str());
	aquariumSize = Eigen::Vector3d(xx, yy, zz);

	getline(input_file, s);
	xx = atoi(s.c_str());
	getline(input_file, s);
	yy = atoi(s.c_str());
	getline(input_file, s);
	zz = atoi(s.c_str());
	swimArea = Eigen::Vector3d(xx, yy, zz);

	getline(input_file, s);
	balkSize2 = atoi(s.c_str());
	if (balkSize2 >= 1)
	{
		balkSize = balkSize2 - 1;
	}
	else
	{
		balkSize = balkSize2;
	}

	getline(input_file, s);
	eye_distance = atof(s.c_str());

	getline(input_file, s);
	faceRange.x() = static_cast<double>(atoi(s.c_str())) / 100.;
	getline(input_file, s);
	faceRange.y() = static_cast<double>(atoi(s.c_str())) / 100.;
}

//laad de modelen uit het opgegeven bestand
static void LoadModels(std::istream& input_file, AquariumController& aquariumController)
{
	Eigen::Matrix4f model_matrix;
	{
		Eigen::Matrix4f& m = model_matrix;
		m(0, 0) = -1; m(1, 0) = 0; m(2, 0) = 0; m(3, 0) = 0;
		m(0, 1) =  0; m(1, 1) = 0; m(2, 1) = 1; m(3, 1) = 0;
		m(0, 2) =  0; m(1, 2) = 1; m(2, 2) = 0; m(3, 2) = 0;
		m(0, 3) =  0; m(1, 3) = 0; m(2, 3) = 0; m(3, 3) = 1;
	}

	map<string, boost::shared_ptr<Model> > models;

	string s;

	getline(input_file, s);
	aquariumController.ground.maxHeight = atoi(s.c_str());
	aquariumController.ground.updateRenderData();

	getline(input_file, s);
	int n=atoi(s.c_str());

	for (int i = 0; i < n; i++)
	{
		string model_name;
		getline(input_file, model_name);
		map<string, boost::shared_ptr<Model> >::iterator model_iterator = models.find(model_name);
		//model bestaat niet
		if(model_iterator==models.end())
		{
			boost::shared_ptr<Model> model(new Model);
			model->loadFromMs3dAsciiFile((datadir + "/Vissen/Modellen/" + model_name + ".txt").c_str(), model_matrix);
			models[model_name] = model;
		}

		string propertieFile;
		getline(input_file, propertieFile);

		getline(input_file, s);
		int m=atoi(s.c_str());
		for (int j = 0; j < m; j++)
		{
			aquariumController.AddFish(models[model_name], propertieFile);
		}
	}


	getline(input_file, s);
	n=atoi(s.c_str());
	for (int i = 0; i < n; i++)
	{
		string model_name;
		getline(input_file, model_name);
		map<string, boost::shared_ptr<Model> >::iterator model_iterator = models.find(model_name);
		//model bestaat niet
		if(model_iterator==models.end())
		{
			boost::shared_ptr<Model> model(new Model);
			model->loadFromMs3dAsciiFile((datadir + "/Objecten/Modellen/" + model_name + ".txt").c_str(), model_matrix);
			models[model_name] = model;
		}

		string propertieFile;
		getline(input_file, propertieFile);

		getline(input_file, s);
		int x = -(aquariumSize.x() / 2) + atoi(s.c_str());
		getline(input_file, s);
		int z = -(aquariumSize.z() / 2) + atoi(s.c_str());
		int groundposx = (x + (aquariumSize.x() / 2)) / aquariumSize.x() * (aquariumController.ground.width());
		int groundposy = (z + (aquariumSize.z() / 2)) / aquariumSize.z() * (aquariumController.ground.depth());
		aquariumController.AddObject(models[model_name], propertieFile, Eigen::Vector3d(x, aquariumController.ground.HeightAt(groundposx, groundposy), z));
	}

	getline(input_file, s);
	n=atoi(s.c_str());
	for (int i = 0; i < n; i++)
	{
		getline(input_file, s);
		int x = -(aquariumSize.x() / 2) + atoi(s.c_str());
		getline(input_file, s);
		int z = -(aquariumSize.z() / 2) + atoi(s.c_str());
		int groundposx = (x + (aquariumSize.x() / 2)) / aquariumSize.x() * (aquariumController.ground.width());
		int groundposy = (z + (aquariumSize.z() / 2)) / aquariumSize.z() * (aquariumController.ground.depth());
		aquariumController.AddBubbleSpot(Eigen::Vector3f(x, aquariumController.ground.HeightAt(groundposx, groundposy), z));
	}
}
static boost::shared_ptr<Camera> webcam;
static boost::weak_ptr<Texture> webcam_texture;

static boost::shared_ptr<ImageReceiver> image_receiver;
static boost::shared_ptr<ImageReceiver> image_receiver2;

static boost::shared_ptr<PositionReceiver> position_receiver;
static boost::shared_ptr<PositionReceiver> faceposition_receiver;

enum CAMERA
{
	LEFT_CAMERA,
	RIGHT_CAMERA,
};

/**
 * Draw the webcam screens en beams, depending on the current camera position.
 */
static void DrawBackground(CAMERA camera)
{
	glEnable(GL_TEXTURE_2D);

	boost::shared_ptr<Texture> _webcam_texture(webcam_texture);

	switch (camera)
	{
		case LEFT_CAMERA:
			if (_webcam_texture)
			{
				if (!webcam)
				{
					webcam.reset(new Camera(cameraResolution.x(), cameraResolution.y(), cameraTransformations.begin(), cameraTransformations.end(), cameraIndex));
				}

				webcam->update_texture(*_webcam_texture);
				_webcam_texture->bind();
			}
			else
			{
				glBindTexture(GL_TEXTURE_2D, image_receiver->TextureID());
			}
			break;

		case RIGHT_CAMERA:
			glBindTexture(GL_TEXTURE_2D, image_receiver2->TextureID());
			break;
	}

	//zet kleur naar wit
	glColor3f(1,1,1);

	glBegin(GL_QUADS);

	switch (camera)
	{
		case LEFT_CAMERA:
			glTexCoord2f(0, 0);
			glVertex3f(0.5*aquariumSize.x(), -0.5*aquariumSize.y() + balkSize, -0.5*aquariumSize.z() + balkSize);

			glTexCoord2f(0, 1);
			glVertex3f(0.5*aquariumSize.x(), 0.5*aquariumSize.y() - balkSize, -0.5*aquariumSize.z() + balkSize);

			glTexCoord2f(1, 1);
			glVertex3f(0.5*aquariumSize.x(), 0.5*aquariumSize.y() - balkSize, 0.5*aquariumSize.z() - balkSize);

			glTexCoord2f(1, 0);
			glVertex3f(0.5*aquariumSize.x(), -0.5*aquariumSize.y() + balkSize, 0.5*aquariumSize.z() - balkSize);
			break;

		case RIGHT_CAMERA:
			glTexCoord2f(1, 0);
			glVertex3f(-0.5*aquariumSize.x() + balkSize, -0.5*aquariumSize.y() + balkSize, 0.5*aquariumSize.z());

			glTexCoord2f(1, 1);
			glVertex3f(-0.5*aquariumSize.x() + balkSize, 0.5*aquariumSize.y() - balkSize, 0.5*aquariumSize.z());

			glTexCoord2f(0, 1);
			glVertex3f(0.5*aquariumSize.x() - balkSize, 0.5*aquariumSize.y() - balkSize, 0.5*aquariumSize.z());

			glTexCoord2f(0, 0);
			glVertex3f(0.5*aquariumSize.x() - balkSize, -0.5*aquariumSize.y() + balkSize, 0.5*aquariumSize.z());
			break;
	}

	glEnd();
	glDisable(GL_TEXTURE_2D);

	glColor3f(0,0,0);

	glBegin(GL_QUADS);

	if (balkSize2 != 0)
	{
		switch (camera)
		{
			case LEFT_CAMERA:
				//onder bij klein scherm
				glVertex3f(0.5 * aquariumSize.x() - 1, -0.5 * aquariumSize.y() + balkSize2, -0.5 * aquariumSize.z());
				glVertex3f(0.5 * aquariumSize.x() - 1, -0.5 * aquariumSize.y(), -0.5 * aquariumSize.z());
				glVertex3f(0.5 * aquariumSize.x() - 1, -0.5 * aquariumSize.y(), 0.5 * aquariumSize.z());
				glVertex3f(0.5 * aquariumSize.x() - 1, -0.5 * aquariumSize.y() + balkSize2, 0.5 * aquariumSize.z());

				//boven bij klein scherm
				glVertex3f(0.5 * aquariumSize.x() - 1, 0.5 * aquariumSize.y() - balkSize2, -0.5 * aquariumSize.z());
				glVertex3f(0.5 * aquariumSize.x() - 1, 0.5 * aquariumSize.y(), -0.5 * aquariumSize.z());
				glVertex3f(0.5 * aquariumSize.x() - 1, 0.5 * aquariumSize.y(), 0.5 * aquariumSize.z());
				glVertex3f(0.5 * aquariumSize.x() - 1, 0.5 * aquariumSize.y() - balkSize2, 0.5 * aquariumSize.z());

				//rechts bij klein scherm
				glVertex3f(0.5 * aquariumSize.x() - 1, -0.5 * aquariumSize.y(), -0.5 * aquariumSize.z() + balkSize2);
				glVertex3f(0.5 * aquariumSize.x() - 1, -0.5 * aquariumSize.y(), -0.5 * aquariumSize.z());
				glVertex3f(0.5 * aquariumSize.x() - 1, 0.5 * aquariumSize.y(), -0.5 * aquariumSize.z());
				glVertex3f(0.5 * aquariumSize.x() - 1, 0.5 * aquariumSize.y(), -0.5 * aquariumSize.z() + balkSize2);

				//links bij klein scherm
				glVertex3f(0.5 * aquariumSize.x() - 1, -0.5 * aquariumSize.y(), 0.5 * aquariumSize.z() - balkSize2);
				glVertex3f(0.5 * aquariumSize.x() - 1, -0.5 * aquariumSize.y(), 0.5 * aquariumSize.z());
				glVertex3f(0.5 * aquariumSize.x() - 1, 0.5 * aquariumSize.y(), 0.5 * aquariumSize.z());
				glVertex3f(0.5 * aquariumSize.x() - 1, 0.5 * aquariumSize.y(), 0.5 * aquariumSize.z() - balkSize2);
				break;

			case RIGHT_CAMERA:
				//dwarsbalk bij groot scherm
				glVertex3f(-balkSize2, -0.5 * aquariumSize.y() + balkSize2, 0.5 * aquariumSize.z() - 1);
				glVertex3f(-balkSize2, 0.5 * aquariumSize.y() - balkSize2, 0.5 * aquariumSize.z() - 1);
				glVertex3f(balkSize2, 0.5 * aquariumSize.y() - balkSize2, 0.5 * aquariumSize.z() - 1);
				glVertex3f(balkSize2, -0.5 * aquariumSize.y() + balkSize2, 0.5 * aquariumSize.z() - 1);
				
				//onder bij groot scherm
				glVertex3f(-0.5 * aquariumSize.x(), -0.5 * aquariumSize.y() + balkSize2, 0.5 * aquariumSize.z() - 1);
				glVertex3f(-0.5 * aquariumSize.x(), -0.5 * aquariumSize.y(), 0.5 * aquariumSize.z() - 1);
				glVertex3f(0.5 * aquariumSize.x(), -0.5 * aquariumSize.y(), 0.5 * aquariumSize.z() - 1);
				glVertex3f(0.5 * aquariumSize.x(), -0.5 * aquariumSize.y() + balkSize2, 0.5 * aquariumSize.z() - 1);

				//boven bij groot scherm
				glVertex3f(-0.5 * aquariumSize.x(), 0.5 * aquariumSize.y() - balkSize2, 0.5 * aquariumSize.z() - 1);
				glVertex3f(-0.5 * aquariumSize.x(), 0.5 * aquariumSize.y(), 0.5 * aquariumSize.z() - 1);
				glVertex3f(0.5 * aquariumSize.x(), 0.5 * aquariumSize.y(), 0.5 * aquariumSize.z() - 1);
				glVertex3f(0.5 * aquariumSize.x(), 0.5 * aquariumSize.y() - balkSize2, 0.5 * aquariumSize.z() - 1);

				//rechts bij groot scherm
				glVertex3f(0.5 * aquariumSize.x() - balkSize2, -0.5 * aquariumSize.y(), 0.5 * aquariumSize.z() - 1);
				glVertex3f(0.5 * aquariumSize.x(), -0.5 * aquariumSize.y(), 0.5 * aquariumSize.z() - 1);
				glVertex3f(0.5 * aquariumSize.x(), 0.5 * aquariumSize.y(), 0.5 * aquariumSize.z() - 1);
				glVertex3f(0.5 * aquariumSize.x() - balkSize2, 0.5 * aquariumSize.y(), 0.5 * aquariumSize.z() - 1);

				//links bij groot scherm
				glVertex3f(-0.5 * aquariumSize.x() + balkSize2, -0.5 * aquariumSize.y(), 0.5 * aquariumSize.z() - 1);
				glVertex3f(-0.5 * aquariumSize.x(), -0.5 * aquariumSize.y(), 0.5 * aquariumSize.z() - 1);
				glVertex3f(-0.5 * aquariumSize.x(), 0.5 * aquariumSize.y(), 0.5 * aquariumSize.z() - 1);
				glVertex3f(-0.5 * aquariumSize.x() + balkSize2, 0.5 * aquariumSize.y(), 0.5 * aquariumSize.z() - 1);
				break;
		}

		//onder tegenover groot scherm
		glVertex3f(-0.5 * aquariumSize.x(), -0.5 * aquariumSize.y() + balkSize2, -0.5 * aquariumSize.z() + 1);
		glVertex3f(-0.5 * aquariumSize.x(), -0.5 * aquariumSize.y(), -0.5 * aquariumSize.z() + 1);
		glVertex3f(0.5 * aquariumSize.x(), -0.5 * aquariumSize.y(), -0.5 * aquariumSize.z() + 1);
		glVertex3f(0.5 * aquariumSize.x(), -0.5 * aquariumSize.y() + balkSize2, -0.5 * aquariumSize.z() + 1);

		//boven tegenover groot scherm
		glVertex3f(-0.5 * aquariumSize.x(), 0.5 * aquariumSize.y() - balkSize2, -0.5 * aquariumSize.z() + 1);
		glVertex3f(-0.5 * aquariumSize.x(), 0.5 * aquariumSize.y(), -0.5 * aquariumSize.z() + 1);
		glVertex3f(0.5 * aquariumSize.x(), 0.5 * aquariumSize.y(), -0.5 * aquariumSize.z() + 1);
		glVertex3f(0.5 * aquariumSize.x(), 0.5 * aquariumSize.y() - balkSize2, -0.5 * aquariumSize.z() + 1);

		//rechts tegenover groot scherm
		glVertex3f(0.5 * aquariumSize.x() - balkSize2, -0.5 * aquariumSize.y(), -0.5 * aquariumSize.z() + 1);
		glVertex3f(0.5 * aquariumSize.x(), -0.5 * aquariumSize.y(), -0.5 * aquariumSize.z() + 1);
		glVertex3f(0.5 * aquariumSize.x(), 0.5 * aquariumSize.y(), -0.5 * aquariumSize.z() + 1);
		glVertex3f(0.5 * aquariumSize.x() - balkSize2, 0.5 * aquariumSize.y(), -0.5 * aquariumSize.z() + 1);

		//links tegenover groot scherm
		glVertex3f(-0.5 * aquariumSize.x() + balkSize2, -0.5 * aquariumSize.y(), -0.5 * aquariumSize.z() + 1);
		glVertex3f(-0.5 * aquariumSize.x(), -0.5 * aquariumSize.y(), -0.5 * aquariumSize.z() + 1);
		glVertex3f(-0.5 * aquariumSize.x(), 0.5 * aquariumSize.y(), -0.5 * aquariumSize.z() + 1);
		glVertex3f(-0.5 * aquariumSize.x() + balkSize2, 0.5 * aquariumSize.y(), -0.5 * aquariumSize.z() + 1);

		//onder tegenover klein scherm
		glVertex3f(-0.5 * aquariumSize.x() + 1, -0.5 * aquariumSize.y() + balkSize2, -0.5 * aquariumSize.z());
		glVertex3f(-0.5 * aquariumSize.x() + 1, -0.5 * aquariumSize.y(), -0.5 * aquariumSize.z());
		glVertex3f(-0.5 * aquariumSize.x() + 1, -0.5 * aquariumSize.y(), 0.5 * aquariumSize.z());
		glVertex3f(-0.5 * aquariumSize.x() + 1, -0.5 * aquariumSize.y() + balkSize2, 0.5 * aquariumSize.z());

		//boven tegenover klein scherm
		glVertex3f(-0.5 * aquariumSize.x() + 1, 0.5 * aquariumSize.y() - balkSize2, -0.5 * aquariumSize.z());
		glVertex3f(-0.5 * aquariumSize.x() + 1, 0.5 * aquariumSize.y(), -0.5 * aquariumSize.z());
		glVertex3f(-0.5 * aquariumSize.x() + 1, 0.5 * aquariumSize.y(), 0.5 * aquariumSize.z());
		glVertex3f(-0.5 * aquariumSize.x() + 1, 0.5 * aquariumSize.y() - balkSize2, 0.5 * aquariumSize.z());

		//rechts tegenover klein scherm
		glVertex3f(-0.5 * aquariumSize.x() + 1, -0.5 * aquariumSize.y(), -0.5 * aquariumSize.z() + balkSize2);
		glVertex3f(-0.5 * aquariumSize.x() + 1, -0.5 * aquariumSize.y(), -0.5 * aquariumSize.z());
		glVertex3f(-0.5 * aquariumSize.x() + 1, 0.5 * aquariumSize.y(), -0.5 * aquariumSize.z());
		glVertex3f(-0.5 * aquariumSize.x() + 1, 0.5 * aquariumSize.y(), -0.5 * aquariumSize.z() + balkSize2);

		//links tegenover klein scherm
		glVertex3f(-0.5 * aquariumSize.x() + 1, -0.5 * aquariumSize.y(), 0.5 * aquariumSize.z() - balkSize2);
		glVertex3f(-0.5 * aquariumSize.x() + 1, -0.5 * aquariumSize.y(), 0.5 * aquariumSize.z() + 20);
		glVertex3f(-0.5 * aquariumSize.x() + 1, 0.5 * aquariumSize.y(), 0.5 * aquariumSize.z() + 20);
		glVertex3f(-0.5 * aquariumSize.x() + 1, 0.5 * aquariumSize.y(), 0.5 * aquariumSize.z() - balkSize2);
	}

	glEnd();

	glColor3f(1,1,1);
}

void render(AquariumController& aquariumController, CAMERA camera, int x, int y, ssize_t port_width, ssize_t port_height, const Eigen::Vector3d& area_size, const Eigen::Vector2d& facePosition = Eigen::Vector2d(0.5, 0.5))
{
	glViewport(x, y, port_width, port_height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	const double kx = 0.25 * area_size.x();
	const double ky = static_cast<double>(port_height) / static_cast<double>(port_width) * kx;

	const Eigen::Vector2d size(Eigen::Vector2d(area_size.x(), area_size.y()).cwise() * faceRange);
	const Eigen::Vector2d pos(size.cwise() * facePosition - size * .5);

	glFrustum(-kx - 0.5 * pos.x(), kx - 0.5 * pos.x(), -ky - 0.5 * pos.y(), ky - 0.5 * pos.y(), .5 * eye_distance, eye_distance * 2. + area_size.z());

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(-pos.x(), -pos.y(), -(eye_distance + area_size.z() * 0.5));

	switch (camera)
	{
		case LEFT_CAMERA:
			break;

		case RIGHT_CAMERA:
			glRotatef(270, 0, 1, 0);
			break;
	}

	DrawBackground(camera);
	aquariumController.Draw();
}

static void update_and_render_simulation(AquariumController& aquariumController, const double dt)
{
	aquariumController.Update(dt);

	image_receiver->Update();
	image_receiver2->Update();

	position_receiver->Update(aquariumController);
	faceposition_receiver->Update(aquariumController);

	glfwGetWindowSize(&win_width,&win_height);/// get window size
	const unsigned int port1_width = win_width * 2. / 3.;
	const unsigned int port2_width = win_width * 1. / 3.;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// left view
	render(aquariumController, LEFT_CAMERA, 0, 0, port1_width, win_height, aquariumSize, aquariumController.facePosition);

	// right view
	render(aquariumController, RIGHT_CAMERA, port1_width, 0, port2_width, win_height, Eigen::Vector3d(aquariumSize.z(), aquariumSize.y(), aquariumSize.x()));

	//TestDrawAquarium();
}

/**
 * Searches a list of possible data directories and selects the first one
 * that's useable.
 */
static string find_data_dir()
{
	static const std::string test_file = "/wiggle.glsl";

	for (const char** dir = &datadirs[0]; dir != &datadirs[ARRAY_SIZE(datadirs)]; ++dir)
	{
		std::ifstream test_if((*dir + test_file).c_str());
		if (test_if.is_open())
			return *dir;
	}

	return "";
}

int main(int argc, char** argv)
{
#if defined(__GNUC__)
	// Report uncaught exceptions in a nicer way than terminating alone.
	std::set_terminate (__gnu_cxx::__verbose_terminate_handler);
#endif

#ifdef CONFIGURE_LINE
	fprintf(stderr, "%s\n", "fishtank was configured with " CONFIGURE_LINE);
#endif

	datadir = find_data_dir();

	try
	{
		srand(time(NULL));/// make random numbers sequence depend to program start time.

		// Search for config files and use the first one found
		ifstream input_file;
		for (const char** dir = &sysconfdirs[0]; dir != &sysconfdirs[ARRAY_SIZE(sysconfdirs)]; ++dir)
		{
			static const string config_file = "/aquaConfig.txt";

			input_file.open((*dir + config_file).c_str());
			if (input_file.is_open())
			{
				// @TODO Should eventually replace this entirely by ParseOptions
				LoadSettings(input_file);
				break;
			}
		}

		ifstream config;
		// First parse command line options (to allow it to override everything else)
		ParseOptions(argc, argv, config);

		// Search for config files and use the first one found
		for (const char** dir = &sysconfdirs[0]; dir != &sysconfdirs[ARRAY_SIZE(sysconfdirs)]; ++dir)
		{
			static const string config_file = "/aquaConfig.cfg";

			config.open((*dir + config_file).c_str());
			if (config.is_open())
			{
				ParseOptions(0, NULL, config);
				break;
			}
		}
		config.close();

		glfwInit();
		glfwOpenWindowHint(GLFW_FSAA_SAMPLES, multi_sample);

		if( !glfwOpenWindow( win_width, win_height,  0,0,0,0,  16, 	 0, GLFW_WINDOW ))
		{/// width, height, rgba bits (4 params), depth bits, stencil bits, mode.
			cout << "Bye world! Open window failed" << endl;
			return 1;
		}
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
		glfwSetWindowTitle("");

		AquariumController aquariumController;

		LoadModels(input_file, aquariumController);
		input_file.close();

		// We're using three screens
		win_width *= 3;
		glfwSetWindowSize(win_width, win_height);
		glfwSetWindowPos(win_move_x, win_move_y);

		/* Required to make sure the back buffer has the same size as
		 * the window (to make sure the first frame is drawn properly
		 * as well).
		 */
		glfwSwapBuffers();

		boost::shared_ptr<Texture> _webcam_texture;
		{
			static const char pixel = 0;
			using namespace boost::gil;

			_webcam_texture.reset(new Texture(interleaved_view(1, 1, (const gray8_pixel_t*)&pixel, 1)));
		}
		webcam_texture = _webcam_texture;

		//gebruik mist voor het "water effect"
		GLfloat fogColor[4]= {0.3f, 0.4f, 0.7f, 1.0f};
		//glClearColor(fogColor[0], fogColor[1], fogColor[2], fogColor[3]); // make it clear to fog color?

		glFogi(GL_FOG_MODE, GL_EXP);
		//glFogi(GL_FOG_COORD_SRC, GL_FRAGMENT_DEPTH);
		//mist kleur
		glFogfv(GL_FOG_COLOR, fogColor);
		//mist dichtheid
		glFogf(GL_FOG_DENSITY, 1.0f/(eye_distance+aquariumSize.z()+aquariumSize.x()));
		/// niet nodig
		//glHint(GL_FOG_HINT, GL_NICEST);
		//glFogf(GL_FOG_START, eye_distance);
		//glFogf(GL_FOG_END, eye_distance+aquariumSize.z()+aquariumSize.x());
		glEnable(GL_FOG);

		fps.reset();

		image_receiver.reset(new ImageReceiver(7778));
		image_receiver2.reset(new ImageReceiver(7779));
		position_receiver.reset(new PositionReceiver(0, 7780));
		faceposition_receiver.reset(new PositionReceiver(1, 7781));

		for (double curTime = glfwGetTime(), oldTime = curTime; glfwGetWindowParam(GLFW_OPENED); oldTime = curTime, curTime = glfwGetTime())
		{
			const double dt = min(0.1, curTime - oldTime);
			update_and_render_simulation(aquariumController, dt);

			glfwSwapBuffers();
			fps.frameRateDelay();
#ifdef DEBUG
			if (fps.frameCount() % fps.targetRate() == 0)
				std::cerr << "Rendered frames: " << fps.frameCount() <<  "; in seconds: " << fps.countTime() << "; with average framerate: " << fps.avgFrameRate() << "; current measured framerate (inaccurate): " << fps.recentAvgFrameRate() << "\n";
#endif
		}

		return 0;
	}
	catch (const exit_exception& e)
	{
		return e.code();
	}
	catch (const OpenGL::shader_source_error& e)
	{
		std::cerr << "Unhandled OpenGL::shader_source_error exception caught: " << e.what() << "\n"
		          << "Info log contains:\n"
		          << e.infoLog() << "\n";
		throw;
	}
	catch (const OpenGL::shader_uniform_location_error& e)
	{
		std::cerr << "Unhandled OpenGL::shader_uniform_location_error exception caught: " << e.what() << "\n"
		          << "Uniform name is: \"" << e.uniform_name() << "\"\n";
		throw;
	}
}
