bits 64
global setjmp
setjmp:
    mov [rdi], rbx
    mov [rdi + 8], rbp
    mov [rdi + 16], r12
    mov [rdi + 24], r13
    mov [rdi + 32], r14
    mov [rdi + 40], r15
    lea rdx, [rsp + 8]
    mov [rdi + 48], rdx
    mov rdx, [rsp]
    mov [rdi + 56], rdx
    xor rax, rax
    ret

;CPU x86
;global setjmp
;setjmp:
;   mov eax, [esp + 4]
;   mov [eax], ebx
;   mov [eax + 4], esi
;   mov [eax + 8], edi
;   mov [eax + 12], ebp
;   lea ecx, [esp + 4]
;   mov [eax + 16], ecx
;   mov ecx, [esp]
;   mov [eax + 20], ecx
;   xor eax, eax
;   ret


