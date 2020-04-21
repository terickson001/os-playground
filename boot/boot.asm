[org 0x7c00]

KERNEL_OFFSET equ 0x1000

start:
mov [BOOT_DRIVE], dl
mov bp, 0x9000              ; Set the stack safely away from us
mov sp, bp

mov si, MSG_REAL_MODE
call print                  ; This will be written after the BIOS messages
call print_nl

call load_kernel
call switch32
jmp $                       ; This should never be executed

%include "boot/print.asm"
%include "boot/print_hex.asm"
%include "boot/boot_disk.asm"
%include "boot/switch32.asm"
%include "boot/print32.asm"
%include "boot/gdt.asm"

[bits 16]
load_kernel:
mov si, MSG_LOAD_KERNEL
call print
call print_nl

mov bx, KERNEL_OFFSET
mov dh, 32                  ; Loading 32 sectors (16Kib)
mov dl, [BOOT_DRIVE]
call disk_load
ret

[bits 32]

BEGIN_PM:
mov si, MSG_PROT_MODE
call print32
call KERNEL_OFFSET
jmp $

BOOT_DRIVE db 0
MSG_REAL_MODE db "Started in 16-bit real mode",0
MSG_PROT_MODE db "Loaded 32-bit protected mode",0
MSG_LOAD_KERNEL db "Loading kernel into memory",0

times 510-($-$$) db 0       ; Pad remainder of boot sector with 0s
dw 0xAA55                   ; The standard PC boot signature
