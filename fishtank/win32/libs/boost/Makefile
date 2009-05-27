PKG_NAME:=boost
PKG_VERSION=1.39.0

PKG_SOURCE:=$(PKG_NAME)_1_39_0.tar.bz2
PKG_SOURCE_URL:=@SF/boost
PKG_MD5SUM:=a17281fd88c48e0d866e1a12deecbcc0

include $(TOPDIR)/rules.mk

boost_1_39_0: $(TOPDIR)/downloads/$(PKG_SOURCE)
	if [ ! -d $@ ] ; then \
	    tar xjf $^ || exit ; \
	    mv boost_1_39_0 include || exit ; \
	    mkdir boost_1_39_0 || exit ; \
	    mv include boost_1_39_0/ || exit ; \
	fi

patch-stamp: boost_1_39_0
	patch -p0 < patches/001-use-mingw-cross-compiler.diff
	touch patch-stamp

build-bjam-stamp:
	cd boost_1_39_0/include/tools/jam/src && ./build.sh gcc
	touch build-bjam-stamp

build-stamp: boost_1_39_0 patch-stamp build-bjam-stamp
	cd boost_1_39_0/include && tools/jam/src/bin.*/bjam -d2 --toolset=gcc --variant=release threading=single link=static --prefix= --layout=system --with-program_options --with-regex --with-thread
	touch build-stamp

build: build-stamp

clean:
	$(RM) -rf boost_1_39_0 patch-stamp build-stamp build-bjam-stamp
