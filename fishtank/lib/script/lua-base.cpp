#include "lua-base.hpp"
#include <boost/bind.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/format.hpp>
#include <boost/system/error_code.hpp>
#include <boost/system/system_error.hpp>
#include <cstring>
#include <framework/vfs.hpp>
#include <fstream>
#include <luabind/luabind.hpp>
#include <luabind/exception_handler.hpp>
#include <luabind/tag_function.hpp>
extern "C" {
#include <lua.h>
#include <lualib.h>
}
#include <string>

// FIXME: #includes from top level dir
#include "../../main.hpp"

using namespace luabind;
using namespace std;
namespace fs = boost::filesystem;
using boost::format;
using boost::system::error_code;
using boost::system::get_system_category;
using boost::system::system_error;

// Cannot use LuaBind to implement this function because LuaBind catches exceptions
static int os_exit (lua_State* L)
{
	throw exit_exception(luaL_optint(L, 1, EXIT_SUCCESS));
}

struct LoadF
{
	LoadF(istream& is) :
		f(is)
	{
	}

	istream& f;
	char buff[LUAL_BUFFERSIZE];
};

static const char* getF(lua_State*, LoadF* lf, size_t* size)
{
	if (lf->f.eof())
		return NULL;

	lf->f.read(lf->buff, sizeof(lf->buff));
	*size = lf->f.gcount();
	return (*size > 0) ? lf->buff : NULL;
}

luabind::object lua_load(lua_State* L, std::istream& is, const std::string& chunkname)
{
	LoadF lf(is);
	const int n = lua_gettop(L);

	int c = lf.f.get();
	if (c == '#')
	{  /* Unix exec. file? */
		while ((c = lf.f.get()) != EOF && c != '\n') ;  /* skip first line */
	}
	lf.f.putback(c);
	const int status = lua_load(L, (const char* (*)(lua_State*, void*, size_t*)) (const char* (*)(lua_State*, LoadF*, size_t*)) &getF, &lf, chunkname.empty() ? NULL : chunkname.c_str());
	if (lf.f.bad())
	{
		lua_settop(L, n);  /* ignore results from `lua_load' */
		throw system_error(error_code(errno, get_system_category()));
	}
	if (status != 0)
		throw luabind::error(L);

	return object(from_stack(L, -1));
}

luabind::object lua_loadfile(lua_State* L, const boost::filesystem::path& path, const std::string& chunkname)
{
	ifstream is;
	vfs::open(is, path, ios_base::binary);
	if (!is.is_open())
		throw fs::filesystem_error("cannot open", path, error_code(errno, get_system_category()));

	try
	{
		return lua_load(L, is, chunkname);
	}
	catch (const system_error& e)
	{
		throw fs::filesystem_error("cannot read", path, e.code());
	}
}

luabind::object lua_loadfile(lua_State* L, const boost::filesystem::path& path)
{
	return lua_loadfile(L, path, "@" + path.string());
}

static int base_loadfile(lua_State* L)
{
	const char* fname = luaL_checkstring(L, 1);

	const boost::optional<object> chunk =
	  handle_exceptions<object>(L,
	    boost::bind(&lua_loadfile,
	      L, fname));

	if (chunk)
	{
		chunk->push(L);
		return 1;
	}

	lua_pushnil(L);
	lua_insert(L, lua_gettop(L) - 1); // Put the 'nil' before the error message
	return 2;
}

int lua_dostream(lua_State* L, std::istream& is, const std::string& chunkname)
{
	object chunk(lua_load(L, is, chunkname));

	const int n = lua_gettop(L);
	chunk.push(L);
	if (lua_pcall(L, 0, LUA_MULTRET, 0) != 0)
		throw luabind::error(L);
	return lua_gettop(L) - n;
}

int lua_dofile(lua_State* L, const boost::filesystem::path& path)
{
	fs::path file(path);

	for (unsigned int loop = 0; fs::is_symlink(file); ++loop)
	{
		if (loop > SYMLOOP_MAX)
			throw system_error(error_code(ELOOP, get_system_category()));
		file = readlink(file);
	}

	file = normalized_path(file);
	// Strip references to the current working directory (to make Lua related debug messages cleaner)
	while (file.string().substr(0, 2) == "./")
		file = file.string().substr(2);

	ifstream is;
	vfs::open(is, path, ios_base::binary);
	if (!is.is_open())
		throw fs::filesystem_error("cannot open", path, error_code(errno, get_system_category()));

	try
	{
		return lua_dostream(L, is, "@" + file.string());
	}
	catch (const system_error& e)
	{
		throw fs::filesystem_error("cannot read", path, e.code());
	}
}

static int base_dofile(lua_State* L)
{
	const char* fname = luaL_checkstring(L, 1);

	// Find out in what file our caller lives
	lua_Debug ar;
	if (!fs::path(fname).is_complete()
	 && lua_getstack(L, 1, &ar)
	 && lua_getinfo(L, "S", &ar)
	 && ar.short_src
	 && ar.short_src[0])
	{
		const boost::optional<int> n =
		  handle_exceptions<int>(L,
		    boost::bind(&lua_dofile,
		      L, fs::path(ar.short_src).parent_path() / fname));

		if (n)
			return *n;
	}
	else
	{
		const boost::optional<int> n =
		  handle_exceptions<int>(L,
		    boost::bind(&lua_dofile,
		      L, fname));

		if (n)
			return *n;
	}

	lua_error(L);
	return 0; // To shut up the compiler (lua_error never returns)
}

void lua_base_register_with_lua(lua_State* L)
{
	/*
	 * Functions that need to be ditched or overridden for safety:
	 *  * dofile
	 *  * loadfile
	 *  * print        -- overridden by debug_register_with_lua
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
	 */

	lua_pushcfunction(L, &base_loadfile);
	lua_setglobal(L, "loadfile");
	lua_pushcfunction(L, &base_dofile);
	lua_setglobal(L, "dofile");

	lua_getglobal(L, "os");
	lua_pushcfunction(L, &os_exit);
	lua_setfield(L, -2, "exit");
	lua_pop(L, 1); // pop the 'os' table
}
