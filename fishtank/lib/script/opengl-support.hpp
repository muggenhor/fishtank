#ifndef __INCLUDED_LIB_SCRIPT_OPENGL_SUPPORT_HPP__
#define __INCLUDED_LIB_SCRIPT_OPENGL_SUPPORT_HPP__

#include <luabind/scope.hpp>

namespace OpenGL
{

void register_with_lua(lua_State* L);

luabind::scope register_with_lua();

}

#endif // __INCLUDED_LIB_SCRIPT_OPENGL_SUPPORT_HPP__
