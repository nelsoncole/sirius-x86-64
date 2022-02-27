[bits 16]
[org 0x8000]	
	jmp 0:code

times 0x8 -($-$$) db 0
spinlock: 	dd 0	
aprunning: 	dd 0
stack: 	    dq 0
COPY_GDTR: 	dq 0
COPY_IDTR: 	dq 0
PML4E: 	    dq 0
func_enter: dq 0
apic_tmr_vector: dd 0

align 16
gdt:
    dd 0, 0
    dd 0x0000FFFF, 0x00CF9A00    ; code
    dd 0x0000FFFF, 0x00CF9200    ; data
gdtr:
    dw gdtr - gdt - 1
    dd gdt
    dd 0
code:
	cli
    cld
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov sp, 0x7E00

    ; Enable A20 Gate
    in al, 0x92
    or al, 0x02
    out 0x92, al

	xor   	ax, ax
    mov 	ds, ax
    mov	    eax, gdtr
    db 0x66		; Execute GDT 32-Bits
	lgdt    [eax]
    	
    mov 	eax, cr0
    or	    eax, 0x10001    ; Turn on protected mode and write protection
    mov 	cr0, eax
    jmp  	8: start32
    	
align 16
[bits 32]
start32:
	mov	    ax, 0x10
    mov 	ds, ax
    mov 	es, ax
    mov 	fs, ax
    mov	    gs, ax
    mov 	ss, ax
    	
    xor	    ebp, ebp
    mov 	esp, dword[stack]
    	
    lock    inc dword[aprunning]
    	
; Local APIC

	mov 	eax, -1
	mov 	dword[0xFEE00000 + 0xE0], eax   ; Value after reset
	
	mov 	eax, dword[0xFEE00000 + 0xD0]
	or 	    eax, 1
	mov 	dword[0xFEE00000 + 0xD0], eax	
	
	;Task Priority Register (TPR), to inhibit softint delivery
	xor 	eax, eax
	mov 	dword[0xFEE00000 + 0x80], eax
	
    ;Spurious vector to 255
	mov 	eax, 0x1FF
    mov 	dword[0xFEE000F0], eax
	
	;Timer interrupt vector, to disable timer interrupts
	mov 	eax, 0x24
	add	    eax, dword[aprunning]	; vector 37++
    mov     dword[apic_tmr_vector], eax
    or      eax, 0x10000
	mov 	dword[0xFEE00000 + 0x320], eax


    ;Performance counter interrupt, to disable performance counter interrupts
    mov 	eax, 0x10000
    mov 	dword[0xFEE00000 + 0x0340], eax

    ;Local interrupt 0, to enable normal external interrupts, Trigger Mode = Level
    mov 	eax, 0x10000
    mov 	dword[0xFEE00000 + 0x0350], eax

    ;Local interrupt 1, to enable normal NMI processing
    mov 	eax, 0x10000
    mov 	dword[0xFEE00000 + 0x0360], eax

    ;Error interrupt, to disable error interrupts
    mov 	eax, 0x10000
    mov 	dword[0xFEE00000 + 0x0370], eax
	
	
	mov 	ecx, 0x1B
    mov 	eax, 0xFEE00800
    mov 	edx, 0
    wrmsr
	
	
	; Timer
    mov 	eax, dword[apic_tmr_vector]
    or      eax, 0x20000 ; periodic mode
	mov 	dword[0xFEE00000 + 0x320], eax
	
	mov 	eax, 0x10
	mov 	dword[0xFEE00000 + 0x380], eax

    mov 	eax, 0x3
	mov 	dword[0xFEE00000 + 0x3e0], eax
	  	    	
; PAE
	mov 	eax, cr4
    or	    eax, 0x20	
    mov 	cr4, eax
    	
    mov 	edi, dword[PML4E]
	mov 	cr3, edi
    	
    mov 	ecx, 0xC0000080
    rdmsr
    or 	    eax, 0x100
    mov 	edx, 0
    wrmsr
	
	mov 	eax, cr0
    or	    eax, 0x80000000
    mov 	cr0, eax
    	  
	; execute modo longo
    mov	eax, dword[COPY_GDTR]
	lgdt [eax]
	jmp 8:start64
align 16	
[bits 64]
start64: 

	xor 	rax, rax
	mov 	ds, ax
    mov 	es, ax
    mov 	fs, ax
    mov	    gs, ax
    mov 	ss, ax
   
    ; carregar o stack
	;xor 	rbp, rbp
    ;mov	    rsp, qword[stack]
	
	
	mov	rax, qword[COPY_GDTR]
	lgdt 	[rax]
	
	;Set IA32_FMASK (flags mask)
	mov ecx, 0xC0000084
	rdmsr
	mov eax, ~0x202
	wrmsr
	
	;Set IA32_STAR ( kernel/ user CS)
	mov ecx, 0xC0000081
	rdmsr
	;Kernel CS (and kernel DS/SS - 8), user CS
	mov edx, 0x8 | (0x1B << 16)
	wrmsr
	
	
	mov 	rax, qword[COPY_IDTR]
	lidt 	[rax]
	
; get our Local APIC ID
    mov   	eax, 1
    cpuid
    shr   	ebx, 24
    
    ; jump into C code (should never return)
    xor	    rax, rax
    mov 	eax, dword[func_enter]
    mov	    rdi, 1
 
; enable sse

	mov     rax, cr4
	or      rax, 0x600 ; OSFXSR and OSXMMEXCPT
	mov     cr4, rax
	

	mov     rax, cr0
	and     ax,  0xFFFB ; Clear EM
	or      rax, 0x2    ; Set MP
	mov     cr0, rax

	fninit
	
	mov rax, cr0
	and ax, ~(1 << 3)	; Clear TS
	mov cr0, rax

 ; spinlock, wait for the BSP to finish
.loop:  
	pause
    cmp 	byte[spinlock], 0 
    jz      .loop
    
    sti
L1:	
	hlt
	jmp L1 
align 16   
times 4096 - ($-$$) db 0	
    	
