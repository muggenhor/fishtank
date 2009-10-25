#ifndef __INCLUDED_LIB_SCRIPT_LUA_BASE_HPP__
#define __INCLUDED_LIB_SCRIPT_LUA_BASE_HPP__

#include <string>

// Forward declaration to allow pointers
struct lua_State;
namespace boost { namespace filesystem {
template<class String, class Traits> class basic_path;
struct path_traits;
typedef basic_path< std::string, path_traits > path;
}}

int lua_base_dofile(lua_State* L, const boost::filesystem::path& path);

void lua_base_register_with_lua(lua_State* L);

#endif // __INCLUDED_LIB_SCRIPT_LUA_BASE_HPP__
