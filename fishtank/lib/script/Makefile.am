# FIXME: Get rid of GLFW_CFLAGS ASAP
AM_CPPFLAGS = -I$(srcdir)/.. $(GLFW_CFLAGS) $(BOOST_CPPFLAGS) $(LUA_CFLAGS) $(LUABIND_CFLAGS) $(FT_CPPFLAGS)
AM_CFLAGS   = $(FT_CFLAGS)
AM_CXXFLAGS = $(FT_CXXFLAGS)

noinst_LTLIBRARIES = libscript.la

libscript_la_SOURCES = \
	debug-support.cpp \
	eigen-support.cpp \
	fish-support.cpp \
	lua-base.cpp \
	object-support.cpp \
	opengl-support.cpp \
	state.cpp \
	vertexarrays-support.cpp

noinst_HEADERS = \
	debug-support.hpp \
	eigen-support.hpp \
	fish-support.hpp \
	lua-base.hpp \
	object-support.hpp \
	opengl-support.hpp \
	state.hpp \
	vertexarrays-support.hpp

libscript_la_LIBADD = \
	$(builddir)/../framework/libframework.la
	$(LUA_LIBS) \
	$(LUABIND_LIBS)

# Force lua-base.cpp to be compiled with optimisations disabled (-O0)
lua-base.lo: lua-base.cpp
@am__fastdepCXX_TRUE@	$(LTCXXCOMPILE) -O0 -MT $@ -MD -MP -MF $(DEPDIR)/$*.Tpo -c -o $@ $<
@am__fastdepCXX_TRUE@	$(am__mv) $(DEPDIR)/$*.Tpo $(DEPDIR)/$*.Plo
@AMDEP_TRUE@@am__fastdepCXX_FALSE@	source='$<' object='$@' libtool=yes @AMDEPBACKSLASH@
@AMDEP_TRUE@@am__fastdepCXX_FALSE@	DEPDIR=$(DEPDIR) $(CXXDEPMODE) $(depcomp) @AMDEPBACKSLASH@
@am__fastdepCXX_FALSE@	$(LTCXXCOMPILE) -O0 -c -o $@ $<
