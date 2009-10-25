#include <boost/asio/io_service.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/format.hpp>
#include <boost/gil/image.hpp>
#include <boost/gil/typedefs.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>
#include <boost/regex.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include "camera.hpp"
#include <framework/debug.hpp>
#include <framework/debug-net-out.hpp>
#include <framework/threadpool.hpp>
#include <framework/vfs.hpp>
#include "GL/GLee.h"
#include <GL/glfw.h>
#include <GL/gl.h>
#include <Eigen/Core>
#include "framerate.hpp"
#include <luabind/luabind.hpp>
#include "main.hpp"
#include <script/state.hpp>
#include "Vis.h"
#include "aquarium.hpp"
#include "MS3D_ASCII.h"
#include <string>

#include <cmath>
#include <cstdlib>
#include <ctime>

#include <algorithm>
#include <iostream>
#include <vector>
#include <map>

#include "glexcept.hpp"

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

// Allow for easy adding of translations
#define _(string) (string)

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

using boost::format;
using namespace std;
namespace fs = boost::filesystem;
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

fs::path datadir;

bool use_vbos = true;
bool drawCollisionSpheres = false;
static bool enable_fog = true;

/* The amount of additional samples per pixel to generate. Higher amounts will
 * generally produce better anti aliasing results.
 */
static unsigned int multi_sample = 4;

static Eigen::Vector3d aquariumInitSize(440., 250., 220.);

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

/**
 * Amount of worker threads to handle various I/O jobs
 */
static unsigned int io_threads = 1;

//scherm resolutie
static Eigen::Vector2i win_size(0, 0);
//de schermpositie
static Eigen::Vector2i win_pos(5, 30);

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

static void ParseOptions(int argc, char** argv, std::istream& config_file, boost::asio::io_service& io_svc)
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
	    ("camera", po::value<int>(&cameraIndex)->default_value(cameraIndex),
	      _("The camera number to use (-1 uses the first available camera)."))
	    ("camera-resolution", po::value<Eigen::Vector2i>(&cameraResolution)->default_value(cameraResolution),
	      _("The resolution to use for the camera."))
	    ("draw-collision-spheres", po::value<bool>(&drawCollisionSpheres)->default_value(drawCollisionSpheres),
	      _("Draw the spheres used to perform collision detection with"))
	    ("fog", po::value<bool>(&enable_fog)->default_value(enable_fog),
	      _("Enable or disable the fog."))
	    ("fps", po::value<unsigned int>()->default_value(fps.targetRate()),
	      _("Set the target framerate, the program will not exceed a rendering rate of this amount in Hz."))
	    ("io-threads", po::value<unsigned int>(&io_threads)->default_value(io_threads),
	      _("The amount of additional threads to use for handling I/O in. Additional to the single thread that the program already has."))
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
	cmdline_options.add(generic);
	DebugStream::addCommandLineOptions(cmdline_options);
	DebugNetOutputStream::addCommandLineOptions(cmdline_options);
	cmdline_options.add(config);
	cmdline_options.add(hidden);

	po::options_description config_file_options;
	DebugStream::addCommandLineOptions(config_file_options);
	DebugNetOutputStream::addCommandLineOptions(config_file_options);
	config_file_options.add(config);
	config_file_options.add(hidden);

	po::options_description visible;
	visible.add(generic);
	DebugStream::addCommandLineOptions(visible);
	DebugNetOutputStream::addCommandLineOptions(visible);
	visible.add(config);

	po::variables_map vm;
	if (argv != NULL)
		po::store(po::parse_command_line(argc, argv, cmdline_options), vm);
	po::store(po::parse_config_file(config_file, cmdline_options), vm);
	po::notify(vm);

	if (vm.count("help"))
	{
		cout << visible;
		throw exit_exception(EXIT_FAILURE);
	}

	if (vm.count("version"))
	{
		cout << "Fishtank - Version <UNSPECIFIED>\n"
		        "Created by Jasper Lammers and Dmytry Lavrov\n";
		throw exit_exception(EXIT_FAILURE);
	}

	if (vm.count("fps"))
	{
		fps.targetRate(vm["fps"].as<unsigned int>());
	}

	if (vm.count("window-size"))
	{
		win_size = vm["window-size"].as<Eigen::Vector2i>();
	}

	if (vm.count("window-position"))
	{
		win_pos = vm["window-position"].as<Eigen::Vector2i>();
	}

	DebugStream::processOptions(vm);
	DebugNetOutputStream::processOptions(io_svc, vm);
}

