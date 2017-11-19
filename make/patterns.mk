%.bin: %.elf
	@echo "OBJCOPY $@"
	$(OBJCOPY) -O binary $< $@

%.o: %.c
	@echo "CC $@"
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.asm
	@echo "NASM $@"
	$(NASM) $< -o $@

%.a:
	@echo "AR $@"
	$(AR) $(ARFLAGS) $@ $^

%.elf: %.o
	@echo "LINK $@"
	$(LD) $(LDFLAGS) $^ -o $@ $(LDLIBS)
