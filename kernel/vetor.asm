%include "include/inc.asm"
[BITS 64]
[section .text]
global isr00,isr01,isr02,isr03,isr04,isr05,isr06,isr07,isr08,isr09
global isr10,isr11,isr12,isr13,isr14,isr15,isr16,isr17,isr18,isr19
global isr20,isr21,isr22,isr23,isr24,isr25,isr26,isr27,isr28,isr29
global isr30,isr31

extern EnableSSE, DisableSSE
extern RestoreSSE, SaveSSE

extern fault_exception
isr_jmp:

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

	fxsave [SavedFloats2]
	
	mov rdi, [rsp + 0x90]
	
	call fault_exception

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
	
	
	add rsp, 16
	iretq

isr00:
	push qword 0
	push qword 0
	jmp isr_jmp
isr01:
	push qword 0
	push qword 1
	jmp isr_jmp
isr02:
	push qword 0
	push qword 2
	jmp isr_jmp
isr03:
	push qword 0
	push qword 3
	jmp isr_jmp
isr04:
	push qword 0
	push qword 4
	jmp isr_jmp
isr05:
	push qword 0
	push qword 5
	jmp isr_jmp
isr06:
	push qword 0
	push qword 6
	jmp isr_jmp
isr07:
	push qword 0
	push qword 7
	jmp isr_jmp
isr08:
	;error code
	push qword 8
	jmp isr_jmp
isr09:
	push qword 0
	push qword 9
	jmp isr_jmp
isr10:
	;error code
	push qword 10
	jmp isr_jmp
isr11:
	;error code
	push qword 11
	jmp isr_jmp
isr12:
	;error code
	push qword 12
	jmp isr_jmp
isr13:
	;error code
	push qword 13
	jmp isr_jmp
isr14:
	;error code
	push qword 14
	jmp isr_jmp
isr15:
	push qword 0
	push qword 15
	jmp isr_jmp
isr16:
	push qword 0
	push qword 16
	jmp isr_jmp
isr17:
	;error code
	push qword 17
	jmp isr_jmp
isr18:
	push qword 0
	push qword 18
	jmp isr_jmp
isr19:
	push qword 0
	push qword 19
	jmp isr_jmp
isr20:
	push qword 0
	push qword 20
	jmp isr_jmp
isr21:
	push qword 0
	push qword 21
	jmp isr_jmp
isr22:
	push qword 0
	push qword 22
	jmp isr_jmp
isr23:
	push qword 0
	push qword 23
	jmp isr_jmp
isr24:
	push qword 0
	push qword 24
	jmp isr_jmp
isr25:
	push qword 0
	push qword 25
	jmp isr_jmp
isr26:
	push qword 0
	push qword 26
	jmp isr_jmp
isr27:
	push qword 0
	push qword 27
	jmp isr_jmp
isr28:
	push qword 0
	push qword 28
	jmp isr_jmp
isr29:
	push qword 0
	push qword 29
	jmp isr_jmp
isr30:
	push qword 0
	push qword 30
	jmp isr_jmp
isr31:
	push qword 0
	push qword 31
	jmp isr_jmp
	
global lvt0,lvt1,lvt2,lvt3,lvt4
extern lvt_function
lvt_jmp:
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
      	
	;fxsave [SavedFloats2]
	
	mov rdi, [rsp + 0x90]
	call lvt_function

	;fxrstor [SavedFloats2]

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
	
	
	add rsp, 16
	iretq

lvt_jmp2:
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
      	
	;fxsave [SavedFloats2]

	mov rdi, [rsp + 0x90]
	call lvt_function

	;fxrstor [SavedFloats2]

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
	
	
	add rsp, 16
	iretq
;timer
extern context_rip, context_rflag, context_cr3
extern context_rax, context_rbx, context_rcx, context_rdx
extern context_rdi, context_rsi
extern context_rbp, context_rsp
extern context_cs, context_es, context_ds, context_fs, context_gs, context_ss
extern context_r15, context_r14, context_r13, context_r12, context_r11, context_r10, context_r9, context_r8
extern task_switch, mult_task
extern task_switch2

extern context_fxsalve
	
