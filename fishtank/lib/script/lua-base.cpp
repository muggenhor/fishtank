#include "lua-base.hpp"
#include <algorithm>
#include <boost/bind.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/format.hpp>
#include <boost/system/error_code.hpp>
#include <boost/system/system_error.hpp>
#include <cstring>
#include <framework/vfs.hpp>
#include <fstream>
#include <iterator>
#include <luabind/luabind.hpp>
#include <luabind/exception_handler.hpp>
#include <luabind/tag_function.hpp>
extern "C" {
#include <lua.h>
#include <lualib.h>
}
#include <string>
#include <vector>

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
		throw system_error(error_code(errno, get_system_category()), "Cannot read");
	}
	if (status != 0)
		throw luabind::error(L);

	object chunk(from_stack(L, -1));
	lua_settop(L, n); // Make sure the stack is as clean as we've left it
	return chunk;
}

luabind::object lua_loadfile(lua_State* L, const boost::filesystem::path& path, const std::string& chunkname)
{
	ifstream is;
	vfs::open(is, path, ios_base::binary);

	try
	{
		return lua_load(L, is, chunkname);
	}
	catch (const system_error& e)
	{
		throw fs::filesystem_error(e.std::runtime_error::what(), path, e.code());
	}
}

luabind::object lua_loadfile(lua_State* L, const boost::filesystem::path& path)
{
	return lua_loadfile(L, path, "@" + path.string());
}

static void base_loadfile(lua_State* L, const fs::path& fname)
{
	const boost::optional<object> chunk =
	  handle_exceptions<object>(L,
	    boost::bind(&lua_loadfile,
	      L, fname));

	if (chunk)
		return chunk->push(L);

	lua_pushnil(L);
	lua_insert(L, lua_gettop(L) - 1); // Put the 'nil' before the error message
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

	try
	{
		return lua_dostream(L, is, "@" + file.string());
	}
	catch (const system_error& e)
	{
		throw fs::filesystem_error("cannot read", path, e.code());
	}
}

static void base_dofile(lua_State* L, fs::path fname)
{
	// Find out in what file our caller lives
	lua_Debug ar;
	if (!fname.is_complete()
	 && lua_getstack(L, 1, &ar)
	 && lua_getinfo(L, "S", &ar)
	 && ar.short_src
	 && ar.short_src[0])
	{
		fname = fs::path(ar.short_src).parent_path() / fname;
	}

	lua_dofile(L, fname);
}

static object io_open_impl(lua_State* L, const fs::path& fname, const string& mode)
{
	if      (mode == "r")
		return object(L, vfs::open_read(fname, ios_base::in));
	else if (mode == "rb")
		return object(L, vfs::open_read(fname, ios_base::in|ios_base::binary));
	else if (mode == "w")
		return object(L, vfs::open_write(fname, ios_base::out|ios_base::trunc));
	else if (mode == "wb")
		return object(L, vfs::open_write(fname, ios_base::out|ios_base::trunc|ios_base::binary));
	else if (mode == "a")
		return object(L, vfs::open_write(fname, ios_base::out|ios_base::app|ios_base::ate));
	else if (mode == "ab")
		return object(L, vfs::open_write(fname, ios_base::out|ios_base::app|ios_base::ate|ios_base::binary));
	else if (mode == "r+")
		return object(L, vfs::open_readwrite(fname, ios_base::in|ios_base::out));
	else if (mode == "rb+"
	      || mode == "r+b")
		return object(L, vfs::open_readwrite(fname, ios_base::in|ios_base::out|ios_base::binary));
	else if (mode == "w+")
		return object(L, vfs::open_readwrite(fname, ios_base::in|ios_base::out|ios_base::trunc));
	else if (mode == "wb+"
	      || mode == "w+b")
		return object(L, vfs::open_readwrite(fname, ios_base::in|ios_base::out|ios_base::trunc|ios_base::binary));
	else if (mode == "a+")
		return object(L, vfs::open_readwrite(fname, ios_base::in|ios_base::out|ios_base::app));
	else if (mode == "ab+"
	      || mode == "a+b")
		return object(L, vfs::open_readwrite(fname, ios_base::in|ios_base::out|ios_base::app|ios_base::binary));
	else
		throw system_error(error_code(EINVAL, get_system_category()), "Invalid mode: " + mode);
}

