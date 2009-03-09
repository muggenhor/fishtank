ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST= \
	autogen.sh \
	configure.ac

SUBDIRS = GL

bin_PROGRAMS = fishtank

fishtank_SOURCES = \
	AquariumController.cpp \
	Bubble.cpp \
	Environment.cpp \
	glexcept.cpp \
	Ground.cpp \
	imagereceiver.cpp \
	JPEG.cpp \
	main.cpp \
	MS3D_ASCII.cpp \
	Object.cpp \
	shaders.cpp \
	Vis.cpp \
	wiggle.cpp

noinst_HEADERS = \
	AquariumController.h \
	Bubble.h \
	Environment.h \
	glexcept.hpp \
	Ground.h \
	imagereceiver.h \
	JPEG.h \
	math-helpers.hpp \
	MS3D_ASCII.h \
	Object.h \
	shaders.hpp \
	Vis.h \
	wiggle.hpp

fishtank_LDADD = $(builddir)/GL/libGLee.a $(GLFW_LIBS) $(OPENGL_LIBS) $(JPEG_LIBS)
fishtank_CPPFLAGS = $(GLFW_CFLAGS) $(BOOST_CPPFLAGS)
