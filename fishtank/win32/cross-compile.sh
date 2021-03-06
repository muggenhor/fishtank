#!/bin/sh

cd `dirname "$0"`

execute()
{
	echo "$@"
	"$@"
	r=$?
	if test $r -ne 0 ; then
		exit $r
	fi
}

DEVPKG=${HOME}/svn/fishtank/devpkg

: ${HOST_TRIPLET:=i686-mingw32}

for arg in "$@"; do
	case "$arg" in
	*=*) val=`expr "X$arg" : '[^=]*=\(.*\)'` ;;
	*)   val=yes ;;
	esac

	case "$arg" in
	-with-* | --with-* | -enable-* | --enable-*)
		;;
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

# Make sure we've got a build directory
if [ ! -d build ] ; then
	mkdir build
fi

# Make sure that the dependencies have been built
execute make -C libs HOST_TRIPLET=$HOST_TRIPLET

cd build

execute ../../autogen.sh
execute ../../configure --host=$HOST_TRIPLET --enable-static --disable-shared --enable-debug=relaxed \
                        GLFW_CFLAGS="-I`pwd`/../libs/glfw/glfw.bin.WIN32/include" \
                        GLFW_LIBS="-L`pwd`/../libs/glfw/glfw.bin.WIN32/lib-mingw -lglfw" \
                        LUA_CFLAGS="-I`pwd`/../libs/lua/include" \
                        LUA_LIBS="-L`pwd`/../libs/lua/lib -llua5.1" \
                        LUABIND_CFLAGS="-I`pwd`/../libs/luabind/stage/include" \
                        LUABIND_LIBS="-L`pwd`/../libs/luabind/stage/lib -lluabind" \
                        --with-boost="`pwd`/../libs/boost/stage" \
                        CPPFLAGS="-I`pwd`/../libs/jpeg/jpeg-6b -I`pwd`/../libs/zlib -I`pwd`/../libs/eigen/eigen2 -I`pwd`/../libs/png/libpng-1.2.36 -I${DEVPKG}/OpenCV/include -D_WIN32_WINNT=0x0501" \
                        LDFLAGS="-L`pwd`/../libs/jpeg/jpeg-6b -L`pwd`/../libs/zlib -L`pwd`/../libs/png/lib -L${DEVPKG}/OpenCV/bin" \
                        CFLAGS=-O3 \
                        CXXFLAGS=-O3 \
                        "$@"
execute make
execute zip -j9 fishtank-win32.zip fishtank.exe ${DEVPKG}/OpenCV/bin/cxcore100.dll ${DEVPKG}/OpenCV/bin/highgui100.dll ${DEVPKG}/OpenCV/bin/libguide40.dll
