PKG_NAME:=zlib
PKG_VERSION=1.2.3

PKG_SOURCEBASE:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE:=$(PKG_SOURCEBASE).tar.gz
PKG_SOURCE_URL:=@SF/libpng
PKG_MD5SUM:=debc62758716a169df9f62e6ab2bc634

include $(TOPDIR)/rules.mk

extract-stamp: $(TOPDIR)/downloads/$(PKG_SOURCE)
	tar xzf $^
	rm $(PKG_SOURCEBASE)/Makefile
	touch extract-stamp

$(PKG_SOURCEBASE)/configure: extract-stamp

$(PKG_SOURCEBASE)/Makefile: $(PKG_SOURCEBASE)/configure
	cd $(PKG_SOURCEBASE) && CC=$(HOST_TRIPLET)-gcc RANLIB=$(HOST_TRIPLET)-ranlib AR='$(HOST_TRIPLET)-ar rc' ./configure

$(PKG_SOURCEBASE)/libz.a: extract-stamp $(PKG_SOURCEBASE)/Makefile
	$(MAKE) -C $(PKG_SOURCEBASE) libz.a

libz.a: $(PKG_SOURCEBASE)/libz.a
	ln -sf $^ $@

zconf.h: $(PKG_SOURCEBASE)/zconf.h
	ln -sf $^ $@

zlib.h: $(PKG_SOURCEBASE)/zlib.h zconf.h
	ln -sf $< $@

build: libz.a zlib.h

clean:
	$(RM) -rf $(PKG_SOURCEBASE) extract-stamp