static void io_open(lua_State* L, const fs::path& fname, const string& mode)
{
	const boost::optional<object> file =
	  handle_exceptions<object>(L,
	    boost::bind(&io_open_impl,
	      L, fname, mode));

	if (file)
		return file->push(L);

	lua_pushnil(L);
	lua_insert(L, lua_gettop(L) - 1); // Put the 'nil' before the error message
}

static void io_open(lua_State* L, const fs::path& fname)
{
	return io_open(L, fname, "r");
}

static const char* io_type(const istream&)    { return "input stream"; }
static const char* io_type(const ostream&)    { return "output stream"; }
static const char* io_type(const iostream&)   { return "input/output stream"; }
static const char* io_type(const ifstream& f) { return f.is_open() ? "file" : "closed file"; }
static const char* io_type(const ofstream& f) { return f.is_open() ? "file" : "closed file"; }
static const char* io_type(const fstream&  f) { return f.is_open() ? "file" : "closed file"; }
static void io_type(lua_State* L, const luabind::object&) { lua_pushnil(L); }

static ios_base::seekdir whence_to_seekdir(const string& whence)
{
	if      (whence == "cur")
		return ios_base::cur;
	else if (whence == "set"
	      || whence == "beg")
		return ios_base::beg;
	else if (whence == "end")
		return ios_base::end;
	else
		throw system_error(error_code(EINVAL, get_system_category()), "Invalid seek direction: " + whence);
}

static unsigned long int io_file_seek(istream& is, const string& whence, const long int offset)
{
	is.seekg(offset, whence_to_seekdir(whence));
	return is.tellg();
}

static unsigned long int io_file_seek(istream& is, const string& whence)
{
	return io_file_seek(is, whence, 0);
}

static unsigned long int io_file_seek(istream& is)
{
	return io_file_seek(is, "cur", 0);
}

static unsigned long int io_file_seek(ostream& os, const string& whence, const long int offset)
{
	os.seekp(offset, whence_to_seekdir(whence));
	return os.tellp();
}

static unsigned long int io_file_seek(ostream& os, const string& whence)
{
	return io_file_seek(os, whence, 0);
}

static unsigned long int io_file_seek(ostream& os)
{
	return io_file_seek(os, "cur", 0);
}

static unsigned long int io_file_seek(iostream& ios, const string& whence, const long int offset)
{
	const ios_base::seekdir dir(whence_to_seekdir(whence));

	ios.seekg(offset, dir);
	ios.seekp(offset, dir);
	return ios.tellp();
}

static unsigned long int io_file_seek(iostream& ios, const string& whence)
{
	return io_file_seek(ios, whence, 0);
}

static unsigned long int io_file_seek(iostream& ios)
{
	return io_file_seek(ios, "cur", 0);
}

static int next_file_line(lua_State* L)
{
	boost::shared_ptr<istream> is(object_cast< boost::shared_ptr<istream> >(object(from_stack(L, lua_upvalueindex(1)))));
	if (!is
	 || is->eof())
		return 0;

	string line;
	getline(*is, line);

	lua_pushlstring(L, line.data(), line.length());
	return 1;
}

static void io_file_lines(lua_State* L, boost::shared_ptr<istream> is)
{
	object(L, is).push(L);
	return lua_pushcclosure(L, &next_file_line, 1);
}

static void io_file_lines(lua_State* L, boost::shared_ptr<ifstream> is)
{
	return io_file_lines(L, boost::static_pointer_cast<istream>(is));
}

static void io_file_lines(lua_State* L, boost::shared_ptr<fstream> is)
{
	return io_file_lines(L, boost::static_pointer_cast<istream>(is));
}

static void io_file_lines(lua_State* L, boost::shared_ptr<fs::ifstream> is)
{
	return io_file_lines(L, boost::static_pointer_cast<istream>(is));
}

static void io_file_lines(lua_State* L, boost::shared_ptr<fs::fstream> is)
{
	return io_file_lines(L, boost::static_pointer_cast<istream>(is));
}

