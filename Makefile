# Define ALLOY_WITH_BAREMETAL=1 for building Alloy
# to run on BareMetal OS.
#ALLOY_WITH_BAREMETAL ?=

.PHONY: all clean test install
all clean test install:
	$(MAKE) -C fonts/source-code-pro $@
	$(MAKE) -C utils $@
	$(MAKE) -C src $@

$(V).SILENT:
