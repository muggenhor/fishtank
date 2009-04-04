#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT(fishtank, 0.1)
AM_INIT_AUTOMAKE([1.10])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([main.cpp])

# HACK
# Workaround stupid autoconf CFLAGS default
# (Bash considers set but empty variables different from null for the ${VAR:+alt} operator)
CFLAGS="${CFLAGS}"
CXXFLAGS="${CXXFLAGS}"

# Make cross compilation options available in --help
AC_CANONICAL_HOST

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC_STDC
AC_PROG_RANLIB
AC_PROG_LIBTOOL
PKG_PROG_PKG_CONFIG

# Check for operating system
case ${host_os} in
	*mingw32*)
		host_os_mingw32=yes
		WIN32_LIBS='-lwsock32'
		AC_SUBST([WIN32_LIBS])
		;;
esac

# pkg-config hooks:

AC_ARG_VAR([PKGCONFIG_PREFIX], [Prefix path to look in for pkg-config templates])
AS_IF([test -n "$PKGCONFIG_PREFIX"],[
	PKG_CONFIG="${PKG_CONFIG} --define-variable=prefix=${PKGCONFIG_PREFIX}"
])


# Commandline options - enable:

AC_ARG_ENABLE([debug],
	AS_HELP_STRING([--enable-debug], [Compile debug version [[default=yes]]]),
	[ enable_debug=${enableval} ], [ enable_debug=yes ])
AC_MSG_CHECKING([whether to compile in debug mode])
AC_MSG_RESULT([${enable_debug}])

FT_CFLAGS="${FT_CFLAGS} -Wstrict-prototypes -Wmissing-declarations"
WARN_AND_DEBUG_FLAGS="-g3 -Wall -Wextra -Wmissing-field-initializers -Wcast-align -Wwrite-strings -Wpointer-arith"
if test "x$enable_debug" = "xyes" ; then
	WARN_AND_DEBUG_FLAGS="${WARN_AND_DEBUG_FLAGS} -O0 -Werror"
	FT_CPPFLAGS="${FT_CPPFLAGS} -DDEBUG"
elif test "x$enable_debug" = "xrelaxed" ; then
	WARN_AND_DEBUG_FLAGS="${WARN_AND_DEBUG_FLAGS} -O0"
	FT_CPPFLAGS="${FT_CPPFLAGS} -DDEBUG"
elif test "x$enable_debug" = "xdebugprofile" ; then
	WARN_AND_DEBUG_FLAGS="${WARN_AND_DEBUG_FLAGS} -O0 -pg"
	FT_CPPFLAGS="${FT_CPPFLAGS} -DDEBUG"
elif test "x$enable_debug" = "xprofile" ; then
	WARN_AND_DEBUG_FLAGS="${WARN_AND_DEBUG_FLAGS} -Os -pg"
	FT_CPPFLAGS="${FT_CPPFLAGS} -DNDEBUG"
else
	WARN_AND_DEBUG_FLAGS="${WARN_AND_DEBUG_FLAGS}"
	FT_CPPFLAGS="${FT_CPPFLAGS} -DNDEBUG"
fi

FT_CFLAGS="${FT_CFLAGS} ${WARN_AND_DEBUG_FLAGS}"
FT_CXXFLAGS="${FT_CXXFLAGS} ${WARN_AND_DEBUG_FLAGS}"

AS_IF([test "x$host_os_mingw32" != "xyes"],[
	FT_CPPFLAGS="${FT_CPPFLAGS} -DDATADIR=\"\\\"\${pkgdatadir}\\\"\" -DSYSCONFDIR=\"\\\"\${sysconfdir}\\\"\""
])

# Checks for libraries.
BOOST_REQUIRE([1.36])
BOOST_BIND
BOOST_CONVERSION
BOOST_FOREACH
BOOST_FUNCTION
BOOST_PROGRAM_OPTIONS
BOOST_REGEX
BOOST_SMART_PTR
BOOST_UTILITY
PKG_CHECK_MODULES([GLFW], [libglfw])

# Look for OpenGL
AC_CHECK_LIB(GL, main,
	OPENGL_LIBS="-lGL",
	AC_CHECK_LIB(opengl32, main,
		OPENGL_LIBS="-lopengl32",
		AC_MSG_ERROR([OpenGL library not found.])))
AC_CHECK_LIB(GLU, main,
	OPENGL_LIBS="${OPENGL_LIBS} -lGLU",
	AC_CHECK_LIB(glu32, main,
		OPENGL_LIBS="${OPENGL_LIBS} -lglu32",
		AC_MSG_ERROR([GLU library not found.])))
AC_SUBST([OPENGL_LIBS])

# Look for libjpeg
AC_CHECK_HEADER([jpeglib.h], ,
	AC_MSG_ERROR([JPEG headers not found]))
AC_CHECK_LIB([jpeg], [jpeg_start_decompress],
	AC_SUBST([JPEG_LIBS], [-ljpeg]),
	AC_MSG_ERROR([JPEG not found]),
	[${WIN32_LIBS}])

# Checks for header files.
AC_HEADER_STDC

AC_SUBST([FT_CFLAGS])
AC_SUBST([FT_CXXFLAGS])
AC_SUBST([FT_CPPFLAGS])

AC_CONFIG_HEADER([config.h])
AC_CONFIG_FILES([Makefile
                 Data/Makefile
                 Data/Objecten/Makefile
                 Data/Vissen/Makefile
                 GL/Makefile
                 Settings/Makefile
                 vertexarrays/Makefile])
AC_OUTPUT