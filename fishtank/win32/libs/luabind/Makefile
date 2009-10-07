PKG_NAME:=luabind
PKG_VERSION:=0.8.1

PKG_SOURCEBASE:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE:=$(PKG_SOURCEBASE).tar.gz
PKG_SOURCE_URL:=@SF/luabind
PKG_MD5SUM:=90f684825f8944e433154a03e1e368fe

include $(TOPDIR)/rules.mk

AR=$(HOST_TRIPLET)-ar
CXX=$(HOST_TRIPLET)-g++
CXXFLAGS=-ftemplate-depth-128 -O3 -fno-inline -Wall -g3 -mthreads
CPPFLAGS=-I$(PKG_SOURCEBASE) -I$(TOPDIR)/libs/boost/stage/include -I$(TOPDIR)/libs/lua/include

DIR=$(PKG_SOURCEBASE)/bin/gcc-mingw/debug/link-static/target-os-windows/threading-multi
OBJECTS = \
	$(DIR)/class.o \
	$(DIR)/class_info.o \
	$(DIR)/class_registry.o \
	$(DIR)/class_rep.o \
	$(DIR)/create_class.o \
	$(DIR)/error.o \
	$(DIR)/exception_handler.o \
	$(DIR)/find_best_match.o \
	$(DIR)/function.o \
	$(DIR)/implicit_cast.o \
	$(DIR)/link_compatibility.o \
	$(DIR)/object_rep.o \
	$(DIR)/open.o \
	$(DIR)/overload_rep.o \
	$(DIR)/pcall.o \
	$(DIR)/ref.o \
	$(DIR)/scope.o \
	$(DIR)/stack_content_by_name.o \
	$(DIR)/weak_ref.o \
	$(DIR)/wrapper_base.o

extract-stamp: $(TOPDIR)/downloads/$(PKG_SOURCE)
	tar xzf $^
	mkdir -p $(DIR) $(PKG_SOURCEBASE)/stage/lib
	test -L $(PKG_SOURCEBASE)/stage/include || ln -s .. $(PKG_SOURCEBASE)/stage/include
	touch extract-stamp

$(DIR)/%.o: $(PKG_SOURCEBASE)/src/%.cpp extract-stamp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c -o $@ $<

$(DIR)/libluabind.a: $(OBJECTS)
	$(AR) rc $@ $(OBJECTS)

$(PKG_SOURCEBASE)/stage/lib/libluabind.a: $(DIR)/libluabind.a
	cp -p $< $@

stage:
	test -L $@ || ln -s $(PKG_SOURCEBASE)/stage $@

# We need a subinvocation of make to reevaluate the rules after directory creation
build: extract-stamp stage
	$(MAKE) -$(MAKEFLAGS) $(PKG_SOURCEBASE)/stage/lib/libluabind.a

clean:
	$(RM) -r $(PKG_SOURCEBASE) extract-stamp

.PHONY: build clean
