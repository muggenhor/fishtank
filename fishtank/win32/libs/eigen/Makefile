PKG_NAME:=eigen
PKG_VERSION=2.0.2

PKG_SOURCE:=$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://bitbucket.org/eigen/eigen2/get
PKG_MD5SUM:=d8d0fb1e22b15fce8d90f396c2edb3ca

include $(TOPDIR)/rules.mk

eigen2: $(TOPDIR)/downloads/$(PKG_SOURCE)
	tar xjf $^

build: eigen2

clean:
	$(RM) -r eigen2
