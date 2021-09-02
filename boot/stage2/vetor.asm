bits 32
section .text
global isr00,isr01,isr02,isr03,isr04,isr05,isr06,isr07,isr08,isr09
global isr10,isr11,isr12,isr13,isr14,isr15,isr16,isr17,isr18,isr19
global isr20,isr21,isr22,isr23,isr24,isr25,isr26,isr27,isr28,isr29
global isr30,isr31

extern fault_exception
isr_jmp:
pushad
	push ds
	push es
	push fs
	push gs
    	push ss
	mov eax,[esp+52]	; pega vetor
	push eax
	mov eax,0x10
	mov ds,ax
	mov es,ax
	mov fs,ax
	mov gs,ax

	call fault_exception
	pop eax
	
    	pop ss
	pop gs
	pop fs
	pop es
	pop ds
	popad	
	add esp,8
	iretd

isr00:
	push dword 0
	push dword 0
	jmp isr_jmp
isr01:
	push dword 0
	push dword 1
	jmp isr_jmp
isr02:
	push dword 0
	push dword 2
	jmp isr_jmp
isr03:
	push dword 0
	push dword 3
	jmp isr_jmp
isr04:
	push dword 0
	push dword 4
	jmp isr_jmp
isr05:
	push dword 0
	push dword 5
	jmp isr_jmp
isr06:
	push dword 0
	push dword 6
	jmp isr_jmp
isr07:
	push dword 0
	push dword 7
	jmp isr_jmp
isr08:
	;error code
	push dword 8
	jmp isr_jmp
isr09:
	push dword 0
	push dword 9
	jmp isr_jmp
isr10:
	;error code
	push dword 10
	jmp isr_jmp
isr11:
	;error code
	push dword 11
	jmp isr_jmp
isr12:
	;error code
	push dword 12
	jmp isr_jmp
isr13:
	;error code
	push dword 13
	jmp isr_jmp
isr14:
	;error code
	push dword 14
	jmp isr_jmp
isr15:
	push dword 0
	push dword 15
	jmp isr_jmp
isr16:
	push dword 0
	push dword 16
	jmp isr_jmp
isr17:
	;error code
	push dword 17
	jmp isr_jmp
isr18:
	push dword 0
	push dword 18
	jmp isr_jmp
isr19:
	push dword 0
	push dword 19
	jmp isr_jmp
isr20:
	push dword 0
	push dword 20
	jmp isr_jmp
isr21:
	push dword 0
	push dword 21
	jmp isr_jmp
isr22:
	push dword 0
	push dword 22
	jmp isr_jmp
isr23:
	push dword 0
	push dword 23
	jmp isr_jmp
isr24:
	push dword 0
	push dword 24
	jmp isr_jmp
isr25:
	push dword 0
	push dword 25
	jmp isr_jmp
isr26:
	push dword 0
	push dword 26
	jmp isr_jmp
isr27:
	push dword 0
	push dword 27
	jmp isr_jmp
isr28:
	push dword 0
	push dword 28
	jmp isr_jmp
isr29:
	push dword 0
	push dword 29
	jmp isr_jmp
isr30:
	push dword 0
	push dword 30
	jmp isr_jmp
isr31:
	push dword 0
	push dword 31
	jmp isr_jmp

global irq00,irq01,irq02,irq03,irq04,irq05,irq06,irq07,irq08,irq09
global irq10,irq11,irq12,irq13,irq14,irq15


extern irq_function
irq00:

	push dword 0
	push dword 32
irq_jmp:
	pushad
	push ds
	push es
	push ss
	push fs
	push gs
	mov eax,[esp+52]	; pega vetor
	push eax
	mov eax,0x10
	mov ds,ax
	mov es,ax
	mov fs,ax
	mov gs,ax



	call irq_function
    	pop eax

	pop gs
	pop fs
	pop ss
	pop es
	pop ds
    	popad
	add esp,8
	iretd


irq01:

	push dword 0
	push dword 33
	jmp irq_jmp

irq02:

	push dword 0
	push dword 34
	jmp irq_jmp

irq03:

	push dword 0
	push dword 35
	jmp irq_jmp

irq04:

	push dword 0
	push dword 36
	jmp irq_jmp

irq05:

	push dword 0
	push dword 37
	jmp irq_jmp

irq06:

	push BYTE 0
	push BYTE 38
	jmp irq_jmp

irq07:

	push dword 0
	push dword 39
	jmp irq_jmp

irq08:

	push dword 0
	push dword 40
	jmp irq_jmp

irq09:

	push dword 0
	push dword 41
	jmp irq_jmp

irq10:

	push dword 0
	push dword 42
	jmp irq_jmp

irq11:

	push dword 0
	push dword 43
	jmp irq_jmp

irq12:

	push dword 0
	push dword 44
	jmp irq_jmp

irq13:

	push dword 0
	push dword 45
	jmp irq_jmp

irq14:

	push dword 0
	push dword 46
	jmp irq_jmp

irq15:

	push dword 0
	push dword 47
	jmp irq_jmp
