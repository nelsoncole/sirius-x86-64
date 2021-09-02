print:
	pusha
.next:
	cld 			; flag de direcção
	lodsb			; a cada loop carrega si p --> al, actualizando si
	cmp 	al, 0		; compara al com o 0
	je 	.end		; se al for 0 pula para o final do programa	
	mov	ah, 0x0e	; função TTY da BIOS imprime caracter na tela
	int 	0x10		; interrupção de vídeo
	jmp 	.next
.end:
	popa
	ret
