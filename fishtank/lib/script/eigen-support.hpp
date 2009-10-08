#ifndef __INCLUDED_LIB_SCRIPT_EIGEN_SUPPORT_HPP__
#define __INCLUDED_LIB_SCRIPT_EIGEN_SUPPORT_HPP__

#include <luabind/scope.hpp>

namespace Eigen
{
	void register_with_lua(lua_State* L);

	luabind::scope register_with_lua();
};

#endif // __INCLUDED_LIB_SCRIPT_EIGEN_SUPPORT_HPP__
