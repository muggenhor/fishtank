PKG_NAME:=lua
PKG_VERSION:=5.1.4

PKG_SOURCEBASE:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE:=$(PKG_SOURCEBASE).tar.gz
PKG_SOURCE_URL:=http://www.lua.org/ftp
PKG_MD5SUM:=d0870f2de55d59c1c8419f36e8fac150

include $(TOPDIR)/rules.mk

extract-stamp: $(TOPDIR)/downloads/$(PKG_SOURCE)
	tar xzf $^
	touch extract-stamp

patch-stamp: extract-stamp
	@for patch in patches/*.diff; do \
	    echo sed -e "s/\$$sourcebase\b/$(PKG_SOURCEBASE)/g" -e "s/\$$host_triplet\b/$(HOST_TRIPLET)/g" "$$patch" \| patch -p0 ; \
	    sed -e "s/\$$sourcebase\b/$(PKG_SOURCEBASE)/g" -e "s/\$$host_triplet\b/$(HOST_TRIPLET)/g" "$$patch" | patch -p0 || exit ; \
	done
	touch patch-stamp

build-stamp: extract-stamp patch-stamp
	$(MAKE) -C $(PKG_SOURCEBASE)/src \
		"CC=$(HOST_TRIPLET)-g++" \
		"AR=$(HOST_TRIPLET)-ar rcu" \
		"RANLIB=$(HOST_TRIPLET)-ranlib" \
		"MYCFLAGS=-g3" \
		LUA_A=liblua5.1.a liblua5.1.a
	touch build-stamp

lib include:
	ln -sf $(PKG_SOURCEBASE)/src $@

build: build-stamp lib include

clean:
	$(RM) -r $(PKG_SOURCEBASE) build-stamp extract-stamp patch-stamp lib include
