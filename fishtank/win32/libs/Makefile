SUBDIRS=boost eigen glfw jpeg lua luabind zlib png

all: all-recursive
build: build-recursive
clean: clean-recursive

all-recursive:
	@list='$(SUBDIRS)'; for subdir in $$list; do \
	  echo $(MAKE) -C $$subdir TOPDIR=../.. -$(MAKEFLAGS) all ; \
	  $(MAKE) -C $$subdir TOPDIR=../.. -$(MAKEFLAGS) all || exit; \
	done

build-recursive:
	@list='$(SUBDIRS)'; for subdir in $$list; do \
	  echo $(MAKE) -C $$subdir TOPDIR=../.. -$(MAKEFLAGS) build ; \
	  $(MAKE) -C $$subdir TOPDIR=../.. -$(MAKEFLAGS) build || exit; \
	done

clean-recursive:
	@list='$(SUBDIRS)'; for subdir in $$list; do \
	  echo $(MAKE) -C $$subdir TOPDIR=../.. -$(MAKEFLAGS) clean ; \
	  $(MAKE) -C $$subdir TOPDIR=../.. -$(MAKEFLAGS) clean || exit; \
	done
