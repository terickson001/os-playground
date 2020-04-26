
gdt_start:
;; The GDT must start with a null QWORD
dd 0x0
dd 0x0

;; GDT for kernel code segment. base = 0x00000000, length - 0xfffff
gdt_kcode:
dw 0xffff                   ; segment length, 0-15
dw 0x0000                   ; segment base, 0-15
db 0x00                     ; segment base, 16-23
db 1_00_1_1010b             ; [Present|Ring|DescType|SegType]
db 1_1_0_0_1111b            ; [Granularity|OpSize|0|Available|SegLengthHigh]
db 0x00                     ; segment base, 24-31

;; GDT for kernel data segment. base and length identical to code segment
gdt_kdata:
dw 0xffff                   ; segment length, 0-15
dw 0x0000                   ; segment base, 0-15
db 0x00                     ; segment base, 16-23
db 1_00_1_0010b             ; [Present|Ring|DescType|SegType]
db 1_1_0_0_1111b            ; [Granularity|OpSize|0|Available|SegLengthHigh]
db 0x00                     ; segment base, 24-31

;; GDT for user code segment. base = 0x00000000, length - 0xfffff
gdt_ucode:
dw 0xffff                   ; segment length, 0-15
dw 0x0000                   ; segment base, 0-15
db 0x00                     ; segment base, 16-23
db 1_11_1_1010b             ; [Present|Ring|DescType|SegType]
db 1_1_0_0_1111b            ; [Granularity|OpSize|0|Available|SegLengthHigh]
db 0x00                     ; segment base, 24-31

;; GDT for user data segment. base and length identical to code segment
gdt_udata:
dw 0xffff                   ; segment length, 0-15
dw 0x0000                   ; segment base, 0-15
db 0x00                     ; segment base, 16-23
db 1_11_1_0010b             ; [Present|Ring|DescType|SegType]
db 1_1_0_0_1111b            ; [Granularity|OpSize|0|Available|SegLengthHigh]
db 0x00                     ; segment base, 24-31

gdt_end:

;; GDT descriptor
gdt_descriptor:
dw gdt_end - gdt_start - 1  ; size (16 bit), always one less of its true size
dd gdt_start                ; address (32 bit)

CODE_SEG equ gdt_kcode - gdt_start
DATA_SEG equ gdt_kdata - gdt_start
UCODE_SEG equ gdt_ucode - gdt_start
UDATA_SEG equ gdt_udata - gdt_start
