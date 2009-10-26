#ifndef __INCLUDED_LIB_SCRIPT_LUA_BASE_HPP__
#define __INCLUDED_LIB_SCRIPT_LUA_BASE_HPP__

#include <luabind/luabind.hpp>
#include <istream>
#include <string>

// Forward declaration to allow pointers
struct lua_State;
namespace boost { namespace filesystem {
template<class String, class Traits> class basic_path;
struct path_traits;
typedef basic_path< std::string, path_traits > path;
}}

/**
 * @pre @c is is a binary stream.
 */
luabind::object lua_load(lua_State* L, std::istream& is, const std::string& chunkname = "");

luabind::object lua_loadfile(lua_State* L, const boost::filesystem::path& path, const std::string& chunkname);

luabind::object lua_loadfile(lua_State* L, const boost::filesystem::path& path);

int lua_dostream(lua_State* L, std::istream& is, const std::string& chunkname = "");

int lua_dofile(lua_State* L, const boost::filesystem::path& path);

void lua_base_register_with_lua(lua_State* L);

#endif // __INCLUDED_LIB_SCRIPT_LUA_BASE_HPP__
