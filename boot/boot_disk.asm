    ;; INPUTS:
    ;;  dh: Number of sectors to load
    ;;  dl: drive to load from
    ;; Outputs:
    ;;  ES:BX: Pointer the the buffer loaded with data

disk_load:
    pusha

    push dx                     ; store dx, we'll be overwriting it

    mov ah, 0x02                ; 0x02 = 'read'
    mov al, dh                  ; number of sectors to read [0x01 - 0x80]
    
    mov cl, 0x02                ; sector [0x01 - 0x11]
    mov ch, 0x00                ; cylinder [0x0 - 0x3FF], upper 2 bits in 'cl'
    mov dh, 0x00                ; head number [0x0 - 0xF]

    int 13h
    jc disk_error               ; carry bit set on disk error

    pop dx                      ; retrieve input 'dx'
    cmp al, dh                  ; BIOS sets 'al' to the number of sectors read
    jne sectors_error
    popa
    ret

disk_error:
    mov bx, DISK_ERROR
    call print
    call print_nl
    mov dh, ah                  ; ah = error code, dl = disk drive
    call print_hex
    jmp disk_loop

sectors_error:
    mov bx, SECTORS_ERROR
    call print

disk_loop:
    jmp $

DISK_ERROR: db "Disk read error",0
SECTORS_ERROR:  db "Incorrect number of sectors read",0
