ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST= \
	autogen.sh \
	configure.ac

SUBDIRS = \
	Data \
	lib \
	GL \
	Settings \
	vertexarrays

AM_CFLAGS=$(FT_CFLAGS)
AM_CXXFLAGS=$(FT_CXXFLAGS)
AM_CPPFLAGS=$(FT_CPPFLAGS)

bin_PROGRAMS = fishtank

fishtank_SOURCES = \
	aquarium.cpp \
	Bubble.cpp \
	camera.cpp \
	Environment.cpp \
	framerate.cpp \
	glexcept.cpp \
	Ground.cpp \
	main.cpp \
	MS3D_ASCII.cpp \
	object.cpp \
	shaders.cpp \
	static-object.cpp \
	textures.cpp \
	Vis.cpp \
	wiggle.cpp

noinst_HEADERS = \
	aquarium.hpp \
	Bubble.h \
	camera.hpp \
	Environment.h \
	framerate.hpp \
	glexcept.hpp \
	Ground.h \
	image.hpp \
	main.hpp \
	math-helpers.hpp \
	MS3D_ASCII.h \
	object.hpp \
	shaders.hpp \
	static-object.hpp \
	textures.hpp \
	Vis.h \
	wiggle.hpp

fishtank_LDADD = \
	$(builddir)/GL/libGLee.la \
	$(builddir)/lib/framework/libframework.la \
	$(builddir)/lib/script/libscript.la \
	$(builddir)/vertexarrays/libvertexarrays.la \
	$(BOOST_PROGRAM_OPTIONS_LIBS) \
	$(BOOST_REGEX_LIBS) \
	$(BOOST_THREAD_LIBS) \
	$(GLFW_LIBS) \
	$(LUA_LIBS) \
	$(LUABIND_LIBS) \
	$(OPENCV_LIBS) \
	$(OPENGL_LIBS) \
	$(JPEG_LIBS) \
	$(PNG_LIBS) \
	$(WIN32_LIBS)

fishtank_LDFLAGS = \
	$(BOOST_PROGRAM_OPTIONS_LDFLAGS) \
	$(BOOST_REGEX_LDFLAGS) \
	$(BOOST_THREAD_LDFLAGS)

fishtank_CPPFLAGS = -I$(srcdir)/lib $(GLFW_CFLAGS) $(BOOST_CPPFLAGS) $(LUA_CFLAGS) $(LUABIND_CFLAGS) $(AM_CPPFLAGS)

dist_doc_DATA = \
	Handleiding.doc
