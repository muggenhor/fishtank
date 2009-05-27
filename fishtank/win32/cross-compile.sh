#!/bin/sh

function execute()
{
	echo "$@"
	"$@"
}

DEVPKG=${HOME}/svn/fishtank/devpkg
BOOSTDIR=${DEVPKG}/boost_1_38_0
BOOSTLIBDIR=${BOOSTDIR}/include/bin.v2/libs

COMPILER=gcc-mingw-4.2.1
BUILD_TYPE=release
LINK=link-static
THREADING=threading-multi

BOOST_LIBS="program_options regex thread"

BOOST_LDFLAGS=""
for lib in $BOOST_LIBS; do
	BOOST_LDFLAGS="$BOOST_LDFLAGS -L${BOOSTLIBDIR}/${lib}/build/${COMPILER}/${BUILD_TYPE}/${LINK}/target-os-windows/threadapi-win32/${THREADING}"
done

if [ ! -d build ] ; then
	mkdir build
fi

cd build

execute ../../autogen.sh || exit
execute ../../configure --host=i586-mingw32msvc GLFW_CFLAGS=-I${DEVPKG}/glfw.bin.WIN32/include GLFW_LIBS="-L${DEVPKG}/glfw.bin.WIN32/lib-mingw -lglfw" --with-boost="${BOOSTDIR}" CPPFLAGS="-I${DEVPKG}/jpeg-6b -I/usr/include/eigen2 -I${DEVPKG}/OpenCV/include" LDFLAGS="-L${DEVPKG}/jpeg-6b ${BOOST_LDFLAGS} -L${DEVPKG}/OpenCV/bin" CFLAGS=-O3 CXXFLAGS=-O3 "$@" || exit
execute make
execute zip -j9 fishtank-win32.zip fishtank.exe ${DEVPKG}/OpenCV/bin/cxcore100.dll ${DEVPKG}/OpenCV/bin/highgui100.dll ${DEVPKG}/OpenCV/bin/libguide40.dll
