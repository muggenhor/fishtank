#ifndef __INCLUDED_LIB_SCRIPT_DEBUG_SUPPORT_HPP__
#define __INCLUDED_LIB_SCRIPT_DEBUG_SUPPORT_HPP__

#include <luabind/scope.hpp>

void debug_register_with_lua(lua_State* L);

luabind::scope debug_register_with_lua();

#endif // __INCLUDED_LIB_SCRIPT_DEBUG_SUPPORT_HPP__