static void io_file_read(lua_State* L, istream& is, const string& format);
static void io_file_read(lua_State* L, istream& is)
{
	if (is.eof())
		return lua_pushnil(L);

	string line;
	getline(is, line);

	return lua_pushlstring(L, line.data(), line.length());
}

static void io_file_read(lua_State* L, istream& is, const string& format)
{
	if      (format == "*n")
	{
		double num;
		is >> num;
		return lua_pushnumber(L, num);
	}
	else if (format == "*a")
	{
		vector<char> buf;
		/* Possible bug in GCC? Passing this iterator set to the
		 * constructor somehow seems to cause the type of 'buf' to
		 * change in such a way that the lua_pushlstring-line fails to
		 * compile (claiming arithmetic is being performed on a
		 * function pointer).
		 */
		buf.assign(istreambuf_iterator<char>(is), istreambuf_iterator<char>());

		return lua_pushlstring(L, &buf[0], buf.size());
	}
	else if (format == "*l")
		return io_file_read(L, is);
	else
		throw system_error(error_code(EINVAL, get_system_category()), "Invalid read format: " + format);
}

static void io_file_read(lua_State* L, istream& is, const unsigned long int size)
{
	if (is.eof())
		return lua_pushnil(L);

	vector<char> buf(size);
	is.read(&buf[0], buf.size());

	if (is.gcount() == 0
	 && is.eof())
		return lua_pushnil(L);

	return lua_pushlstring(L, &buf[0], is.gcount());
}

static void io_file_write(ostream& os,
	const string& a)
{
	os << a;
}

static void io_file_write(ostream& os,
	const string& a,
	const string& b)
{
	os << a << b;
}

static void io_file_write(ostream& os,
	const string& a,
	const string& b,
	const string& c)
{
	os << a << b << c;
}

static void io_file_write(ostream& os,
	const string& a,
	const string& b,
	const string& c,
	const string& d)
{
	os << a << b << c << d;
}

static void io_file_write(ostream& os,
	const string& a,
	const string& b,
	const string& c,
	const string& d,
	const string& e)
{
	os << a << b << c << d << e;
}

/**
 * Don't prefix @c boost::system::system_error exceptions with
 * 'std::runtime_error'. Instead format their messages only according to
 * @c e.what().
 */
static void translate_system_error_exception(lua_State* L, const system_error& e)
{
	lua_pushstring(L, e.what());
}

