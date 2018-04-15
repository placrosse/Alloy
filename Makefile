-include config.mk
TOP = $(CURDIR)
CONFIG ?= default
include $(TOP)/make/$(CONFIG)-config.mk
include $(TOP)/make/patterns.mk

.PHONY: all clean
all clean:
	$(MAKE) -C utils $@
	$(MAKE) -C fonts/source-code-pro $@
	$(MAKE) -C lib $@
	$(MAKE) -C src $@

.PHONY: test
test:
	$(MAKE) -C lib $@

.PHONY: install
install:
	$(MAKE) -C utils
	$(MAKE) -C fonts/source-code-pro
	$(MAKE) -C lib $@
	$(MAKE) -C src $@

$(V).SILENT:
