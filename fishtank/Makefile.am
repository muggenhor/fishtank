ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST= \
	autogen.sh \
	configure.ac

SUBDIRS = \
	Data \
	GL \
	Settings \
	vertexarrays

AM_CFLAGS=$(FT_CFLAGS)
AM_CXXFLAGS=$(FT_CXXFLAGS)
AM_CPPFLAGS=$(FT_CPPFLAGS)

bin_PROGRAMS = fishtank

fishtank_SOURCES = \
	AquariumController.cpp \
	Bubble.cpp \
	Environment.cpp \
	framerate.cpp \
	glexcept.cpp \
	Ground.cpp \
	imagereceiver.cpp \
	main.cpp \
	MS3D_ASCII.cpp \
	Object.cpp \
	shaders.cpp \
	textures.cpp \
	Vis.cpp \
	wiggle.cpp

noinst_HEADERS = \
	AquariumController.h \
	Bubble.h \
	Environment.h \
	framerate.hpp \
	glexcept.hpp \
	Ground.h \
	imagereceiver.h \
	main.hpp \
	math-helpers.hpp \
	MS3D_ASCII.h \
	Object.h \
	shaders.hpp \
	textures.hpp \
	Vis.h \
	wiggle.hpp

fishtank_LDADD = \
	$(builddir)/GL/libGLee.la \
	$(builddir)/vertexarrays/libvertexarrays.la \
	$(BOOST_PROGRAM_OPTIONS_LIBS) \
	$(BOOST_REGEX_LIBS) \
	$(GLFW_LIBS) \
	$(OPENGL_LIBS) \
	$(JPEG_LIBS) \
	$(WIN32_LIBS)

fishtank_CPPFLAGS = $(GLFW_CFLAGS) $(BOOST_CPPFLAGS) $(AM_CPPFLAGS)

dist_doc_DATA = \
	Handleiding.doc
