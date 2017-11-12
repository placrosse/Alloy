source_code_pro_url ?= https://github.com/adobe-fonts/source-code-pro/archive/2.030R-ro/1.050R-it.tar.gz
source_code_pro_tar ?= source-code-pro-2.030R-ro-1.050R-it.tar.gz
source_code_pro_dir ?= source-code-pro-2.030R-ro-1.050R-it

.PHONY: all
all: $(source_code_pro_dir)

$(source_code_pro_dir): $(source_code_pro_tar)
	tar --extract --file $<
	touch $@

$(source_code_pro_tar):
	wget --quiet $(source_code_pro_url) -O $@

.PHONY: clean
clean:
	$(RM) $(source_code_pro_tar)
	$(RM) -R $(source_code_pro_dir)

.PHONY: test
test:

.PHONY: install
install:

$(V).SILENT:
