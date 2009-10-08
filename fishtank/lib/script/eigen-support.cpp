#include "eigen-support.hpp"
#include <Eigen/Array>
#include <Eigen/Core>
#include <luabind/luabind.hpp>
#include <luabind/operator.hpp>

namespace Eigen
{

static int    eigen_vector2i_get_x(const Eigen::Vector2i& v) { return v.x(); }
static void   eigen_vector2i_set_x(      Eigen::Vector2i& v, const int x) { v.x() = x; }
static int    eigen_vector2i_get_y(const Eigen::Vector2i& v) { return v.y(); }
static void   eigen_vector2i_set_y(      Eigen::Vector2i& v, const int y) { v.y() = y; }

static float  eigen_vector2f_get_x(const Eigen::Vector2f& v) { return v.x(); }
static void   eigen_vector2f_set_x(      Eigen::Vector2f& v, const float x) { v.x() = x; }
static float  eigen_vector2f_get_y(const Eigen::Vector2f& v) { return v.y(); }
static void   eigen_vector2f_set_y(      Eigen::Vector2f& v, const float y) { v.y() = y; }

static double eigen_vector2d_get_x(const Eigen::Vector2d& v) { return v.x(); }
static void   eigen_vector2d_set_x(      Eigen::Vector2d& v, const double x) { v.x() = x; }
static double eigen_vector2d_get_y(const Eigen::Vector2d& v) { return v.y(); }
static void   eigen_vector2d_set_y(      Eigen::Vector2d& v, const double y) { v.y() = y; }

static int    eigen_vector3i_get_x(const Eigen::Vector3i& v) { return v.x(); }
static void   eigen_vector3i_set_x(      Eigen::Vector3i& v, const int x) { v.x() = x; }
static int    eigen_vector3i_get_y(const Eigen::Vector3i& v) { return v.y(); }
static void   eigen_vector3i_set_y(      Eigen::Vector3i& v, const int y) { v.y() = y; }
static int    eigen_vector3i_get_z(const Eigen::Vector3i& v) { return v.z(); }
static void   eigen_vector3i_set_z(      Eigen::Vector3i& v, const int z) { v.z() = z; }

static float  eigen_vector3f_get_x(const Eigen::Vector3f& v) { return v.x(); }
static void   eigen_vector3f_set_x(      Eigen::Vector3f& v, const float x) { v.x() = x; }
static float  eigen_vector3f_get_y(const Eigen::Vector3f& v) { return v.y(); }
static void   eigen_vector3f_set_y(      Eigen::Vector3f& v, const float y) { v.y() = y; }
static float  eigen_vector3f_get_z(const Eigen::Vector3f& v) { return v.z(); }
static void   eigen_vector3f_set_z(      Eigen::Vector3f& v, const float z) { v.z() = z; }

static float  eigen_vector3d_get_x(const Eigen::Vector3d& v) { return v.x(); }
static void   eigen_vector3d_set_x(      Eigen::Vector3d& v, const float x) { v.x() = x; }
static float  eigen_vector3d_get_y(const Eigen::Vector3d& v) { return v.y(); }
static void   eigen_vector3d_set_y(      Eigen::Vector3d& v, const float y) { v.y() = y; }
static float  eigen_vector3d_get_z(const Eigen::Vector3d& v) { return v.z(); }
static void   eigen_vector3d_set_z(      Eigen::Vector3d& v, const float z) { v.z() = z; }

static bool eigen_vector2i_isApprox     (const Eigen::Vector2i& r, const Eigen::Vector2i& l)              { return r.isApprox(l);       }
static bool eigen_vector2i_isApprox_prec(const Eigen::Vector2i& r, const Eigen::Vector2i& l, int    prec) { return r.isApprox(l, prec); }
static bool eigen_vector2f_isApprox     (const Eigen::Vector2f& r, const Eigen::Vector2f& l)              { return r.isApprox(l);       }
static bool eigen_vector2f_isApprox_prec(const Eigen::Vector2f& r, const Eigen::Vector2f& l, float  prec) { return r.isApprox(l, prec); }
static bool eigen_vector2d_isApprox     (const Eigen::Vector2d& r, const Eigen::Vector2d& l)              { return r.isApprox(l);       }
static bool eigen_vector2d_isApprox_prec(const Eigen::Vector2d& r, const Eigen::Vector2d& l, double prec) { return r.isApprox(l, prec); }
static bool eigen_vector3i_isApprox     (const Eigen::Vector3i& r, const Eigen::Vector3i& l)              { return r.isApprox(l);       }
static bool eigen_vector3i_isApprox_prec(const Eigen::Vector3i& r, const Eigen::Vector3i& l, int    prec) { return r.isApprox(l, prec); }
static bool eigen_vector3f_isApprox     (const Eigen::Vector3f& r, const Eigen::Vector3f& l)              { return r.isApprox(l);       }
static bool eigen_vector3f_isApprox_prec(const Eigen::Vector3f& r, const Eigen::Vector3f& l, float  prec) { return r.isApprox(l, prec); }
static bool eigen_vector3d_isApprox     (const Eigen::Vector3d& r, const Eigen::Vector3d& l)              { return r.isApprox(l);       }
static bool eigen_vector3d_isApprox_prec(const Eigen::Vector3d& r, const Eigen::Vector3d& l, double prec) { return r.isApprox(l, prec); }

static Eigen::Vector2i eigen_vector2i_mul(const Eigen::Vector2i& v, int    s) { return v * s; }
static Eigen::Vector2f eigen_vector2f_mul(const Eigen::Vector2f& v, float  s) { return v * s; }
static Eigen::Vector2d eigen_vector2d_mul(const Eigen::Vector2d& v, double s) { return v * s; }
static Eigen::Vector3i eigen_vector3i_mul(const Eigen::Vector3i& v, int    s) { return v * s; }
static Eigen::Vector3f eigen_vector3f_mul(const Eigen::Vector3f& v, float  s) { return v * s; }
static Eigen::Vector3d eigen_vector3d_mul(const Eigen::Vector3d& v, double s) { return v * s; }

static Eigen::Vector2i eigen_vector2i_div(const Eigen::Vector2i& v, int    s) { return v / s; }
static Eigen::Vector2f eigen_vector2f_div(const Eigen::Vector2f& v, float  s) { return v / s; }
static Eigen::Vector2d eigen_vector2d_div(const Eigen::Vector2d& v, double s) { return v / s; }
static Eigen::Vector3i eigen_vector3i_div(const Eigen::Vector3i& v, int    s) { return v / s; }
static Eigen::Vector3f eigen_vector3f_div(const Eigen::Vector3f& v, float  s) { return v / s; }
static Eigen::Vector3d eigen_vector3d_div(const Eigen::Vector3d& v, double s) { return v / s; }

static Eigen::Vector2i eigen_vector2i_add(const Eigen::Vector2i& r, const Eigen::Vector2i& l) { return r + l; }
static Eigen::Vector2f eigen_vector2f_add(const Eigen::Vector2f& r, const Eigen::Vector2f& l) { return r + l; }
static Eigen::Vector2d eigen_vector2d_add(const Eigen::Vector2d& r, const Eigen::Vector2d& l) { return r + l; }
static Eigen::Vector3i eigen_vector3i_add(const Eigen::Vector3i& r, const Eigen::Vector3i& l) { return r + l; }
static Eigen::Vector3f eigen_vector3f_add(const Eigen::Vector3f& r, const Eigen::Vector3f& l) { return r + l; }
static Eigen::Vector3d eigen_vector3d_add(const Eigen::Vector3d& r, const Eigen::Vector3d& l) { return r + l; }

static Eigen::Vector2i eigen_vector2i_sub(const Eigen::Vector2i& r, const Eigen::Vector2i& l) { return r - l; }
static Eigen::Vector2f eigen_vector2f_sub(const Eigen::Vector2f& r, const Eigen::Vector2f& l) { return r - l; }
static Eigen::Vector2d eigen_vector2d_sub(const Eigen::Vector2d& r, const Eigen::Vector2d& l) { return r - l; }
static Eigen::Vector3i eigen_vector3i_sub(const Eigen::Vector3i& r, const Eigen::Vector3i& l) { return r - l; }
static Eigen::Vector3f eigen_vector3f_sub(const Eigen::Vector3f& r, const Eigen::Vector3f& l) { return r - l; }
static Eigen::Vector3d eigen_vector3d_sub(const Eigen::Vector3d& r, const Eigen::Vector3d& l) { return r - l; }

void register_with_lua(lua_State* L)
{
	using namespace luabind;

	module(L, "Eigen")
	[
		register_with_lua()
	];
}

luabind::scope register_with_lua()
{
	using namespace luabind;

	return
		class_<Eigen::Matrix4f>("Matrix4f")
			.def(const_self(int(), int())),

		class_<Eigen::Vector2i>("Vector2i")
			.def(constructor<int, int>())
			.def("__mul", &eigen_vector2i_mul)
			.def("__div", &eigen_vector2i_div)
			.def("__add", &eigen_vector2i_add)
			.def("__sub", &eigen_vector2i_sub)
			.def(const_self == const_self)
			.def("dot", &Eigen::Vector2i::dot<Eigen::Vector2i>)
			.def("isApprox", &eigen_vector2i_isApprox)
			.def("isApprox", &eigen_vector2i_isApprox_prec)
			.property("length", &Eigen::Vector2i::norm)
			.property("normalized", &Eigen::Vector2i::normalized)
			.property("x", &eigen_vector2i_get_x, &eigen_vector2i_set_x)
			.property("y", &eigen_vector2i_get_y, &eigen_vector2i_set_y),

		class_<Eigen::Vector2f>("Vector2f")
			.def(constructor<double, double>())
			.def("__mul", &eigen_vector2f_mul)
			.def("__div", &eigen_vector2f_div)
			.def("__add", &eigen_vector2f_add)
			.def("__sub", &eigen_vector2f_sub)
			.def(const_self == const_self)
			.def("dot", &Eigen::Vector2f::dot<Eigen::Vector2f>)
			.def("isApprox", &eigen_vector2f_isApprox)
			.def("isApprox", &eigen_vector2f_isApprox_prec)
			.property("length", &Eigen::Vector2f::norm)
			.property("normalized", &Eigen::Vector2f::normalized)
			.property("x", &eigen_vector2f_get_x, &eigen_vector2f_set_x)
			.property("y", &eigen_vector2f_get_y, &eigen_vector2f_set_y),

		class_<Eigen::Vector2d>("Vector2d")
			.def(constructor<double, double>())
			.def("__mul", &eigen_vector2d_mul)
			.def("__div", &eigen_vector2d_div)
			.def("__add", &eigen_vector2d_add)
			.def("__sub", &eigen_vector2d_sub)
			.def(const_self == const_self)
			.def("dot", &Eigen::Vector2d::dot<Eigen::Vector2d>)
			.def("isApprox", &eigen_vector2d_isApprox)
			.def("isApprox", &eigen_vector2d_isApprox_prec)
			.property("length", &Eigen::Vector2d::norm)
			.property("normalized", &Eigen::Vector2d::normalized)
			.property("x", &eigen_vector2d_get_x, &eigen_vector2d_set_x)
			.property("y", &eigen_vector2d_get_y, &eigen_vector2d_set_y),

		class_<Eigen::Vector3i>("Vector3i")
			.def(constructor<int, int, int>())
			.def("__mul", &eigen_vector3i_mul)
			.def("__div", &eigen_vector3i_div)
			.def("__add", &eigen_vector3i_add)
			.def("__sub", &eigen_vector3i_sub)
			.def(const_self == const_self)
			.def("dot", &Eigen::Vector3i::dot<Eigen::Vector3i>)
			.def("isApprox", &eigen_vector3i_isApprox)
			.def("isApprox", &eigen_vector3i_isApprox_prec)
			.property("length", &Eigen::Vector3i::norm)
			.property("normalized", &Eigen::Vector3i::normalized)
			.property("x", &eigen_vector3i_get_x, &eigen_vector3i_set_x)
			.property("y", &eigen_vector3i_get_y, &eigen_vector3i_set_y)
			.property("z", &eigen_vector3i_get_z, &eigen_vector3i_set_z),

		class_<Eigen::Vector3f>("Vector3f")
			.def(constructor<float, float, float>())
			.def("__mul", &eigen_vector3f_mul)
			.def("__div", &eigen_vector3f_div)
			.def("__add", &eigen_vector3f_add)
			.def("__sub", &eigen_vector3f_sub)
			.def(const_self == const_self)
			.def("isApprox", &eigen_vector3f_isApprox)
			.def("isApprox", &eigen_vector3f_isApprox_prec)
			.def("dot", &Eigen::Vector3f::dot<Eigen::Vector3f>)
			.property("length", &Eigen::Vector3f::norm)
			.property("normalized", &Eigen::Vector3f::normalized)
			.property("x", &eigen_vector3f_get_x, &eigen_vector3f_set_x)
			.property("y", &eigen_vector3f_get_y, &eigen_vector3f_set_y)
			.property("z", &eigen_vector3f_get_z, &eigen_vector3f_set_z),

		class_<Eigen::Vector3d>("Vector3d")
			.def(constructor<double, double, double>())
			.def("__mul", &eigen_vector3d_mul)
			.def("__div", &eigen_vector3d_div)
			.def("__add", &eigen_vector3d_add)
			.def("__sub", &eigen_vector3d_sub)
			.def("isApprox", &eigen_vector3d_isApprox)
			.def("isApprox", &eigen_vector3d_isApprox_prec)
			.def(const_self == const_self)
			.def("dot", &Eigen::Vector3d::dot<Eigen::Vector3d>)
			.property("length", &Eigen::Vector3d::norm)
			.property("normalized", &Eigen::Vector3d::normalized)
			.property("x", &eigen_vector3d_get_x, &eigen_vector3d_set_x)
			.property("y", &eigen_vector3d_get_y, &eigen_vector3d_set_y)
			.property("z", &eigen_vector3d_get_z, &eigen_vector3d_set_z);
}

}