static boost::shared_ptr<Camera> webcam;
static boost::weak_ptr<Texture> webcam_texture;

enum CAMERA
{
	LEFT_CAMERA,
	RIGHT_CAMERA,
};

/**
 * Draw the webcam screens en beams, depending on the current camera position.
 */
static void DrawBackground(CAMERA camera, Aquarium& aquarium)
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
			break;

		case RIGHT_CAMERA:
			glDisable(GL_TEXTURE_2D);
			break;
	}

	//zet kleur naar wit
	glColor3f(0,0,0);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glBegin(GL_QUADS);

	switch (camera)
	{
		case LEFT_CAMERA:
			glTexCoord2f(0, 0);
			glVertex3f(0.5 * aquarium.size().x(), -0.5 * aquarium.size().y() + balkSize, -0.5 * aquarium.size().z() + balkSize);

			glTexCoord2f(0, 1);
			glVertex3f(0.5 * aquarium.size().x(),  0.5 * aquarium.size().y() - balkSize, -0.5 * aquarium.size().z() + balkSize);

			glTexCoord2f(1, 1);
			glVertex3f(0.5 * aquarium.size().x(),  0.5 * aquarium.size().y() - balkSize,  0.5 * aquarium.size().z() - balkSize);

			glTexCoord2f(1, 0);
			glVertex3f(0.5 * aquarium.size().x(), -0.5 * aquarium.size().y() + balkSize,  0.5 * aquarium.size().z() - balkSize);
			break;

		case RIGHT_CAMERA:
			glTexCoord2f(1, 0);
			glVertex3f(-0.5 * aquarium.size().x() + balkSize, -0.5 * aquarium.size().y() + balkSize, 0.5 * aquarium.size().z());

			glTexCoord2f(1, 1);
			glVertex3f(-0.5 * aquarium.size().x() + balkSize,  0.5 * aquarium.size().y() - balkSize, 0.5 * aquarium.size().z());

			glTexCoord2f(0, 1);
			glVertex3f( 0.5 * aquarium.size().x() - balkSize,  0.5 * aquarium.size().y() - balkSize, 0.5 * aquarium.size().z());

			glTexCoord2f(0, 0);
			glVertex3f( 0.5 * aquarium.size().x() - balkSize, -0.5 * aquarium.size().y() + balkSize, 0.5 * aquarium.size().z());
			break;
	}

	glEnd();
	glDisable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glBegin(GL_QUADS);

	if (balkSize2 != 0)
	{
		switch (camera)
		{
			case LEFT_CAMERA:
				//onder bij klein scherm
				glVertex3f(0.5 * aquarium.size().x() - 1, -0.5 * aquarium.size().y() + balkSize2, -0.5 * aquarium.size().z());
				glVertex3f(0.5 * aquarium.size().x() - 1, -0.5 * aquarium.size().y()            , -0.5 * aquarium.size().z());
				glVertex3f(0.5 * aquarium.size().x() - 1, -0.5 * aquarium.size().y()            ,  0.5 * aquarium.size().z());
				glVertex3f(0.5 * aquarium.size().x() - 1, -0.5 * aquarium.size().y() + balkSize2,  0.5 * aquarium.size().z());

				//boven bij klein scherm
				glVertex3f(0.5 * aquarium.size().x() - 1, 0.5 * aquarium.size().y() - balkSize2, -0.5 * aquarium.size().z());
				glVertex3f(0.5 * aquarium.size().x() - 1, 0.5 * aquarium.size().y()            , -0.5 * aquarium.size().z());
				glVertex3f(0.5 * aquarium.size().x() - 1, 0.5 * aquarium.size().y()            ,  0.5 * aquarium.size().z());
				glVertex3f(0.5 * aquarium.size().x() - 1, 0.5 * aquarium.size().y() - balkSize2,  0.5 * aquarium.size().z());

				//rechts bij klein scherm
				glVertex3f(0.5 * aquarium.size().x() - 1, -0.5 * aquarium.size().y(), -0.5 * aquarium.size().z() + balkSize2);
				glVertex3f(0.5 * aquarium.size().x() - 1, -0.5 * aquarium.size().y(), -0.5 * aquarium.size().z());
				glVertex3f(0.5 * aquarium.size().x() - 1,  0.5 * aquarium.size().y(), -0.5 * aquarium.size().z());
				glVertex3f(0.5 * aquarium.size().x() - 1,  0.5 * aquarium.size().y(), -0.5 * aquarium.size().z() + balkSize2);

				//links bij klein scherm
				glVertex3f(0.5 * aquarium.size().x() - 1, -0.5 * aquarium.size().y(), 0.5 * aquarium.size().z() - balkSize2);
				glVertex3f(0.5 * aquarium.size().x() - 1, -0.5 * aquarium.size().y(), 0.5 * aquarium.size().z());
				glVertex3f(0.5 * aquarium.size().x() - 1,  0.5 * aquarium.size().y(), 0.5 * aquarium.size().z());
				glVertex3f(0.5 * aquarium.size().x() - 1,  0.5 * aquarium.size().y(), 0.5 * aquarium.size().z() - balkSize2);
				break;

			case RIGHT_CAMERA:
				//dwarsbalk bij groot scherm
				glVertex3f(-balkSize2, -0.5 * aquarium.size().y() + balkSize2, 0.5 * aquarium.size().z() - 1);
				glVertex3f(-balkSize2,  0.5 * aquarium.size().y() - balkSize2, 0.5 * aquarium.size().z() - 1);
				glVertex3f( balkSize2,  0.5 * aquarium.size().y() - balkSize2, 0.5 * aquarium.size().z() - 1);
				glVertex3f( balkSize2, -0.5 * aquarium.size().y() + balkSize2, 0.5 * aquarium.size().z() - 1);

				//onder bij groot scherm
				glVertex3f(-0.5 * aquarium.size().x(), -0.5 * aquarium.size().y() + balkSize2, 0.5 * aquarium.size().z() - 1);
				glVertex3f(-0.5 * aquarium.size().x(), -0.5 * aquarium.size().y()            , 0.5 * aquarium.size().z() - 1);
				glVertex3f( 0.5 * aquarium.size().x(), -0.5 * aquarium.size().y()            , 0.5 * aquarium.size().z() - 1);
				glVertex3f( 0.5 * aquarium.size().x(), -0.5 * aquarium.size().y() + balkSize2, 0.5 * aquarium.size().z() - 1);

				//boven bij groot scherm
				glVertex3f(-0.5 * aquarium.size().x(), 0.5 * aquarium.size().y() - balkSize2, 0.5 * aquarium.size().z() - 1);
				glVertex3f(-0.5 * aquarium.size().x(), 0.5 * aquarium.size().y()            , 0.5 * aquarium.size().z() - 1);
				glVertex3f( 0.5 * aquarium.size().x(), 0.5 * aquarium.size().y()            , 0.5 * aquarium.size().z() - 1);
				glVertex3f( 0.5 * aquarium.size().x(), 0.5 * aquarium.size().y() - balkSize2, 0.5 * aquarium.size().z() - 1);

				//rechts bij groot scherm
				glVertex3f(0.5 * aquarium.size().x() - balkSize2, -0.5 * aquarium.size().y(), 0.5 * aquarium.size().z() - 1);
				glVertex3f(0.5 * aquarium.size().x()            , -0.5 * aquarium.size().y(), 0.5 * aquarium.size().z() - 1);
				glVertex3f(0.5 * aquarium.size().x()            ,  0.5 * aquarium.size().y(), 0.5 * aquarium.size().z() - 1);
				glVertex3f(0.5 * aquarium.size().x() - balkSize2,  0.5 * aquarium.size().y(), 0.5 * aquarium.size().z() - 1);

				//links bij groot scherm
				glVertex3f(-0.5 * aquarium.size().x() + balkSize2, -0.5 * aquarium.size().y(), 0.5 * aquarium.size().z() - 1);
				glVertex3f(-0.5 * aquarium.size().x()            , -0.5 * aquarium.size().y(), 0.5 * aquarium.size().z() - 1);
				glVertex3f(-0.5 * aquarium.size().x()            ,  0.5 * aquarium.size().y(), 0.5 * aquarium.size().z() - 1);
				glVertex3f(-0.5 * aquarium.size().x() + balkSize2,  0.5 * aquarium.size().y(), 0.5 * aquarium.size().z() - 1);
				break;
		}

		//onder tegenover groot scherm
		glVertex3f(-0.5 * aquarium.size().x(), -0.5 * aquarium.size().y() + balkSize2, -0.5 * aquarium.size().z() + 1);
		glVertex3f(-0.5 * aquarium.size().x(), -0.5 * aquarium.size().y()            , -0.5 * aquarium.size().z() + 1);
		glVertex3f( 0.5 * aquarium.size().x(), -0.5 * aquarium.size().y()            , -0.5 * aquarium.size().z() + 1);
		glVertex3f( 0.5 * aquarium.size().x(), -0.5 * aquarium.size().y() + balkSize2, -0.5 * aquarium.size().z() + 1);

		//boven tegenover groot scherm
		glVertex3f(-0.5 * aquarium.size().x(), 0.5 * aquarium.size().y() - balkSize2, -0.5 * aquarium.size().z() + 1);
		glVertex3f(-0.5 * aquarium.size().x(), 0.5 * aquarium.size().y()            , -0.5 * aquarium.size().z() + 1);
		glVertex3f( 0.5 * aquarium.size().x(), 0.5 * aquarium.size().y()            , -0.5 * aquarium.size().z() + 1);
		glVertex3f( 0.5 * aquarium.size().x(), 0.5 * aquarium.size().y() - balkSize2, -0.5 * aquarium.size().z() + 1);

		//rechts tegenover groot scherm
		glVertex3f(0.5 * aquarium.size().x() - balkSize2, -0.5 * aquarium.size().y(), -0.5 * aquarium.size().z() + 1);
		glVertex3f(0.5 * aquarium.size().x()            , -0.5 * aquarium.size().y(), -0.5 * aquarium.size().z() + 1);
		glVertex3f(0.5 * aquarium.size().x()            ,  0.5 * aquarium.size().y(), -0.5 * aquarium.size().z() + 1);
		glVertex3f(0.5 * aquarium.size().x() - balkSize2,  0.5 * aquarium.size().y(), -0.5 * aquarium.size().z() + 1);

		//links tegenover groot scherm
		glVertex3f(-0.5 * aquarium.size().x() + balkSize2, -0.5 * aquarium.size().y(), -0.5 * aquarium.size().z() + 1);
		glVertex3f(-0.5 * aquarium.size().x()            , -0.5 * aquarium.size().y(), -0.5 * aquarium.size().z() + 1);
		glVertex3f(-0.5 * aquarium.size().x()            ,  0.5 * aquarium.size().y(), -0.5 * aquarium.size().z() + 1);
		glVertex3f(-0.5 * aquarium.size().x() + balkSize2,  0.5 * aquarium.size().y(), -0.5 * aquarium.size().z() + 1);

		//onder tegenover klein scherm
		glVertex3f(-0.5 * aquarium.size().x() + 1, -0.5 * aquarium.size().y() + balkSize2, -0.5 * aquarium.size().z());
		glVertex3f(-0.5 * aquarium.size().x() + 1, -0.5 * aquarium.size().y()            , -0.5 * aquarium.size().z());
		glVertex3f(-0.5 * aquarium.size().x() + 1, -0.5 * aquarium.size().y()            ,  0.5 * aquarium.size().z());
		glVertex3f(-0.5 * aquarium.size().x() + 1, -0.5 * aquarium.size().y() + balkSize2,  0.5 * aquarium.size().z());

		//boven tegenover klein scherm
		glVertex3f(-0.5 * aquarium.size().x() + 1, 0.5 * aquarium.size().y() - balkSize2, -0.5 * aquarium.size().z());
		glVertex3f(-0.5 * aquarium.size().x() + 1, 0.5 * aquarium.size().y()            , -0.5 * aquarium.size().z());
		glVertex3f(-0.5 * aquarium.size().x() + 1, 0.5 * aquarium.size().y()            ,  0.5 * aquarium.size().z());
		glVertex3f(-0.5 * aquarium.size().x() + 1, 0.5 * aquarium.size().y() - balkSize2,  0.5 * aquarium.size().z());

		//rechts tegenover klein scherm
		glVertex3f(-0.5 * aquarium.size().x() + 1, -0.5 * aquarium.size().y(), -0.5 * aquarium.size().z() + balkSize2);
		glVertex3f(-0.5 * aquarium.size().x() + 1, -0.5 * aquarium.size().y(), -0.5 * aquarium.size().z());
		glVertex3f(-0.5 * aquarium.size().x() + 1,  0.5 * aquarium.size().y(), -0.5 * aquarium.size().z());
		glVertex3f(-0.5 * aquarium.size().x() + 1,  0.5 * aquarium.size().y(), -0.5 * aquarium.size().z() + balkSize2);

		//links tegenover klein scherm
		glVertex3f(-0.5 * aquarium.size().x() + 1, -0.5 * aquarium.size().y(), 0.5 * aquarium.size().z() - balkSize2);
		glVertex3f(-0.5 * aquarium.size().x() + 1, -0.5 * aquarium.size().y(), 0.5 * aquarium.size().z() + 20);
		glVertex3f(-0.5 * aquarium.size().x() + 1,  0.5 * aquarium.size().y(), 0.5 * aquarium.size().z() + 20);
		glVertex3f(-0.5 * aquarium.size().x() + 1,  0.5 * aquarium.size().y(), 0.5 * aquarium.size().z() - balkSize2);
	}

	glEnd();

	glColor3f(1,1,1);
}

