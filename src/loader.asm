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

	jmp loadfile

	lea rsi, [rel notfound]		; Load error message
	call [b_output]			; Output the error
	jmp $				; Halt

loadfile:
	; Load Alloy to memory
	mov rax, 512			; Start at 2MiB
	mov rcx, 512			; Load 2MiB
	xor rdx, rdx
	mov rdi, 0x200000
	call [b_disk_read]

	; Jump to Alloy
	jmp 0x200000			; Jump to file

ret					; Return to OS

notfound: db 'Binary not found. System halted.', 13, 0
dir: