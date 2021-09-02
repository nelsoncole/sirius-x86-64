%include "include/inc.asm"
[bits 64]
section .text
global _start
_start:
	jmp start
	extern main
start:
	mov rsp, KERNELSTACK ;_stack
	
	mov rax, (KERNELSTACK - 0x100000) ; - 1MiB
	mov qword[_rsp0], rax
	
	mov rax, APsSTACK
	mov qword[_ap_stack], rax
	
	xor rdi,rdi
	mov edi,ebx
	call main
	
L1:	hlt
	jmp L1

global gdt_flush
gdt_flush:
    	mov rax, rdi
    	lgdt [rax]
	
	;Set IA32_FMASK (flags mask)
	mov ecx, 0xC0000084
	rdmsr
	mov eax, ~0x202
	wrmsr
	
	
	; Set IA32_STAR ( kernel/ user CS)
	mov ecx, 0xC0000081
	rdmsr
	; Kernel CS (and kernel DS/SS - 8), user CS
	mov edx, 0x8 | (0x1B << 16)
	wrmsr
	
    	ret

global idt_flush
idt_flush:
   	mov rax, rdi
    	lidt [rax]

    	ret
    	
; call em x86_64 (RDI, RSI, RDX, RCX, R8 e R9)	
global call_function
call_function:
   	mov rax, rdi
   	
   	mov rdi, rsi
   	mov rsi, rdx
   	mov rdx, rcx
   	mov rcx, r8
   	mov r8,  r9
  
    call rax
    ret
    	
global copymem     	
copymem:
	
	cli
	push rbx
	
	mov rcx, rdx

	xor rdx, rdx
	mov rax, rcx
	mov rbx, 8
	div rbx
	mov rcx, rax
	
.loop:
	mov rax, qword[rsi]
	mov qword[rdi], rax

	add rdi,8
	add rsi,8
	
	loop .loop

    	pop rbx
    	sti
	ret
	
[global DisableSSE]
DisableSSE:
	mov rax, cr0
	or ax, 1 << 3	; Set TS
	mov cr0, rax
	ret
[global EnableSSE]
EnableSSE:
	mov rax, cr0
	and ax, ~(1 << 3)	; Clear TS
	mov cr0, rax
	ret

[global SaveSSE]
SaveSSE:
	fxsave [rdi]
	ret
	
[global RestoreSSE]
RestoreSSE:
	fxrstor [rdi]
	ret

section .data
	
global _rsp0
_rsp0: dq 0
global _ap_stack
_ap_stack: dq 0

section .bss
	resb 0x20000 ;128KiB
