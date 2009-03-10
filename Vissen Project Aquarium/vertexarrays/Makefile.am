AM_CFLAGS=$(FT_CFLAGS)
AM_CXXFLAGS=$(FT_CXXFLAGS)
AM_CPPFLAGS=$(FT_CPPFLAGS)

noinst_LIBRARIES = libvertexarrays.a
libvertexarrays_a_SOURCES = \
	vertexbuffer.cpp
noinst_HEADERS = \
	abstractarray.hpp \
	gl_type_constants.hpp \
	texcoordarray.hpp \
	trianglearray.hpp \
	vertexarray.hpp \
	vertexbuffer.hpp

libvertexarrays_a_CPPFLAGS = $(BOOST_CPPFLAGS) $(AM_CPPFLAGS)
