    ;; INPUTS
    ;;  dx: The bytes to print
    
print_hex:
    pusha

    mov cx, 0                   ; index

hex_loop:
    cmp cx, 4                   ; loop 4 times
    je end

    ;; convert last char to ascii
    mov ax, dx                  ; ax is the working register
    and ax, 0x000F              ; mask the last 4 bytes
    add al, 0x30                ; add 0x30 to convert to ASCII
    cmp al, 0x39                ; if > 9, add 8 to convert to `A-F`
    jle store
    add al, 7

store:
    ;; Get the position to store in string
    mov bx, HEX_OUT + 5         ; end of string
    sub bx, cx                  ; cx bytes from end
    mov [bx], al                ; Copy ASCII representation to string
    ror dx, 4                   ; rotate input right by 4 bytes

    add cx, 1
    jmp hex_loop

end:
    mov si, HEX_OUT
    call print
    
    popa
    ret

HEX_OUT:
    db '0x0000',0           ; reserved memory for output
    
