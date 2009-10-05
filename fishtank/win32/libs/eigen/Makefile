PKG_NAME:=eigen
PKG_VERSION=2.0.6

PKG_SOURCE:=$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://bitbucket.org/eigen/eigen2/get
PKG_MD5SUM:=0673563a5e7f74ae4ee130553458b81e

include $(TOPDIR)/rules.mk

eigen2: $(TOPDIR)/downloads/$(PKG_SOURCE)
	tar xjf $^

build: eigen2

clean:
	$(RM) -r eigen2
