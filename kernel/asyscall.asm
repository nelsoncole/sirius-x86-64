%include "include/inc.asm"
[BITS 64]
[section .text]
global int114	
extern syscall_function_handler, ret, SavedFloats2

syscall_jmp:
	push gs
	push fs
      	
    push r15
	push r14
	push r13
	push r12
	push r11
	push r10
	push r9
	push r8
	push rdi
	push rsi
	push rbp
	push rsp
	push rbx
	push rdx
	push rcx
	push rax
      	
    ;PUSH_FPU
	;PUSH_XMM
	fxsave [SavedFloats2]

	
	
	mov rcx, [rsp + (8*(2-1))]
	mov rdx, [rsp + (8*(3-1))]
	mov rsi, [rsp + (8*(7-1))]
	mov rdi, [rsp + (8*(8-1))]
	mov r8,  [rsp + (8*(9-1))]
	
	call syscall_function_handler
	
	
	
	;POP_XMM
	;POP_FPU
	fxrstor [SavedFloats2]
	
	pop rax
	pop rcx
	pop rdx
	pop rbx
	pop rsp
	pop rbp
	pop rsi
	pop rdi
	pop r8
	pop r9
	pop r10
	pop r11
	pop r12
	pop r13
	pop r14
	pop r15
	
	pop fs
	pop gs
	
	mov rax, qword[ret] 
	add rsp, 16
	iretq
	
int114:   
	;cli
	
	push qword 0
	push qword 114
	jmp syscall_jmp