lvt0:
	
	cmp byte [mult_task], 0
	je no_mult_task
	mov byte [mult_task], 0
	

	pop qword [context_rip	]
    pop qword [context_cs	]
    pop qword [context_rflag]
    pop qword [context_rsp	]
    pop qword [context_ss	]
    	

	;registers 
    mov qword [context_rax], rax 
    mov qword [context_rbx], rbx 
    mov qword [context_rcx], rcx
    mov qword [context_rdx], rdx

	;registers  
    mov qword [context_rdi], rdi
    mov qword [context_rsi], rsi
	
	;registers
	mov qword [context_rbp], rbp
	
	;registers
	mov qword [context_r8], r8
	mov qword [context_r9], r9
	mov qword [context_r10], r10
	mov qword [context_r11], r11
	mov qword [context_r12], r12
	mov qword [context_r13], r13
	mov qword [context_r14], r14
	mov qword [context_r15], r15
	
	;segments
    xor rax, rax
    mov rax, ds
    mov word [context_ds], ax
    mov rax, es
    mov word [context_es], ax
    mov rax, fs
    mov word [context_fs], ax
    mov rax, gs
    mov word [context_gs], ax


	mov rax, cr3
    mov qword [context_cr3], rax
    
    mov rdi, SavedFloats
    call SaveSSE

	call task_switch
	mov rax, qword [context_cr3]
    mov cr3, rax
    	
    mov rcx, 4 ;TLB
.loop:
	nop
	loop .loop
	
	
	mov rdi, SavedFloats
    call RestoreSSE

	;segments
    xor rax, rax
    mov ax,	word [context_ds]
    mov ds,	ax
	mov ax,	word [context_es]
    mov es,	ax
	mov ax,	word [context_fs]
    mov fs,	ax
	mov ax,	word [context_gs]
    mov gs,	ax
    	
	;registers
	mov rbp, qword [context_rbp]

	;registers  
    mov rdi, qword [context_rdi]
    mov rsi, qword [context_rsi]

	;registers 
    mov rax, qword [context_rax]
    mov rbx, qword [context_rbx]
    mov rcx, qword [context_rcx]
    mov rdx, qword [context_rdx]
    	
    ;registers 
    mov r8, qword [context_r8]
    mov r9, qword [context_r9]
    mov r10, qword [context_r10]
    mov r11, qword [context_r11]
    mov r12, qword [context_r12]
    mov r13, qword [context_r13]
    mov r14, qword [context_r14]
    mov r15, qword [context_r15]
    	
    	

	push qword [context_ss	 ]
	push qword [context_rsp ]
	push qword [context_rflag]
	push qword [context_cs	 ]
	push qword [context_rip ]
	
	mov byte [mult_task], 1

no_mult_task:
	push qword 0
	push qword 32
	jmp lvt_jmp

lvt1:
	push qword 0
	push qword 33
	jmp lvt_jmp
lvt2:
	push qword 0
	push qword 34
	jmp lvt_jmp
lvt3:
	push qword 0
	push qword 35
	jmp lvt_jmp
lvt4:
	push qword 0
	push qword 36
	jmp lvt_jmp

; __MP__
global timer1,timer2,timer3,timer4,timer5, timer6, timer7, timer8

iv db 0
timer1:
;ap_jmp:

	cmp byte[iv], 1 ; isto vai bloquei o taskswiching do ap
	je __no_mult_task

	cmp byte [mult_task], 0
	je __no_mult_task
	
	mov byte[iv], 1
	
	mov byte [mult_task], 0
	
	pop qword [context_rip	]
    pop qword [context_cs	]
    pop qword [context_rflag]
    pop qword [context_rsp	]
    pop qword [context_ss	]

	;registers 
    mov qword [context_rax], rax 
    mov qword [context_rbx], rbx 
    mov qword [context_rcx], rcx
    mov qword [context_rdx], rdx

	;registers  
    mov qword [context_rdi], rdi
    mov qword [context_rsi], rsi
	
	;registers
	mov qword [context_rbp], rbp
	
	;registers
	mov qword [context_r8], r8
	mov qword [context_r9], r9
	mov qword [context_r10], r10
	mov qword [context_r11], r11
	mov qword [context_r12], r12
	mov qword [context_r13], r13
	mov qword [context_r14], r14
	mov qword [context_r15], r15
	
	;segments
    xor rax, rax
    mov rax, ds
    mov word [context_ds], ax
    mov rax, es
    mov word [context_es], ax
    mov rax, fs
    mov word [context_fs], ax
    mov rax, gs
    mov word [context_gs], ax


	mov rax, cr3
    mov qword [context_cr3], rax
    	

	call task_switch2
	mov rax, qword [context_cr3]
    mov cr3, rax
    	
    mov rcx, 4 ;TLB
