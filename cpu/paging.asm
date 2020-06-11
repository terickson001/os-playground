global load_page_directory
global enable_paging
global copy_page_frame
global flush_tlb

load_page_directory:
push ebp
mov ebp, esp

mov eax, [esp+8]
mov cr3, eax

mov esp, ebp
pop ebp
ret

enable_paging:
push ebp
mov ebp, esp

mov eax, cr0
or  eax, 0x80000000
mov cr0, eax

mov esp, ebp
pop ebp
ret

copy_page_frame:
push ebp
mov ebp, esp

push ebx
pushf

cli

mov ebx, [esp+12]   ; Source Frame Address
mov ecx, [esp+16]   ; Destination Frame Address

;mov edx,  cr0
;and edx, 0x7FFFFFFF ; Unset paging bit
;mov cr0, edx

mov edx, 1024       ; Counter

.loop:
mov eax, [ebx]      ; Load dword from source
mov ecx, [eax]      ; Store dword into source
add ebx, 4
add ecx, 4
dec edx
jnz .loop

;mov eax, cr0
;or  eax, 0x80000000 ; Re-Enable Paging
;mov cr0, eax

popf
pop ebx

mov esp, ebp
pop ebp
ret

flush_tlb:
mov eax, cr3
mov cr3, eax
ret