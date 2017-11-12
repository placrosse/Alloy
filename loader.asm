; A quick and dirty loader
; Loads 2MiB from the second block on the disk


[BITS 64]
[ORG 0x00000000001E0000]

%INCLUDE "libBareMetal.asm"

start:					; Start of program label

	; Read BMFS directory into memory
	mov rax, 8
	mov rcx, 8
	xor rdx, rdx
	mov rdi, dir
	call [b_disk_read]

	; Check directory for "alloy.bin"
;	lea rsi, [rel foundit]		; Load RSI with memory address of string
;	call [b_output]			; Print the string that RSI points to

	; Load Alloy to memory
	mov rax, 512			; Start at 2MiB
	mov rcx, 512			; Load 2MiB
	xor rdx, rdx
	mov rdi, 0x200000
	call [b_disk_read]

	; Jump to Alloy
	jmp 0x200000			; Jump to file

ret					; Return to OS

foundit: db 'Found it', 13, 0
dir: