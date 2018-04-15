%.bin: %
	@echo "OBJCOPY $@"
	$(OBJCOPY) -O binary $< $@

%.o: %.c
	@echo "CC      $@"
	$(CC) $(CFLAGS) -c $< -o $@

%.a:
	@echo "AR      $@"
	$(AR) $(ARFLAGS) $@ $^

%: %.o
	@echo "LD      $@"
	$(LD) $(LDFLAGS) $^ -o $@ $(LDLIBS)
