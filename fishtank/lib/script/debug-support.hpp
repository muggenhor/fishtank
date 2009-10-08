#ifndef __INCLUDED_LIB_SCRIPT_DEBUG_SUPPORT_HPP__
#define __INCLUDED_LIB_SCRIPT_DEBUG_SUPPORT_HPP__

extern "C" {
# include <lua.h>
}

void debug_register_with_lua(lua_State* L);

#endif // __INCLUDED_LIB_SCRIPT_DEBUG_SUPPORT_HPP__
