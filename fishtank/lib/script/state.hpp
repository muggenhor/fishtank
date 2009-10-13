#ifndef __INCLUDED_LIB_SCRIPT_STATE_HPP__
#define __INCLUDED_LIB_SCRIPT_STATE_HPP__

#include <boost/noncopyable.hpp>
#include <luabind/luabind.hpp>

// Forward declaration to allow pointers
struct lua_State;

class LuaScript
{
	public:
		LuaScript();

		operator lua_State*();

		template<class Key>
		luabind::adl::index_proxy<luabind::object> operator[](Key const& key)
		{
			return luabind::adl::index_proxy<luabind::object>(
					globals, L, key
					);
		}

	private:
		void register_safe_default_lua_libs();
		void register_interfaces();

		class RAIIState : public boost::noncopyable
		{
			public:
				RAIIState();
				~RAIIState();

				operator lua_State*();

			private:
				struct lua_State* const L;
		};

	private:
		RAIIState L;
		const luabind::object globals;
};

#endif // __INCLUDED_LIB_SCRIPT_STATE_HPP__
