#include "debug-support.hpp"
#include <framework/debug.hpp>
#include <luabind/luabind.hpp>
#include <string>

extern "C" {
# include <lua.h>
}

using namespace luabind;
using namespace std;

static int lua_DebugPrint(lua_State* L, code_part part)
{
	const int argc = lua_gettop(L);
	string function;

	// Get information from our caller
	lua_Debug ar;
	if (lua_getstack(L, 1, &ar)
	 && lua_getinfo(L, "Snl", &ar))
	{
		if (ar.short_src
		 && ar.short_src[0])
		{
			function += ar.short_src;
			function += ":";

			if (ar.currentline > 0
			 || ar.linedefined > 0)
			{
				char currentline[32];

				snprintf(currentline, sizeof(currentline), "%d:",
				  ar.currentline > 0 ?
				    ar.currentline :
				    ar.linedefined);
				function += currentline;
			}
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
			function += "()";
		}
	}
	else
	{
		function = "lua:unknown-function";
	}

	DebugStream stream(_debug(part, function.c_str()));

	for (int i = 1; i <= argc; ++i)
	{
		object arg(from_stack(L, i));

		string str = call_function<string>(L, "tostring", arg);
		stream << str;

		// Separate arguments by spaces
		if (i < argc
		 && str[str.length() - 1] != '\n'
		 && str[str.length() - 1] != ' '
		 && str[str.length() - 1] != '\t')
			stream << '\t';
	}

	// no (i.e. zero) return values
	return 0;
}

static int lua_FishDebug(lua_State* L)
{
	try
	{
		if (lua_gettop(L) < 1)
		{
			lua_Debug ar;
			if (!lua_getstack(L, 1, &ar)
			 || lua_getinfo(L, "n", &ar))
				ar.name = NULL;

			string message("bad argument #1 to '" + string(ar.name ? ar.name : "debug") + "' (string or number expected, got no value)");
			throw std::runtime_error(message);
		}

		object log_part(from_stack(L, 1));
		switch (type(log_part))
		{
			case LUA_TNUMBER:
			{
				code_part log_part_num = object_cast<code_part>(log_part);
				if (log_part_num >= LOG_LAST)
					throw "unknown debug log part";

				lua_remove(L, 1);
				return lua_DebugPrint(L, log_part_num);
			}
			case LUA_TSTRING:
			{
				const boost::array<std::string, LOG_LAST>::const_iterator
				  log_part_num = find(
				    DebugStream::debug_level_names.begin(),
				    DebugStream::debug_level_names.end(),
				    object_cast<std::string>(log_part)
				  );

				if (log_part_num == DebugStream::debug_level_names.end())
					throw "unknown debug log part";

				lua_remove(L, 1);
				return lua_DebugPrint(L, static_cast<code_part>(distance(
				    DebugStream::debug_level_names.begin(),
				    log_part_num
				  )));
			}
			default:
			{
				lua_Debug ar;
				if (!lua_getstack(L, 1, &ar)
				 || lua_getinfo(L, "n", &ar))
					ar.name = NULL;

				string message("bad argument #1 to '" + string(ar.name ? ar.name : "debug") + "' (string or number expected, got another type)");
				throw std::runtime_error(message);
			}
		}

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
	lua_register(L, "print", &lua_Print);

	// Register 'debug' (acts similar to the C++ version, without stream operators, instead infinite arguments are allowed)
	lua_register(L, "debug", &lua_FishDebug);

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

luabind::scope debug_register_with_lua()
{
	using namespace luabind;

	return
		// Substitute our own version of print
		def("print", &lua_Print, raw(_1)),
		// Register 'debug' (acts similar to the C++ version, without stream operators, instead infinite arguments are allowed)
		def("debug", &lua_FishDebug, raw(_1))
		;
}
