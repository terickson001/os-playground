[bits 32]
global _start
extern kernel_main

MB_MAGIC equ 0x1BADB002
MB_FLAGS equ 0b11
MB_CHECKSUM equ (0 - (MB_MAGIC + MB_FLAGS))

align 4
section .multiboot
dd MB_MAGIC
dd MB_FLAGS
dd MB_CHECKSUM

section .bss
align 16
stack_bottom:
resb 4096
stack_top:

section .text
_start:
mov esp, stack_top
call kernel_main
hang:
hlt
jmp hang