
getchar:
	xor ax, ax
	int 0x16
	mov	ah, 0x0e
	int 	0x10	
	ret
	
	
