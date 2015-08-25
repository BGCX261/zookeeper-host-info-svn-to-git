PROJ=ZK_Service
SRC=$(PROJ).tar.gz

PKG = $(wildcard *.spec)

TEMP_DIR = /tmp/$(shell whoami)
ROOT_DIR = $(shell PWD=$$(pwd); echo $${PWD%%/$(PROJ)*}/$(PROJ))

DIST=.el5

all: prepare
	export builddir=$(shell pwd)/build; \
	for pkg in $(PKG); do \
		pkg=`basename $${pkg} .spec`; \
		echo "Building $${pkg}"; \
		arch=`rpm -q --specfile --qf "%{arch}\n" $${pkg}.spec | head -1`; \
		rpmbuild --define 'dist $(DIST)' --define="_topdir $$builddir" -ba $${pkg}.spec && \
			mv build/RPMS/$${arch}/$${pkg}*.rpm ./ && \
			continue; \
		echo "Build failed for $${pkg}"; break; \
	done

# Make rpm building base and source tarball
prepare: clean-build
	@echo "Creating source package to build/SOURCES/$(SRC)"
	@mkdir -p build/{BUILD,RPMS,SRPMS,SPECS,SOURCES}
	@mkdir -p $(TEMP_DIR)
	@rm -f $(TEMP_DIR)/$(PROJ)
	@ln -s $(ROOT_DIR) $(TEMP_DIR)/$(PROJ)
	@tar zchf $(TEMP_DIR)/$(SRC) \
					--exclude .git \
					--exclude .svn \
					--exclude pkg \
				-C $(TEMP_DIR) $(PROJ)
	@mv $(TEMP_DIR)/$(SRC) build/SOURCES/

clean-build:
	rm -rf build
clean-rpm:
	rm -f *.rpm
clean: clean-build clean-rpm

.PHONY: pkg prepare clean clean-build clean-rpm

