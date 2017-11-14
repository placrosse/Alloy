; A quick and dirty loader


[BITS 64]
[ORG 0x00000000001E0000]

%INCLUDE "libBareMetal.asm"

start:					; Start of program label

	; Read BMFS directory into memory
	mov rax, 1
	mov rcx, 1
	xor rdx, rdx
	mov rdi, dir
	call [b_disk_read]

	; Check directory for "alloy.bin"
	mov rsi, dir
checknext:
	mov al, byte [rsi]
	cmp al, 0
	je error
	mov rdi, alloyname
	call string_compare
	jc loadfile
	add rsi, 64			; next record
	jmp checknext

error:
	lea rsi, [rel notfound]		; Load error message
	mov rcx, 33
	call [b_output]			; Output the error
	jmp $				; Halt

loadfile:
	; Load Alloy to memory
	add rdi, 32
	mov rax, [rdi]			; Load the starting block #
	shl rax, 9			; Quick multiply by 512
	mov rax, 512			; Start at 2MiB
	mov rcx, 512			; Load 2MiB
	xor rdx, rdx
	mov rdi, 0x200000
	call [b_disk_read]

	; Jump to Alloy
	jmp 0x200000			; Jump to file

ret					; Return to OS

; -----------------------------------------------------------------------------
; string_compare -- See if two strings match
;  IN:	RSI = string one
;	RDI = string two
; OUT:	Carry flag set if same
string_compare:
	push rsi
	push rdi
	push rbx
	push rax

string_compare_more:
	mov al, [rsi]			; Store string contents
	mov bl, [rdi]
	test al, al			; End of first string?
	jz string_compare_terminated
	cmp al, bl
	jne string_compare_not_same
	inc rsi
	inc rdi
	jmp string_compare_more

string_compare_not_same:
	pop rax
	pop rbx
	pop rdi
	pop rsi
	clc
	ret

string_compare_terminated:
	test bl, bl			; End of second string?
	jnz string_compare_not_same

	pop rax
	pop rbx
	pop rdi
	pop rsi
	stc
	ret
; -----------------------------------------------------------------------------


alloyname: db 'alloy.bin', 0
notfound: db 'Binary not found. System halted.', 13, 0

align 16
dir:
