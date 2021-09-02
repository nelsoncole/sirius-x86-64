%include "include/inc.asm"
[BITS 32]
section .text
global _start
_start:
	jmp start
	
	
	extern main
	extern dv_num
	extern dv_uid
	
	
start:

	mov esp, _stack
	mov ebp, 0;
	
	mov dword[dv_uid],ebx
	mov byte[dv_num],dl
	mov dword[file_sector_count],ecx
	call main

	hlt

global gdt_flush
gdt_flush:
    	push ebp
	mov ebp, esp
    	mov eax, [esp+8]
    	lgdt [eax]
    	jmp 0x08:.flush
.flush:
    	mov ax, 0x10
    	mov ds, ax
    	mov es, ax
    	mov fs, ax
    	mov gs, ax
	mov ss, ax

    	mov esp, ebp
	pop ebp
    	ret

global idt_flush
idt_flush:
    	push ebp
	mov ebp,esp
    	mov eax, [esp+8]
    	lidt [eax]
    	mov esp,ebp
	pop ebp
    	ret

global long_mode
long_mode:    
	push ebp
	mov ebp, esp
    	mov eax, [esp + 0x08] ; GDTR
    	mov edx, [esp + 0x0c] ; Kernel address = 0x401000
    	mov ebx, [esp + 0x10] ; bootblock
    	mov esp, ebp
	pop ebp
	
	
    	cli
    	lgdt [eax]
    	
    	jmp 0x08: start64
    	
global file_sector_count
file_sector_count dd 0
    	
align 16
[BITS 64]
start64:

	xor rcx, rcx
	mov ecx, eax
	
	lgdt [rcx]

	xor r8, r8
	xor r9, r9
	xor r10, r10
	xor r11, r11
	xor r12, r12
	xor r13, r13
	xor r14, r14
	xor r15, r15
	
	xor rbp, rbp
	mov rsp, _stack

	xor rax, rax
    	mov ds, ax
	mov es, ax
	mov ss, ax
	mov fs, ax
	mov gs, ax
	
	
	mov eax, ebx
	xor rbx, rbx
	
	mov ebx, eax 
	mov eax, edx
	
	push rbx
	jmp rax
	nop

	jmp $
	

section .bss
	resb 0x8000 ;32KiB
_stack:
