HEADER_MAGIC	equ "_ cole _"
HEADER_FLAGS	equ 0x00000000
[BITS 64]
[section .text]
global _start
_start:
	jmp start
	extern ctr0, pipe_r, ptr_mouse2
	extern _end
Align 8
header:
    dq HEADER_MAGIC
    dq HEADER_FLAGS    
    dq header
	dq _start
	dq 0
    dq _end
    dq 0
    dq 0

	times 0x1000 -($-$$)   	db 0
	
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

[section .bss]
;resb 0x2000 ; 8 KiB TODO o Lua 1.0 me obriga a fazer isto, e ao que parece o .eh_frame  

