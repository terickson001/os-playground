    ;; INPUTS:
    ;;  si: input string location
print:                          ; Routine: output string in SI to screen
    pusha
    mov ah, 0Eh                 ; int 10h 'print char' function

repeat:
    lodsb                       ; Get character from string
    cmp al, 0                   ; char == 0
    je done
    int 10h
    jmp repeat

done:
    popa
    ret
    
print_nl:
    pusha
    
    mov ah, 0x0e
    mov al, 0x0a ; newline char
    int 0x10
    mov al, 0x0d ; carriage return
    int 0x10
    
    popa
    ret
