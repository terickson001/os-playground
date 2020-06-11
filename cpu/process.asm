global read_eip
global perform_task_switch
read_eip:
pop eax
jmp eax

perform_task_switch:
cli
mov ecx, [esp+4]   ; EIP
mov eax, [esp+8]   ; physical address of current directory
mov ebp, [esp+12]  ; EBP
mov esp, [esp+16]  ; ESP
mov cr3, eax       ; set the page directory
mov eax, 0xBEEF    ; magic number to detect a task switch
sti
jmp ecx