void lua_base_register_with_lua(lua_State* L)
{
	/*
	 * Functions that need to be ditched or overridden for safety:
	 *  * dofile
	 *  * loadfile
	 *  * print        -- overridden by debug_register_with_lua
	 *  * io.close
	 *  * io.flush
	 *  * io.input
	 *  * io.lines
	 *  * io.open      -- overridden by lua_base_register_with_lua
	 *  * io.output
	 *  * io.popen
	 *  * io.read
	 *  * io.stderr
	 *  * io.stdin
	 *  * io.stdout
	 *  * io.type      -- overridden by lua_base_register_with_lua
	 *  * io.write
	 *  * os.execute
	 *  * os.exit      -- overridden by lua_base_register_with_lua
	 *  * os.getenv
	 *  * os.remove
	 *  * os.rename
	 *  * os.setlocale
	 *
	 * To implement member functions of class file
	 *  * file:close()                   -- ifstream,ofstream,fstream
	 *  * file:flush()                   -- ostream
	 *  * file:lines()                   -- istream
	 *  * file:read(...)                 -- istream
	 *  * file:seek([whence] [, offset]) -- istream,ostream,iostream
	 *  * file:setvbuf(mode, [, size])   -- ostream Don't implement....
	 *  * file:write(...)                -- ostream
	 */

	module(L)
	[
		def("loadfile", tag_function<void (lua_State*, const string&)>(&base_loadfile), raw(_1)),
		def("dofile",   tag_function<void (lua_State*, const string&)>(&base_dofile),   raw(_1))
	];

	module(L, "io")
	[
		def("open", tag_function<void (lua_State*, const string&, const string&)>((void (*)(lua_State*, const fs::path&, const string&)) &io_open), raw(_1)),
		def("open", tag_function<void (lua_State*, const string&)>((void (*)(lua_State*, const fs::path&)) &io_open), raw(_1)),

		def("type", (const char* (*)(const istream&))  &io_type),
		def("type", (const char* (*)(const ostream&))  &io_type),
		def("type", (const char* (*)(const iostream&)) &io_type),
		def("type", (const char* (*)(const ifstream&)) &io_type),
		def("type", (const char* (*)(const ofstream&)) &io_type),
		def("type", (const char* (*)(const fstream&))  &io_type),
		def("type", (void (*)(lua_State*, const luabind::object&)) &io_type, raw(_1)),

		class_<istream, boost::shared_ptr<istream> >("file")
			.def("lines", (void (*)(lua_State* L, boost::shared_ptr<istream>)) &io_file_lines, raw(_1))
			.def("read",  (void (*)(lua_State*, istream&)) &io_file_read, raw(_1))
			.def("read",  (void (*)(lua_State*, istream&, const string&)) &io_file_read, raw(_1))
			.def("read",  (void (*)(lua_State*, istream&, unsigned long int)) &io_file_read, raw(_1))
			.def("seek",  (unsigned long int (*)(istream&)) &io_file_seek)
			.def("seek",  (unsigned long int (*)(istream&, const string&)) &io_file_seek)
			.def("seek",  (unsigned long int (*)(istream&, const string&, long int)) &io_file_seek),

		class_<ifstream, istream, boost::shared_ptr<ifstream> >("file")
			.def("lines", (void (*)(lua_State* L, boost::shared_ptr<ifstream>)) &io_file_lines, raw(_1))
			.def("close", &ifstream::close),

		class_<ostream, boost::shared_ptr<ostream> >("file")
			.def("flush", &ofstream::flush)
			.def("seek",  (unsigned long int (*)(ostream&)) &io_file_seek)
			.def("seek",  (unsigned long int (*)(ostream&, const string&)) &io_file_seek)
			.def("seek",  (unsigned long int (*)(ostream&, const string&, long int)) &io_file_seek)
			//.def("setvbuf") // Not implmented on purpose
			.def("write", (void (*)(ostream&, const string&)) &io_file_write)
			.def("write", (void (*)(ostream&, const string&, const string&)) &io_file_write)
			.def("write", (void (*)(ostream&, const string&, const string&, const string&)) &io_file_write)
			.def("write", (void (*)(ostream&, const string&, const string&, const string&, const string&)) &io_file_write)
			.def("write", (void (*)(ostream&, const string&, const string&, const string&, const string&, const string&)) &io_file_write),

		class_<ofstream, ostream, boost::shared_ptr<ofstream> >("file")
			.def("close", &ofstream::close),

		class_<iostream, bases<istream, ostream>, boost::shared_ptr<iostream> >("file")
			.def("seek", (unsigned long int (*)(iostream&)) &io_file_seek)
			.def("seek", (unsigned long int (*)(iostream&, const string&)) &io_file_seek)
			.def("seek", (unsigned long int (*)(iostream&, const string&, long int)) &io_file_seek),

		class_<fstream, iostream, boost::shared_ptr<fstream> >("file")
			.def("close", &fstream::close)
			.def("lines", (void (*)(lua_State* L, boost::shared_ptr<fstream>)) &io_file_lines, raw(_1)),

		class_<fs::ifstream, ifstream, boost::shared_ptr<fs::ifstream> >("file")
			.def("lines", (void (*)(lua_State* L, boost::shared_ptr<fs::ifstream>)) &io_file_lines, raw(_1)),

		class_<fs::ofstream, ofstream, boost::shared_ptr<fs::ofstream> >("file"),

		class_<fs::fstream, fstream, boost::shared_ptr<fs::fstream> >("file")
			.def("lines", (void (*)(lua_State* L, boost::shared_ptr<fs::fstream>)) &io_file_lines, raw(_1))
	];

	/* These classes where only registered with Luabind to make sure
	 * Luabind can work with them, they shouldn't be directly accessible
	 * from within Lua itself.
	 */
	globals(L)["io"]["file"] = nil;

	lua_getglobal(L, "os");
	lua_pushcfunction(L, &os_exit);
	lua_setfield(L, -2, "exit");
	lua_pop(L, 1); // pop the 'os' table

	// Don't prefix system_error exceptions with 'std::runtime_error'
	luabind::register_exception_handler<system_error>(&translate_system_error_exception);
}
