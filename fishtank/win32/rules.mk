all: build

$(TOPDIR)/downloads/$(PKG_SOURCE):
	$(TOPDIR)/../scripts/download.pl $(TOPDIR)/downloads "$(PKG_SOURCE)" "$(PKG_MD5SUM)" $(PKG_SOURCE_URL)

.PHONY: all build clean
