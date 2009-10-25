# FIXME: Get rid of GLFW_CFLAGS ASAP
AM_CPPFLAGS = $(GLFW_CFLAGS) $(BOOST_CPPFLAGS) $(FT_CPPFLAGS)
AM_CFLAGS   = $(FT_CFLAGS)
AM_CXXFLAGS = $(FT_CXXFLAGS)

noinst_LTLIBRARIES = libframework.la

libframework_la_SOURCES = \
	debug-net-out.cpp \
	debug.cpp \
	resource.cpp \
	threadpool.cpp \
	vfs.cpp

noinst_HEADERS = \
	debug-net-out.hpp \
	debug.hpp \
	property.hpp \
	resource.hpp \
	threadpool.hpp \
	vfs.hpp

libframework_la_LIBADD = \
	$(BOOST_PROGRAM_OPTIONS_LIBS) \
	$(BOOST_SYSTEM_LIBS) \
	$(BOOST_THREAD_LIBS)
libframework_la_LDFLAGS = \
	$(BOOST_PROGRAM_OPTIONS_LDFLAGS) \
	$(BOOST_SYSTEM_LDFLAGS) \
	$(BOOST_THREAD_LDFLAGS)
