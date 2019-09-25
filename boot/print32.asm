    [bits 32]

    VIDEO_MEMORY equ 0xb8000    ; The location of video memory
    WHITE_ON_BLACK equ 0x0f     ; The color byte for each character

    
print32:                        ; Routine: output string into video memory
    pusha
    mov edx, VIDEO_MEMORY

print32_loop:
    lodsb                       ; Get character from string
    mov ah, WHITE_ON_BLACK
    
    cmp al, 0                   ; char == 0
    je print32_done

    mov [edx], ax               ; store character + attribute in video memory
    add edx, 2                  ; advance to next video memory location
    jmp print32_loop

print32_done:
    popa
    ret
