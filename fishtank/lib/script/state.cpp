#include "state.hpp"
#include <boost/array.hpp>
#include "debug-support.hpp"
#include "eigen-support.hpp"
#include <framework/resource.hpp>
#include <luabind/luabind.hpp>
#include <luabind/operator.hpp>
extern "C" {
#include <lua.h>
#include <lualib.h>
}
#include "object-support.hpp"
#include "opengl-support.hpp"
#include <vector>
#include "vertexarrays-support.hpp"

// FIXME: #includes from top level dir
#include "../../aquarium.hpp"
#include "../../camera.hpp"
#include "../../Ground.h"
#include "../../main.hpp"
#include "../../wiggle.hpp"

using namespace luabind;

static int os_exit (lua_State* L)
{
	throw exit_exception(luaL_optint(L, 1, EXIT_SUCCESS));
}

LuaScript::LuaScript() :
	globals(luabind::globals(L))
{
	/* Intialises LuaBind support code for this lua_State (automatically
	 * garbage collected when lua_State closes).
	 */
	open(L);

	register_safe_default_lua_libs();
	register_interfaces();
}

LuaScript::operator lua_State*()
{
	return L;
}

void LuaScript::register_safe_default_lua_libs()
{
	lua_pushcfunction(L, &luaopen_base);    lua_call(L, 0, 0);
	//lua_pushcfunction(L, &luaopen_package); lua_call(L, 0, 0);
	lua_pushcfunction(L, &luaopen_string);  lua_call(L, 0, 0);
	lua_pushcfunction(L, &luaopen_table);   lua_call(L, 0, 0);
	lua_pushcfunction(L, &luaopen_math);    lua_call(L, 0, 0);
	lua_pushcfunction(L, &luaopen_io);      lua_call(L, 0, 0);
	lua_pushcfunction(L, &luaopen_os);      lua_call(L, 0, 0);

	/*
	 * Functions that need to be ditched or overridden for safety:
	 *  * dofile
	 *  * loadfile
	 *  * print
	 *  * io.input
	 *  * io.lines
	 *  * io.open
	 *  * io.output
	 *  * io.popen
	 *  * os.execute
	 *  * os.exit
	 *  * os.getenv
	 *  * os.remove
	 *  * os.rename
	 *  * os.setlocale
	 *
	 *  Ditching these functions (to allow later redefinition).
	 */
	globals["dofile"] = nil;
	globals["loadfile"] = nil;
	globals["print"] = nil;
	globals["io"]["input"] = nil;
	globals["io"]["lines"] = nil;
	globals["io"]["open"] = nil;
	globals["io"]["output"] = nil;
	globals["io"]["popen"] = nil;
	globals["os"]["execute"] = nil;
	globals["os"]["exit"] = nil;
	globals["os"]["getenv"] = nil;
	globals["os"]["remove"] = nil;
	globals["os"]["rename"] = nil;
	globals["os"]["setlocale"] = nil;
}

void LuaScript::register_interfaces()
{
	lua_getglobal(L, "os");
	lua_pushcfunction(L, &os_exit);
	lua_setfield(L, -2, "exit");
	lua_pop(L, 1); // pop the 'os' table

	debug_register_with_lua(L);

	OpenGL::register_with_lua(L);
	Eigen::register_with_lua(L);
	vertexarrays_register_with_lua(L);

	// Misc stuff
	module(L)
	[
		class_<Transformation>("Transformation")
			.def(constructor<Transformation::VIEW_TRANSFORMATIONS>())
			.def(constructor<Transformation::VIEW_TRANSFORMATIONS, std::ptrdiff_t, std::ptrdiff_t>())
			.def(constructor<Transformation::VIEW_TRANSFORMATIONS, std::ptrdiff_t, std::ptrdiff_t, std::ptrdiff_t, std::ptrdiff_t>())
			.enum_("VIEW_TRANSFORMATIONS")
			[
				value("FLIP_UP_DOWN",    Transformation::FLIP_UP_DOWN),
				value("FLIP_LEFT_RIGHT", Transformation::FLIP_LEFT_RIGHT),
				value("TRANSPOSE",       Transformation::TRANSPOSE),
				value("ROTATE_90CW",     Transformation::ROTATE_90CW),
				value("ROTATE_90CCW",    Transformation::ROTATE_90CCW),
				value("ROTATE_180",      Transformation::ROTATE_180),
				value("SUBIMAGE",        Transformation::SUBIMAGE),
				value("SUBSAMPLE",       Transformation::SUBSAMPLE)
			],

		class_< std::vector<Transformation> >("Transformation_vector")
			.def("push_back", &std::vector<Transformation>::push_back)
	];

	module(L)
	[
		def("loadModel", (boost::shared_ptr<const Model> (*)(const std::string&, const std::string&))&loadModel),
		def("loadModel", (boost::shared_ptr<const Model> (*)(const std::string&, const std::string&, const Eigen::Matrix4f&))&loadModel),
		def("loadTexture", &loadTexture),

		class_<Model, boost::shared_ptr<Model> >("Model")
			.def("render", &Model::render)
			.def_readonly("bb_l", &Model::bb_l)
			.def_readonly("bb_h", &Model::bb_h)
			.def_readonly("dir",  &Model::dir),

		Object_lua_wrapper::register_with_lua(),

		class_<Ground>("Ground")
			.def("depth", &Ground::depth)
			.def("HeightAt", &Ground::HeightAt)
			.property("maxHeight", (int (Ground::*)() const) &Ground::maxHeight, (void (Ground::*)(int)) &Ground::maxHeight)
			.def("width", &Ground::width),

		class_<Aquarium>("Aquarium")
			.def("AddFish", &Aquarium::AddFish)
			.def("AddObject", &Aquarium::AddObject)
			.def("AddBubbleSpot", &Aquarium::AddBubbleSpot)
			.def_readwrite("ground", &Aquarium::ground)
			.property("size", (const Eigen::Vector3d& (Aquarium::*)() const) &Aquarium::size, (void (Aquarium::*)(const Eigen::Vector3d&)) &Aquarium::size),

		class_<Texture, boost::shared_ptr<Texture> >("Texture")
			.def("bind",  &Texture::bind)
			.def("clear", &Texture::clear)
			.def("empty", &Texture::empty),

		class_<WiggleTransformation, boost::shared_ptr<WiggleTransformation> >("WiggleTransformation")
			.def(constructor<>())
			.def("update", &WiggleTransformation::update)
			.def("use", &WiggleTransformation::use)
			.scope
			[
				def("noUse", &WiggleTransformation::noUse)
			]
	];
}

LuaScript::RAIIState::RAIIState() :
	L(lua_open())
{
}

LuaScript::RAIIState::~RAIIState()
{
	lua_close(L);
}

LuaScript::RAIIState::operator lua_State*()
{
	return L;
}
