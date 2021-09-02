
edid:
	push es
	push ds
	pusha
	
	mov ax, 0x4f15
	mov bl, 0x01
	xor cx, cx
	xor dx, dx
	mov es, dx
	mov di, 0x500
	int 0x10

	popa
	pop ds
	pop es
	
	ret