void render(Aquarium& aquarium, LuaScript& L, CAMERA camera, int x, int y, int port_width, int port_height, const Eigen::Vector3d& area_size, const Eigen::Vector2d& facePosition = Eigen::Vector2d(0.5, 0.5))
{
	glViewport(x, y, port_width, port_height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	const double kx = 0.25 * area_size.x();
	const double ky = static_cast<double>(port_height) / static_cast<double>(port_width) * kx;

	const Eigen::Vector2d size(Eigen::Vector2d(area_size.x(), area_size.y()).cwise() * faceRange);
	const Eigen::Vector2d pos(size.cwise() * facePosition - size * .5);

	glFrustum(-kx - 0.5 * pos.x(), kx - 0.5 * pos.x(), -ky - 0.5 * pos.y(), ky - 0.5 * pos.y(), .5 * aquarium.eye_distance, aquarium.eye_distance * 2. + area_size.z());

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(-pos.x(), -pos.y(), -(aquarium.eye_distance + area_size.z() * 0.5));

	switch (camera)
	{
		case LEFT_CAMERA:
			break;

		case RIGHT_CAMERA:
			glRotatef(270, 0, 1, 0);
			break;
	}

	DrawBackground(camera, aquarium);
	aquarium.draw();

	if (luabind::type(L["draw"]) == LUA_TFUNCTION)
		luabind::call_function<void>(L, "draw");
}

static void update_and_render_simulation(Aquarium& aquarium, LuaScript& L, const double dt)
{
	aquarium.update(dt);

	if (luabind::type(L["update"]) == LUA_TFUNCTION)
		luabind::call_function<void>(L, "update", dt);

	glfwGetWindowSize(&win_size.x(), &win_size.y());/// get window size
	const unsigned int port1_width = win_size.x() * 2. / 3.;
	const unsigned int port2_width = win_size.x() * 1. / 3.;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// left view
	render(aquarium, L, LEFT_CAMERA, 0, 0, port1_width, win_size.y(), aquarium.size(), aquarium.facePosition);

	// right view
	render(aquarium, L, RIGHT_CAMERA, port1_width, 0, port2_width, win_size.y(), Eigen::Vector3d(aquarium.size().z(), aquarium.size().y(), aquarium.size().x()));
}

/**
 * Searches a list of possible data directories and selects the first one
 * that's useable.
 */
static fs::path find_data_dir()
{
	static const std::string test_file = "/wiggle.glsl";

	for (const char** dir = &datadirs[0]; dir != &datadirs[ARRAY_SIZE(datadirs)]; ++dir)
	{
		fs::path test_file(fs::path(*dir) / "wiggle.glsl");

		if (fs::exists(test_file))
			return test_file.parent_path();
	}

	return "";
}

int main(int argc, char** argv)
{
#if defined(__GNUC__)
	// Report uncaught exceptions in a nicer way than terminating alone.
	std::set_terminate (__gnu_cxx::__verbose_terminate_handler);
#endif

	DebugStream::registerDebugOutput(new stderr_wrapper);

	for (const char** dir = &datadirs[0]; dir != &datadirs[ARRAY_SIZE(datadirs)]; ++dir)
		vfs::allowed_read_paths.push_back(*dir);
	for (const char** dir = &sysconfdirs[0]; dir != &sysconfdirs[ARRAY_SIZE(sysconfdirs)]; ++dir)
		vfs::allowed_read_paths.push_back(*dir);

	datadir = find_data_dir();

	LuaScript lua_state;

	try
	{
		srand(time(NULL));/// make random numbers sequence depend to program start time.

		fs::ifstream config;
		boost::asio::io_service io_svc;
		// First parse command line options (to allow it to override everything else)
		ParseOptions(argc, argv, config, io_svc);

		// Search for config files and use the first one found
		for (const char** dir = &sysconfdirs[0]; dir != &sysconfdirs[ARRAY_SIZE(sysconfdirs)]; ++dir)
		{
			fs::path config_file(fs::path(*dir) / "aquaConfig.cfg");

			config.open(config_file);
			if (config.is_open())
			{
				ParseOptions(0, NULL, config, io_svc);
				break;
			}
		}
		config.close();

		/* Prevents io_service's run() function from returning if no
		 * jobs are available. Required to make sure the I/O threads
		 * stay alive when there's little to do.
		 */
		boost::asio::io_service::work work(io_svc);

		// Fire off several worker threads.
		boost::thread_group threads;
		create_thread_pool(io_svc, threads, io_threads);

#ifdef CONFIGURE_LINE
		debug(LOG_NEVER) << "fishtank was configured with " CONFIGURE_LINE;
#endif

		glfwInit();
		glfwOpenWindowHint(GLFW_FSAA_SAMPLES, multi_sample);

		if (!glfwOpenWindow(win_size.x(), win_size.y(), 0, 0, 0, 0, 16, 0, GLFW_WINDOW))
		{/// width, height, rgba bits (4 params), depth bits, stencil bits, mode.
			debug(LOG_ERROR) << "Bye cruel world! glfwOpenWindow() failed";
			throw exit_exception(EXIT_FAILURE);
		}
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
		glfwSetWindowTitle("");

		Aquarium aquarium(aquariumInitSize);

		// Give management of this piece of memory to Lua
		lua_state["aquarium"] = &aquarium;
		lua_state["cameraTransformations"] = &cameraTransformations;
		lua_state["faceRange"] = &faceRange;
		lua_state["swimArea"] = &swimArea;
		lua_state["win_pos"] = &win_pos;
		lua_state["win_size"] = &win_size;

		for (const char** dir = &sysconfdirs[0]; dir != &sysconfdirs[ARRAY_SIZE(sysconfdirs)]; ++dir)
		{
			fs::path lua_setup_file(normalized_path(fs::path(*dir) / "fishtank-setup.lua"));

			// Strip references to the current working directory (to make Lua related debug messages cleaner)
			while (lua_setup_file.string().substr(0, 2) == "./")
				lua_setup_file = lua_setup_file.string().substr(2);

			if (!fs::exists(lua_setup_file))
				continue;

			lua_state.dofile(lua_setup_file);
		}

		// We're using three screens
		win_size.x() *= 3;
		glfwSetWindowSize(win_size.x(), win_size.y());
		glfwSetWindowPos(win_pos.x(), win_pos.y());

		/* Required to make sure the back buffer has the same size as
		 * the window (to make sure the first frame is drawn properly
		 * as well).
		 */
		glfwSwapBuffers();

		boost::shared_ptr<Texture> _webcam_texture;
		{
			using namespace boost::gil;

			_webcam_texture.reset(new Texture);
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
		glFogf(GL_FOG_DENSITY, 1.f / (aquarium.eye_distance + aquarium.size().z() + aquarium.size().x()));
		/// niet nodig
		//glHint(GL_FOG_HINT, GL_NICEST);
		//glFogf(GL_FOG_START, aquarium.eye_distance);
		//glFogf(GL_FOG_END, aquarium.eye_distance + aquarium.size().z() + aquarium.size().x());
		if (enable_fog)
			glEnable(GL_FOG);
		else
			glDisable(GL_FOG);

		fps.reset();

		for (double curTime = glfwGetTime(), oldTime = curTime; glfwGetWindowParam(GLFW_OPENED); oldTime = curTime, curTime = glfwGetTime())
		{
			const double dt = min(0.1, curTime - oldTime);
			update_and_render_simulation(aquarium, lua_state, dt);

			glfwSwapBuffers();
			fps.frameRateDelay();
#ifdef DEBUG
			if (fps.frameCount() % fps.targetRate() == 0)
				debug(LOG_MAIN) << fps.frameCount() <<  " frames in " << fps.countTime() << " seconds, average framerate: " << fps.avgFrameRate() << "; current framerate (inaccurate): " << fps.recentAvgFrameRate();
#endif
		}
	}
	catch (const exit_exception& e)
	{
		return e.code();
	}
	catch (const OpenGL::shader_source_error& e)
	{
		debug(LOG_ERROR) << "Unhandled OpenGL::shader_source_error exception caught: " << e.what() << "\n"
		                 << "Info log contains:\n"
		                 << e.infoLog();
		throw;
	}
	catch (const OpenGL::shader_uniform_location_error& e)
	{
		debug(LOG_ERROR) << "Unhandled OpenGL::shader_uniform_location_error exception caught: " << e.what() << "\n"
		                 << "Uniform name is: \"" << e.uniform_name();
		throw;
	}
	catch (const luabind::error& e)
	{
		using namespace luabind;
		object error_msg(from_stack(e.state(), -1));
		debug(LOG_ERROR) << "Lua error: " << error_msg;
	}
}
