#include "debug-support.hpp"
#include <framework/debug.hpp>
#include <luabind/luabind.hpp>
#include <string>

extern "C" {
# include <lua.h>
}

using namespace luabind;
using namespace std;

static int lua_DebugPrint(lua_State* L, code_part part, int first_arg = 1)
{
	const int argc = lua_gettop(L);
	string function;

	// Get information from our caller
	lua_Debug ar;
	if (lua_getstack(L, 1, &ar)
	 && lua_getinfo(L, "nS", &ar))
	{
		if (ar.short_src
		 && ar.short_src[0])
		{
			char linedefined[32];
			snprintf(linedefined, sizeof(linedefined), "%d", ar.linedefined);
			function += ar.short_src;
			function += ":";
			function += linedefined;
			function += ":";
		}

		function += ar.what ? ar.what : "unknown";

		if (ar.namewhat
		 && ar.namewhat[0])
		{
			function += ":";
			function += ar.namewhat;
		}

		if (ar.name
		 && ar.name[0])
		{
			function += ":";
			function += ar.name;
		}
	}
	else
	{
		function = "lua:unknown-function";
	}

	DebugStream stream(_debug(part, function.c_str()));

	for (int arg = first_arg; arg <= argc; ++arg)
	{
		lua_getglobal(L, "tostring");
		lua_pushvalue(L, arg);
		if (lua_pcall(L, 1, 1, 0))
		{
			const char* const err_str = lua_tostring(L, -1);
			throw err_str;
		}

		const char* const str = lua_tostring(L, -1);
		if (!str)
		{
			throw "non-string argument";
		}

		stream << str;
		lua_pop(L, 1);

		// Separate arguments by spaces
		if (arg < argc
		 && str[strlen(str) - 1] != '\n')
			stream << ' ';
	}

	// no (i.e. zero) return values
	return 0;
}

static int lua_FishDebug(lua_State* L)
{
	try
	{
		if (lua_gettop(L) < 1
		 || (!lua_isnumber(L, 1)
		  && !lua_isstring(L, 1)))
		{
			lua_Debug ar;
			if (!lua_getstack(L, 1, &ar)
			 || lua_getinfo(L, "n", &ar))
				ar.name = NULL;
	
			string message("bad argument #1 to '" + string(ar.name ? ar.name : "debug") + "' (string or number expected, got ");
			if (lua_gettop(L) < 1)
				message += "no value)";
			else
				message += "another type)";
			throw std::runtime_error(message);
		}
	
		if (lua_isnumber(L, 1))
			return lua_DebugPrint(L, static_cast<code_part>(lua_tonumber(L, 1)), 2);
	
		assert(lua_isstring(L, 1) && "coding error");
	
		const boost::array<std::string, LOG_LAST>::const_iterator
		  log_part = find(
		    DebugStream::debug_level_names.begin(),
		    DebugStream::debug_level_names.end(),
		    lua_tostring(L, 1)
		  );
	
		if (log_part == DebugStream::debug_level_names.end())
		{
			throw "unknown debug log part";
		}
	
		return lua_DebugPrint(L, static_cast<code_part>(distance(
		    DebugStream::debug_level_names.begin(),
		    log_part
		  )), 2);
	}
	catch (std::exception const& e)
	{
		lua_pushstring(L, e.what());
	}
	catch (const char* e)
	{
		lua_pushstring(L, e);
	}
	lua_error(L); // never returns (long jumps back into Lua)
	return 0; // shut up the compiler
}

static int lua_Print(lua_State* L)
{
	try
	{
		return lua_DebugPrint(L, LOG_SCRIPT);
	}
	catch (std::exception const& e)
	{
		lua_pushstring(L, e.what());
	}
	catch (const char* e)
	{
		lua_pushstring(L, e);
	}
	lua_error(L); // never returns (long jumps back into Lua)
	return 0; // shut up the compiler
}

void debug_register_with_lua(lua_State* L)
{
	// Substitute our own version of print
	lua_pushcfunction(L, &lua_Print);
	lua_setglobal(L, "print");

	// Register 'debug' (acts similar to the C++ version, without stream operators, instead infinite arguments are allowed)
	lua_pushcfunction(L, &lua_FishDebug);
	lua_setglobal(L, "debug");

	globals(L)["LOG_ERROR"] = LOG_ERROR;
	globals(L)["LOG_WARNING"] = LOG_WARNING;
	globals(L)["LOG_NEVER"] = LOG_NEVER;
	globals(L)["LOG_SCRIPT"] = LOG_SCRIPT;
	globals(L)["LOG_3D"] = LOG_3D;
	globals(L)["LOG_MAIN"] = LOG_MAIN;
	globals(L)["LOG_MEDIA"] = LOG_MEDIA;
	globals(L)["LOG_CAMERA"] = LOG_CAMERA;
	globals(L)["LOG_NET"] = LOG_NET;
	globals(L)["LOG_RPC"] = LOG_RPC;
	globals(L)["LOG_MEMORY"] = LOG_MEMORY;
	globals(L)["LOG_GUI"] = LOG_GUI;
}
