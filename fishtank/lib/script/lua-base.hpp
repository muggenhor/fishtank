#ifndef __INCLUDED_LIB_SCRIPT_LUA_BASE_HPP__
#define __INCLUDED_LIB_SCRIPT_LUA_BASE_HPP__

#include <luabind/luabind.hpp>
#include <luabind/scope.hpp>

luabind::object lua_base_create_dofile_func(lua_State* L, const luabind::object& loadfile);

void lua_base_register_with_lua(lua_State* L);

#endif // __INCLUDED_LIB_SCRIPT_LUA_BASE_HPP__