.loop:
	nop
	loop .loop

	;segments
    xor rax, rax
    mov ax,	word [context_ds]
    mov ds,	ax
	mov ax,	word [context_es]
    mov es,	ax
	mov ax,	word [context_fs]
    mov fs,	ax
	mov ax,	word [context_gs]
    mov gs,	ax
    	

	;registers
	mov rbp, qword [context_rbp]

	;registers  
    mov rdi, qword [context_rdi]
    mov rsi, qword [context_rsi]

	;registers 
    mov rax, qword [context_rax]
    mov rbx, qword [context_rbx]
    mov rcx, qword [context_rcx]
    mov rdx, qword [context_rdx]
    	
    ;registers 
    mov r8, qword [context_r8]
    mov r9, qword [context_r9]
    mov r10, qword [context_r10]
    mov r11, qword [context_r11]
    mov r12, qword [context_r12]
    mov r13, qword [context_r13]
    mov r14, qword [context_r14]
    mov r15, qword [context_r15]


	push qword [context_ss	 ]
	push qword [context_rsp ]
	push qword [context_rflag]
	push qword [context_cs	 ]
	push qword [context_rip ]

	mov byte [mult_task], 1
__no_mult_task:
	;ret

	;call ap_jmp2
	push qword 0
	push qword 37
	jmp lvt_jmp

timer2:
	push qword 0
	push qword 38
	jmp lvt_jmp
timer3:
	push qword 0
	push qword 39
	jmp lvt_jmp
timer4:
	push qword 0
	push qword 40
	jmp lvt_jmp
timer5:
	push qword 0
	push qword 41
	jmp lvt_jmp
timer6:
	push qword 0
	push qword 42
	jmp lvt_jmp
timer7:
	push qword 0
	push qword 43
	jmp lvt_jmp
timer8:
	push qword 0
	push qword 44
	jmp lvt_jmp


global irq0,irq1,irq2,irq3,irq4,irq5,irq6,irq7,irq8,irq9
global irq10,irq11,irq12,irq13,irq14,irq15,irq16,irq17,irq18,irq19
global irq20,irq21,irq22,irq23

extern irq_function
irq_jmp:

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

	;fxsave [SavedFloats2]
	
	mov rdi, [rsp + 0x90]
	call irq_function

	;fxrstor [SavedFloats2]

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
	
	
	add rsp, 16
	iretq
	
irq0:
	push qword 0
	push qword 64 
	jmp irq_jmp
	
irq1:
	push qword 0
	push qword 65 
	jmp irq_jmp

irq2:
	push qword 0
	push qword 66 
	jmp irq_jmp

irq3:
	push qword 0
	push qword 67 
	jmp irq_jmp

irq4:
	push qword 0
	push qword 68 
	jmp irq_jmp
	
irq5:
	push qword 0
	push qword 69 
	jmp irq_jmp
	
irq6:
	push qword 0
	push qword 70 
	jmp irq_jmp

irq7:
	push qword 0
	push qword 71 
	jmp irq_jmp

irq8:
	push qword 0
	push qword 72 
	jmp irq_jmp

irq9:
	push qword 0
	push qword 73 
	jmp irq_jmp

irq10:
	push qword 0
	push qword 74 
	jmp irq_jmp
	
irq11:
	push qword 0
	push qword 75 
	jmp irq_jmp

irq12:
	push qword 0
	push qword 76 
	jmp irq_jmp

irq13:
	push qword 0
	push qword 77
	jmp irq_jmp

irq14:
	push qword 0
	push qword 78
	jmp irq_jmp
	
irq15:
	push qword 0
	push qword 79 
	jmp irq_jmp
	
irq16:
	push qword 0
	push qword 80 
	jmp irq_jmp

irq17:
	push qword 0
	push qword 81 
	jmp irq_jmp

irq18:
	push qword 0
	push qword 82 
	jmp irq_jmp

irq19:
	push qword 0
	push qword 83 
	jmp irq_jmp

irq20:
	push qword 0
	push qword 84 
	jmp irq_jmp
	
