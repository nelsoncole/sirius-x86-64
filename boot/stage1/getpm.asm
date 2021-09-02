
str_getpm_error1 db "BIOSes Extensions Parameters not present",10,13,0
str_getpm_error2 db "BIOSes Get Device Parameters Error",10,13,0
str_getpm_error3 db "BIOSes Enhanced disk device not suport (EDD)",10,13,0

GetDeviceParameters:
;checkExtensionsParameters
	mov ah, 0x41
	mov dl, byte[dv_num]
	mov bx, 0x55aa
	int 0x13
	jnc .present
	cmp bx,0xaa55
	je .present

	mov si,str_getpm_error1
	jmp getpm_error

.present:
	test cx,4
	jnz .suport
	mov si,str_getpm_error3
	jmp getpm_error
	
.suport:	

	push ds
	mov si, LDSBASE
	mov word[si],0x4A ; Length 74 Bytes
	mov ah, 0x48
	mov dl, byte[dv_num]
	int 0x13
	cmp ah,0
    	je .ok  

.error:
	pop ds
	mov si,str_getpm_error2
	jmp getpm_error
	
.ok:
	push es
	push ds
	push si
	push di

	mov di,LDSPARAMETERSBASE
	mov es,di
	xor di,di
	mov cx,0x4A
	cld
	rep movsb	

	pop di
	pop si
	pop ds
	pop es
	
	pop ds
	ret

getpm_error:
	call print
	ret
