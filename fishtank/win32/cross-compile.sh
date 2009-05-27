#!/bin/sh

function execute()
{
	echo "$@"
	"$@"
	r=$?
	if test $r -ne 0 ; then
		exit $r
	fi
}

DEVPKG=${HOME}/svn/fishtank/devpkg
BOOSTDIR="\${top_builddir}/../libs/boost_1_39_0"
BOOSTLIBDIR=${BOOSTDIR}/include/bin.v2/libs

HOST_TRIPLET=i586-mingw32msvc

COMPILER=gcc-mingw-4.2.1
BUILD_TYPE=release
LINK=link-static
THREADING=threading-multi

BOOST_LIBS="program_options regex thread"

BOOST_LDFLAGS=""
for lib in $BOOST_LIBS; do
	BOOST_LDFLAGS="$BOOST_LDFLAGS -L${BOOSTLIBDIR}/${lib}/build/${COMPILER}/${BUILD_TYPE}/${LINK}/target-os-windows/threadapi-win32/${THREADING}"
done

# Make sure we've got build and download directories for our dependencies
if [ ! -d build ] ; then
	mkdir build
fi
if [ ! -d downloads ] ; then
	mkdir downloads
fi

# Make sure that the dependencies have been built
execute make -C libs HOST_TRIPLET=$HOST_TRIPLET

cd build

execute ../../autogen.sh
execute ../../configure --host=$HOST_TRIPLET GLFW_CFLAGS="-I\${top_builddir}/../libs/glfw/glfw.bin.WIN32/include" GLFW_LIBS="-L\${top_builddir}/../libs/glfw/glfw.bin.WIN32/lib-mingw -lglfw" --with-boost="${BOOSTDIR}" CPPFLAGS="-I\${top_builddir}/../libs/jpeg/jpeg-6b -I/usr/include/eigen2 -I${DEVPKG}/OpenCV/include" LDFLAGS="-L\${top_builddir}/../libs/jpeg/jpeg-6b ${BOOST_LDFLAGS} -L${DEVPKG}/OpenCV/bin" CFLAGS=-O3 CXXFLAGS=-O3 "$@"
execute make
execute zip -j9 fishtank-win32.zip fishtank.exe ${DEVPKG}/OpenCV/bin/cxcore100.dll ${DEVPKG}/OpenCV/bin/highgui100.dll ${DEVPKG}/OpenCV/bin/libguide40.dll