irq21:
	push qword 0
	push qword 85 
	jmp irq_jmp

irq22:
	push qword 0
	push qword 86 
	jmp irq_jmp

irq23:
	push qword 0
	push qword 87 
	jmp irq_jmp


global intr255
extern intr_function
intr_jmp:

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
      	
	fxsave [SavedFloats2]
	
	mov rdi, [rsp + 0x90]
	
	call intr_function
	
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
	
	
	add rsp, 16
	iretq

intr255:
	push qword 0
	push qword 255
	jmp intr_jmp


global intr_null
extern intr_null_function
intr_null:
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
      	
	fxsave [SavedFloats2]
	
	mov rdi, [rsp + 0x90]
	
	call intr_null_function
	
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
	
	iretq

global msi0,msi1,msi2,msi3,msi4,msi5,msi6,msi7,msi8,msi9
global msi10,msi11,msi12,msi13,msi14,msi15,msi16,msi17,msi18,msi19
global msi20,msi21,msi22,msi23,msi24,msi25,msi26,msi27,msi28,msi29
global msi30,msi31

extern msi_function
msi_jmp:

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

	;fxsave [SavedFloats2]
	
	mov rdi, [rsp + 0x90]
	call msi_function

	;fxrstor [SavedFloats2]

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
	
	
	add rsp, 16
	iretq
	
msi0:
	push qword 0
	push qword 0 
	jmp msi_jmp
	
msi1:
	push qword 0
	push qword 1 
	jmp msi_jmp

msi2:
	push qword 0
	push qword 2 
	jmp msi_jmp
	
msi3:
	push qword 0
	push qword 3 
	jmp msi_jmp

msi4:
	push qword 0
	push qword 4 
	jmp msi_jmp
	
msi5:
	push qword 0
	push qword 5 
	jmp msi_jmp

msi6:
	push qword 0
	push qword 6 
	jmp msi_jmp
	
msi7:
	push qword 0
	push qword 7 
	jmp msi_jmp

msi8:
	push qword 0
	push qword 8 
	jmp msi_jmp
	
msi9:
	push qword 0
	push qword 9 
	jmp msi_jmp

msi10:
	push qword 0
	push qword 10 
	jmp msi_jmp
	
msi11:
	push qword 0
	push qword 11 
	jmp msi_jmp

msi12:
	push qword 0
	push qword 12 
	jmp msi_jmp
	
msi13:
	push qword 0
	push qword 13 
	jmp msi_jmp

msi14:
	push qword 0
	push qword 14 
	jmp msi_jmp
	
msi15:
	push qword 0
	push qword 15 
	jmp msi_jmp

msi16:
	push qword 0
	push qword 16 
	jmp msi_jmp
	
msi17:
	push qword 0
	push qword 17 
	jmp msi_jmp

msi18:
	push qword 0
	push qword 18 
	jmp msi_jmp
	
msi19:
	push qword 0
	push qword 19 
	jmp msi_jmp

msi20:
	push qword 0
	push qword 20 
	jmp msi_jmp
	
msi21:
	push qword 0
	push qword 21 
	jmp msi_jmp

msi22:
	push qword 0
	push qword 22 
	jmp msi_jmp
	
msi23:
	push qword 0
	push qword 23 
	jmp msi_jmp

msi24:
	push qword 0
	push qword 24 
	jmp msi_jmp
	
msi25:
	push qword 0
	push qword 25 
	jmp msi_jmp

msi26:
	push qword 0
	push qword 26 
	jmp msi_jmp
	
msi27:
	push qword 0
	push qword 27 
	jmp msi_jmp

msi28:
	push qword 0
	push qword 28 
	jmp msi_jmp
	
msi29:
	push qword 0
	push qword 29 
	jmp msi_jmp

msi30:
	push qword 0
	push qword 30 
	jmp msi_jmp
	
msi31:
	push qword 0
	push qword 31 
	jmp msi_jmp

global intr_vmnet
extern handler_vmnet;
intr_vmnet:
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
      	
	fxsave [SavedFloats2]
	
	mov rdi, [rsp + 0x90]
	
	call handler_vmnet
	
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
	
	iretq

segment .data
global SavedFloats
align 16
SavedFloats: TIMES 512 db 0
global SavedFloats2
align 16
SavedFloats2: TIMES 512 db 0

