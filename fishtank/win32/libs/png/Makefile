PKG_NAME:=libpng
PKG_VERSION=1.2.36

PKG_SOURCEBASE:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE:=$(PKG_SOURCEBASE).tar.bz2
PKG_SOURCE_URL:=@SF/libpng
PKG_MD5SUM:=325d67a8dd4a1f672d395c339fddd75c

include $(TOPDIR)/rules.mk

extract-stamp: $(TOPDIR)/downloads/$(PKG_SOURCE)
	tar xjf $^
	touch extract-stamp

$(PKG_SOURCEBASE)/configure: extract-stamp

$(PKG_SOURCEBASE)/config.status: $(PKG_SOURCEBASE)/configure
	cd $(PKG_SOURCEBASE) && ./configure --host=$(HOST_TRIPLET) --prefix="`pwd`/.." --disable-shared --enable-static CPPFLAGS=-I`pwd`/../../zlib LDFLAGS=-L`pwd`/../../zlib

$(PKG_SOURCEBASE)/libpng.la: extract-stamp $(PKG_SOURCEBASE)/config.status
	$(MAKE) -C $(PKG_SOURCEBASE) libpng.la

lib/libpng.a: $(PKG_SOURCEBASE)/libpng.la
	$(MAKE) -C $(PKG_SOURCEBASE) install-exec

build: lib/libpng.a

clean:
	$(RM) -rf $(PKG_SOURCEBASE) extract-stamp
