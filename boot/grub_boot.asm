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

dd 0
dd 0
dd 0
dd 0
dd 0
dd 0

dd 0
dd 1024
dd 768
dd 32

section .bss
align 16
stack_bottom:
resb 4096
stack_top:

section .text
%include "boot/gdt.asm"

_start:
lgdt [gdt_descriptor]
jmp CODE_SEG:update_segments

update_segments:
mov dx, DATA_SEG ; update segment registers
mov ds, dx
mov ss, dx
mov es, dx
mov fs, dx
mov gs, dx

mov esp, stack_top

push 0
popf

push ebx
push eax
call kernel_main

hang:
hlt
jmp hang