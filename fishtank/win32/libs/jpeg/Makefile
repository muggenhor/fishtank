PKG_NAME:=jpeg
PKG_VERSION=6b

PKG_SOURCE:=$(PKG_NAME)src.v$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:= \
	http://www.ijg.org/files/ \
	ftp://ftp.uu.net/graphics/jpeg/
PKG_MD5SUM:=dbd5f3b47ed13132f04c685d608a7547

include $(TOPDIR)/rules.mk

jpeg-6b: $(TOPDIR)/downloads/$(PKG_SOURCE)
	tar xzf $^

jpeg-6b/configure: jpeg-6b

jpeg-6b/config.status: jpeg-6b/configure
	cd jpeg-6b && ./configure --host=$(HOST_TRIPLET) CC=$(HOST_TRIPLET)-gcc CXX=$(HOST_TRIPLET)-g++ RANLIB=$(HOST_TRIPLET)-ranlib

jpeg-6b/libjpeg.a: jpeg-6b jpeg-6b/config.status
	$(MAKE) -C jpeg-6b AR='$(HOST_TRIPLET)-ar rc' libjpeg.a

build: jpeg-6b/libjpeg.a

clean:
	$(RM) -rf jpeg-6b
