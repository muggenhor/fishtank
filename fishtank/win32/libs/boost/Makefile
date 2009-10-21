PKG_NAME:=boost
PKG_MAJOR_VERSION:=1
PKG_MINOR_VERSION:=39
PKG_SUBMINOR_VERSION:=0
PKG_VERSION:=$(PKG_MAJOR_VERSION).$(PKG_MINOR_VERSION).$(PKG_SUBMINOR_VERSION)

PKG_SOURCEBASE:=$(PKG_NAME)_$(PKG_MAJOR_VERSION)_$(PKG_MINOR_VERSION)_$(PKG_SUBMINOR_VERSION)
PKG_SOURCE:=$(PKG_SOURCEBASE).tar.bz2
PKG_SOURCE_URL:=@SF/boost
PKG_MD5SUM:=a17281fd88c48e0d866e1a12deecbcc0

include $(TOPDIR)/rules.mk

$(PKG_SOURCEBASE) extract-stamp: $(TOPDIR)/downloads/$(PKG_SOURCE)
	tar xjf $^
	@rm -f patch-stamp build-bjam-stamp
	touch extract-stamp

patch-stamp: extract-stamp
	@for patch in patches/*.diff; do \
	    echo sed -e "s/\$$sourcebase\b/$(PKG_SOURCEBASE)/g" -e "s/\$$host_triplet\b/$(HOST_TRIPLET)/g" "$$patch" \| patch -p0 ; \
	    sed -e "s/\$$sourcebase\b/$(PKG_SOURCEBASE)/g" -e "s/\$$host_triplet\b/$(HOST_TRIPLET)/g" "$$patch" | patch -p0 || exit ; \
	done
	touch patch-stamp

build-bjam-stamp: extract-stamp
	cd $(PKG_SOURCEBASE)/tools/jam/src && ./build.sh gcc
	touch build-bjam-stamp

build-stamp: extract-stamp patch-stamp build-bjam-stamp
	cd $(PKG_SOURCEBASE) && tools/jam/src/bin.*/bjam -d2 --toolset=gcc --variant=release threading=multi link=static --prefix=./stage --layout=system --with-filesystem --with-program_options --with-regex --with-system --with-thread stage
	test -L "$(PKG_SOURCEBASE)/stage/include" || ln -s .. "$(PKG_SOURCEBASE)/stage/include"
	@for lib in $(PKG_SOURCEBASE)/stage/lib/*_win32*.lib ; do \
	    suffix=`echo $$lib | sed "s/^.*\(_win32.*\.lib\)$$/\1/"` ; \
	    echo ln -s "`basename "$$lib"`" "`dirname "$$lib"`/`basename "$$lib" "$$suffix"`.lib" ; \
	    test -L "`dirname "$$lib"`/`basename "$$lib" "$$suffix"`.lib" || ln -s "`basename "$$lib"`" "`dirname "$$lib"`/`basename "$$lib" "$$suffix"`.lib" || exit ; \
	done
	@for lib in $(PKG_SOURCEBASE)/stage/lib/*.lib ; do \
	    echo ln -s "`basename "$$lib"`" "`dirname "$$lib"`/`basename "$$lib" .lib`.a" ; \
	    test -L "`dirname "$$lib"`/`basename "$$lib" .lib`.a" || ln -s "`basename "$$lib"`" "`dirname "$$lib"`/`basename "$$lib" .lib`.a" || exit ; \
	    echo ln -s "`basename "$$lib"`" "`dirname "$$lib"`/`basename "$$lib" .lib`-mt.a" ; \
	    test -L "`dirname "$$lib"`/`basename "$$lib" .lib`-mt.a" || ln -s "`basename "$$lib"`" "`dirname "$$lib"`/`basename "$$lib" .lib`-mt.a" || exit ; \
	done
	touch build-stamp

build: build-stamp
	test -L stage || ln -s $(PKG_SOURCEBASE)/stage stage

clean:
	$(RM) -r $(PKG_SOURCEBASE) extract-stamp patch-stamp build-stamp build-bjam-stamp
