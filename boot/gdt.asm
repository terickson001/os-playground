
gdt_start:
;; The GDT must start with a null QWORD
dd 0x0
dd 0x0

;; GDT for code segment. base = 0x00000000, length - 0xffff
gdt_code:
dw 0xffff                   ; segment length, 0-15
dw 0x0000                   ; segment base, 0-15
db 0x00                     ; segment base, 16-23
db 10011010b                ; flags (8 bits)
db 11001111b                ; flags (4 bits) + segment length, 16-19
db 0x00                     ; segment base, 24-31

;; GDT for data segment. base and length identical to code segment
gdt_data:
dw 0xffff
dw 0x0000
db 0x00
db 10010010b
db 11001111b
db 0x00

gdt_end:

;; GDT descriptor
gdt_descriptor:
dw gdt_end - gdt_start - 1  ; size (16 bit), always one less of its true size
dd gdt_start                ; address (32 bit)

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start
