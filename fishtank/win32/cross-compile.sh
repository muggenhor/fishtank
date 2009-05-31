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

HOST_TRIPLET=i686-mingw32

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
execute ../../configure --host=$HOST_TRIPLET \
                        GLFW_CFLAGS="-I`pwd`/../libs/glfw/glfw.bin.WIN32/include" \
                        GLFW_LIBS="-L`pwd`/../libs/glfw/glfw.bin.WIN32/lib-mingw -lglfw" \
                        --with-boost="`pwd`/../libs/boost/boost_1_39_0/stage" \
                        CPPFLAGS="-I`pwd`/../libs/jpeg/jpeg-6b -I`pwd`/../libs/eigen/eigen2 -I${DEVPKG}/OpenCV/include" \
                        LDFLAGS="-L`pwd`/../libs/jpeg/jpeg-6b -L${DEVPKG}/OpenCV/bin" \
                        CFLAGS=-O3 \
                        CXXFLAGS=-O3 \
                        "$@"
execute make
execute zip -j9 fishtank-win32.zip fishtank.exe ${DEVPKG}/OpenCV/bin/cxcore100.dll ${DEVPKG}/OpenCV/bin/highgui100.dll ${DEVPKG}/OpenCV/bin/libguide40.dll
