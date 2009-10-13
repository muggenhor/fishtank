#ifndef __INCLUDED_LIB_SCRIPT_VERTEXARRAYS_SUPPORT_HPP__
#define __INCLUDED_LIB_SCRIPT_VERTEXARRAYS_SUPPORT_HPP__

#include <luabind/scope.hpp>

void vertexarrays_register_with_lua(lua_State* L);

luabind::scope vertexarrays_register_with_lua();

#endif // __INCLUDED_LIB_SCRIPT_VERTEXARRAYS_SUPPORT_HPP__
