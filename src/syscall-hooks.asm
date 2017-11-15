; asmsyntax=nasm

SECTION .text
USE64

extern alloy_output

global b_output_hook

b_output_hook:
	push rdi	; preserve rdi
	push rsi	; preserve rsi
	mov rdi, rsi	; pass message address to alloy_output
	mov rsi, rcx	; pass message length to alloy_output
	call alloy_output
	pop rsi		; restore rsi
	pop rdi		; restore rdi
	ret
