# Define ALLOY_WITH_BAREMETAL=1 for building Alloy
# to run on BareMetal OS.
#ALLOY_WITH_BAREMETAL ?=

.PHONY: all clean
all clean:
	$(MAKE) -C fonts/source-code-pro $@
	$(MAKE) -C utils $@
	$(MAKE) -C src $@

.PHONY: test
test:

.PHONY: install
install:
	$(MAKE) -C fonts/source-code-pro
	$(MAKE) -C utils
	$(MAKE) -C src $@

$(V).SILENT:
