cmd:
	mov si, strcmd1
	call print
.cmd:	
	mov si, strcmd2
	call print
	
	mov di, 0x500
	push cx
	push ax
	mov cx, 32	
.loop:	
	mov ax, 0
	mov word[di], ax
	add di, 2
	loop .loop
	pop ax	
	pop cx
	
	mov di, 0x500
	
.getchar:
	call getchar
	cmp al, 13
	je .end2
	mov [di], al 
	inc di
	jmp .getchar
.end2:
	mov al, 0xa
	mov ah, 0x0e
	int 0x10
	mov al, 13
	int 0x10
	
	mov di, 0x500
	mov si, cmdboot
	call strcmp

	cmp ax, 0
	je .end
	
	mov di, 0x500
	mov si, cmdinstall
	call strcmp
	
	cmp ax, 0
	je .copy
	
;	
	
	
	jmp .cmd
.end:	
	ret
	
.copy:	

	push cx
	mov cx, 16384 ;8192
	
	xor di,di
	mov bx,0x600
	mov eax, 0
.loop2:
	
	call read
	call write
	
	add eax, 1
	loop .loop2	
	pop cx
	
	int 0x19
	ret	
	
	
	
	
	
	
	
