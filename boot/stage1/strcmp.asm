;di = s1
;si = s2
strcmp:
	push si
	push di
.loop:
	mov al, byte [di]
	mov ah, byte [si]

	cmp al, ah
	jne .end
	
	cmp ax, 0
	je .end

	inc di
	inc si
	jmp .loop

.end:
	pop di
	pop si
	ret
