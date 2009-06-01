#!/bin/sh

cd `dirname "$0"`

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

for arg in "$@"; do
	case "$arg" in
	*=*) val=`expr "X$arg" : '[^=]*=\(.*\)'` ;;
	*)   val=yes ;;
	esac

	case "$arg" in
	*=*)
		var=`expr "x$arg" : 'x\([^=]*\)='`
		# Reject names that are not valid shell variable names.
		expr "x$var" : ".*[^_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789]" >/dev/null && {
			echo "$0: error: invalid variable name: $var" >&2
			exit 1
		}
		eval $var=\$val
		export $var
		;;
	*)
		echo "$0: error: unknown command line option: $arg" >&2
		exit 1
		;;
	esac
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
execute ../../configure --host=$HOST_TRIPLET \
                        GLFW_CFLAGS="-I`pwd`/../libs/glfw/glfw.bin.WIN32/include" \
                        GLFW_LIBS="-L`pwd`/../libs/glfw/glfw.bin.WIN32/lib-mingw -lglfw" \
                        --with-boost="`pwd`/../libs/boost/boost_1_39_0/stage" \
                        CPPFLAGS="-I`pwd`/../libs/jpeg/jpeg-6b -I`pwd`/../libs/zlib -I`pwd`/../libs/eigen/eigen2 -I`pwd`/../libs/png/libpng-1.2.36 -I${DEVPKG}/OpenCV/include" \
                        LDFLAGS="-L`pwd`/../libs/jpeg/jpeg-6b -L`pwd`/../libs/zlib -L`pwd`/../libs/png/lib -L${DEVPKG}/OpenCV/bin" \
                        CFLAGS=-O3 \
                        CXXFLAGS=-O3 \
                        "$@"
execute make
execute zip -j9 fishtank-win32.zip fishtank.exe ${DEVPKG}/OpenCV/bin/cxcore100.dll ${DEVPKG}/OpenCV/bin/highgui100.dll ${DEVPKG}/OpenCV/bin/libguide40.dll
