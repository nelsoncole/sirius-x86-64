bits 64
global longjmp
longjmp:
    mov rax, rsi
    test rax, rax
    jnz .1
    inc rax
.1:
    mov rbx, [rdi]
    mov rbp, [rdi + 8]
    mov r12, [rdi + 16]
    mov r13, [rdi + 24]
    mov r14, [rdi + 32]
    mov r15, [rdi + 40]
    mov rdx, [rdi + 48]
    mov rsp, rdx
    mov rdx, [rdi + 56]
    jmp rdx

;CPU x86
;global longjmp
;longjmp:
;    mov edx, [esp + 4]
;    mov eax, [esp + 8]
;    test eax, eax
;    jnz .1
;    inc eax
;.1:
;    mov ebx, [edx]
;    mov esi, [edx + 4]
;    mov edi, [edx + 8]
;    mov ebp, [edx + 12]
;    mov ecx, [edx + 16]
;    mov esp, ecx
;    mov ecx, [edx + 20]    
;    jmp ecx


