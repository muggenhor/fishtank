PKG_NAME:=glfw
PKG_VERSION=2.6

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).bin.WIN32.zip
PKG_SOURCE_URL:=@SF/glfw
PKG_MD5SUM:=f1b965bc8e438c7ac59d8e04913ded8b

include $(TOPDIR)/rules.mk

glfw.bin.WIN32: $(TOPDIR)/downloads/$(PKG_SOURCE)
	unzip -n $^

build: glfw.bin.WIN32

clean:
	$(RM) -r glfw.bin.WIN32
