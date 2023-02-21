HEADER_MAGIC	equ "_ cole _"
HEADER_FLAGS	equ 0x00000000
[BITS 64]
[section .text]
global _start
_start:
	jmp start
	extern ctr0, pipe_r, ptr_mouse2
start:

	mov r8, rbx 
	mov rbx, pipe_r
	mov qword [rbx], r10 
	
	mov rbx, ptr_mouse2
	mov [rbx], rax
	
	mov rbx, argc
	mov qword[rbx], r11
	mov rbx, argv
	mov qword[rbx], r12
	mov rbx, pwd_ptr
	mov qword[rbx], r13
	
	call ctr0
	jmp $
	
global call_function	
call_function:
   	mov rax, rdi
   	
   	mov rdi, rsi
   	mov rsi, rdx
   	mov rdx, rcx
   	mov rcx, r8
  
    call rax
    ret
  
[section .data]
global argc, argv, pwd_ptr
argc dq 0
argv dq 0
pwd_ptr dq 0
