%.bin: %.elf
	@echo "OBJCOPY $@"
	$(OBJCOPY) -O binary $< $@

%.o: %.c
	@echo "CC      $@"
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.asm
	@echo "NASM    $@"
	$(NASM) $< -f elf64 -o $@

%.a:
	@echo "AR      $@"
	$(AR) $(ARFLAGS) $@ $^

%: %.o
	@echo "LD      $@"
	$(LD) $(LDFLAGS) $^ -o $@ $(LDLIBS)

%.elf: %.o
	@echo "LD      $@"
	$(LD) $(LDFLAGS) $^ -o $@ $(LDLIBS)
