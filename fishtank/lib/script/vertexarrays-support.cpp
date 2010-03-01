#include "vertexarrays-support.hpp"
#include <luabind/luabind.hpp>
#include <luabind/raw_policy.hpp>

// FIXME: #includes from top level dir
#include "../../vertexarrays/trianglearray.hpp"

using namespace luabind;

template <typename IndexIntegerType, typename VertexCoordType, typename TexCoordType, typename NormalCoordType, std::size_t textureCount, bool supportVertexVBOs, bool supportTexVBOs, bool supportNormalVBOs, std::size_t VertexCoordinateCount, std::size_t TexCoordCount>
void lua_TriangleArray_AddPoint(lua_State* L,
              TriangleArray<IndexIntegerType, VertexCoordType, TexCoordType, NormalCoordType, textureCount, supportVertexVBOs, supportTexVBOs, supportNormalVBOs, VertexCoordinateCount, TexCoordCount>& array,
              const typename VertexArray<VertexCoordType, VertexCoordinateCount, supportVertexVBOs>::value_type& vertex,
              const object& lua_texcoords,
              const typename NormalArray<NormalCoordType, supportNormalVBOs>::value_type& normal)
{
	typedef TriangleArray<IndexIntegerType, VertexCoordType, TexCoordType, NormalCoordType, textureCount, supportVertexVBOs, supportTexVBOs, supportNormalVBOs, VertexCoordinateCount, TexCoordCount> array_t;
	typedef typename array_t::vertex_type   vertex_type;
	typedef typename array_t::texcoord_type texcoord_type;
	typedef typename array_t::normal_type   normal_type;

	luaL_checktype(L, 3, LUA_TTABLE);
	if (lua_objlen(L, 3) != textureCount)
	{
		const char *msg = lua_pushfstring(L, "list of %u texture coordinates expected, got %u",
		                                  static_cast<unsigned int>(textureCount), lua_objlen(L, 3));
		luaL_argerror(L, 3, msg);
		return;
	}

	boost::array<texcoord_type, textureCount> texcoords;
	typename boost::array<texcoord_type, textureCount>::iterator out_texcoords = texcoords.begin();

	for (iterator i(lua_texcoords), end; i != end; ++i)
	{
		assert(out_texcoords != texcoords.end());
		*out_texcoords++ = object_cast<texcoord_type>(*i);
	}

	array.AddPoint(vertex, texcoords.begin(), texcoords.end(), normal);
}

template <typename IndexIntegerType, typename VertexCoordType, typename TexCoordType, typename NormalCoordType, std::size_t textureCount, bool supportVertexVBOs, bool supportTexVBOs, bool supportNormalVBOs, std::size_t VertexCoordinateCount, std::size_t TexCoordCount>
static scope triangle_array_register_with_lua(const char* const className)
{
	typedef TriangleArray<IndexIntegerType, VertexCoordType, TexCoordType, NormalCoordType, textureCount, supportVertexVBOs, supportTexVBOs, supportNormalVBOs, VertexCoordinateCount, TexCoordCount> array_t;
	typedef typename array_t::vertex_type   vertex_type;
	typedef typename array_t::texcoord_type texcoord_type;
	typedef typename array_t::normal_type   normal_type;

	return
		class_<array_t>(className)
			.def(constructor<>())
			.def(constructor<bool>())
			.def(constructor<const array_t&>())
			.def("draw", &array_t::draw)
			.def("empty", &array_t::empty)
			.def("clear", &array_t::clear)
#if 0 // These functions aren't safe to be instantiated (they contain type-related coding errors)
			.def("ModelViewLeftMult", &array_t::ModelViewLeftMult)
			.def("ModelViewMult", &array_t::ModelViewMult)
			.def("TextureLeftMult", &array_t::TextureLeftMult)
#endif
			.def("TextureMult", &array_t::TextureMult)
			.def("HasVertexVBO", &array_t::HasVertexVBO)
			.def("UseVertexVBO", &array_t::UseVertexVBO)
			.def("HasTexVBO", &array_t::HasTexVBO)
			.def("UseTexVBO", &array_t::UseTexVBO)
			.def("HasNormalVBO", &array_t::HasNormalVBO)
			.def("UseNormalVBO", &array_t::UseNormalVBO)
			.def("UseVBOs", &array_t::UseVBOs)
			.def("uniqueVertices", &array_t::uniqueVertices)
			.def("drawnVertices", &array_t::drawnVertices)
			.def("AddTriangle", (void (array_t::*)(const vertex_type*, const texcoord_type*, const normal_type*)) &array_t::AddTriangle)
			.def("AddTriangle", (void (array_t::*)(const vertex_type*, const normal_type*)) &array_t::AddTriangle)
			.def("AddPoint", (void (array_t::*)(const vertex_type&, const texcoord_type&, const normal_type&)) &array_t::AddPoint)
			.def("AddPoint", &lua_TriangleArray_AddPoint<IndexIntegerType, VertexCoordType, TexCoordType, NormalCoordType, textureCount, supportVertexVBOs, supportTexVBOs, supportNormalVBOs, VertexCoordinateCount, TexCoordCount>, raw(_1))
		;
}

void vertexarrays_register_with_lua(lua_State* L)
{
	module(L)
	[
		vertexarrays_register_with_lua()
	];

	// Some aliases
	globals(L)["TriangleArray"]   = globals(L)["TriangleArrayNRRR"];
	globals(L)["TriangleArrayT2"] = globals(L)["TriangleArrayNRRRT2"];
}

luabind::scope vertexarrays_register_with_lua()
{
	using namespace luabind;

	return
		triangle_array_register_with_lua<unsigned int, float, int,   float, 1, true, true, true, 3, 2>("TriangleArrayNRZR"),
		triangle_array_register_with_lua<unsigned int, float, float, float, 1, true, true, true, 3, 2>("TriangleArrayNRRR"),
		triangle_array_register_with_lua<unsigned int, float, int,   float, 2, true, true, true, 3, 2>("TriangleArrayNRZRT2"),
		triangle_array_register_with_lua<unsigned int, float, float, float, 2, true, true, true, 3, 2>("TriangleArrayNRRRT2")
		;
}
