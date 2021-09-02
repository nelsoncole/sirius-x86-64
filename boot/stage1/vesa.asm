str_vesa_error: 	db "VESA VBE ERROR",10,13,0
vesa_var0:   dw 0
vesa_var1:   db 32   ; BPB de pelomenos 32-bit
vesa_var2:   dw 800;1024 ; X Resolution de Pelomenos  => 1024 
vesa_var3:   dw 0x118  ; Especil mode 

vesa_vbe_mode:
; Info da chip VBE


    	call .vbe_infos
    	cmp ax,0x004F
    	jne vesa_error

	push ds
	xor si,si
	mov ds,si
	mov si,LDSBASE
	mov di,LDSVESABASE
	mov es,di
	xor di,di
	mov cx,0x200
	cld
	rep movsb
	pop ds

;   Info do modo de video

	; verifica se especial mode bpp = 32 bits
	
	MOV 	CX, WORD [vesa_var3]
	CALL 	vbe_info_mode
    	CMP 	AX, 0x004F
    	JNE 	vesa_error
	MOV 	AL, BYTE[vesa_var1]
 	CMP 	BYTE [vbe_mode_info_block + 25], AL
	JZ 	.SUCCESSFULL
	JMP	.NO_SUCCESSFULL
.SUCCESSFULL:
	MOV	AX, WORD [vesa_var3]
	MOV 	WORD [vesa_var0], AX
	PUSH	ES
	JMP	.end2
	
.NO_SUCCESSFULL:
	; Procurar outro modo.


	push es
	mov ax,word[LDSBASE+16]
	mov es,ax
	mov di,word[LDSBASE+14]
	xor bx,bx
.next:
	mov ax,word[di+bx]
	mov word[vesa_var0],ax

	mov cx,word[vesa_var0]
	cmp cx,0xffff
	jz .end

    	call vbe_info_mode
    	cmp ax,0x004F
    	jne vesa_error

	mov al,byte[vesa_var1]
 	cmp byte[vbe_mode_info_block+25],al
	jz .ok
	jmp .continue
.ok:
	mov ax,word[vesa_var2]
	cmp word[vbe_mode_info_block+18],ax
	jge .end2
.continue:

	add bx,2
	jmp .next
.end:
	cmp byte[vesa_var1],0
	jne .continue2
	cmp word[vesa_var2],800
	jz .error	
	mov word[vesa_var2],800
	mov byte[vesa_var1],32	
	xor bx,bx
	jmp .next

.continue2:
	sub byte[vesa_var1],8 ; TODO SUB 8-bytes or 16-bytes
	xor bx,bx
	jmp .next
.error:
	
	pop es
	jmp vesa_error

.end2:
	pop es

	push ds
	xor si,si
	mov ds,si
	mov si,vbe_mode_info_block
	mov di,LDSVESABASE
	mov es,di
	mov di,0x200
	mov cx,0x200
	cld
	rep movsb
	pop ds

; Nelson Aqui, e por onde controlamos, o hablitar, desabiliatr do VBE, comentando descomentando o RET	
	 ;ret	
;   Define modo de video 
    
    	xor ebx,ebx
	mov bx,word[vesa_var0]
    	or bx, 0x4000
    	call vbe_define_mode
    	cmp ax,0x004F
    	jne vesa_error


    	ret




; Obtém infos, sobre o cartão VBE
.vbe_infos:
    	xor eax,eax    
    	push di
    	push es
    	mov ax,0x4F00
    	mov di,LDSBASE
    	mov dword[di],"VBE2"
    	int 0x10
    	pop es
    	pop di
    	ret




; Obtém infos, do modo VBE
; cx, numero de modo
vbe_info_mode:
    	xor eax,eax
	mov ax,0
	mov es,ax    
    	push di
    	push es
    	mov ax,0x4F01
    	mov di,vbe_mode_info_block
    	int 0x10
    	pop es
    	pop di
    	ret

; Define modo de vídeo
; bx numero de modo
vbe_define_mode:
    	xor eax,eax  
    	push di
    	push es
    	mov ax,0x4F02
    	int 0x10
    	pop es
    	pop di
    	ret


vesa_error:
	mov si,str_vesa_error  
    	call print
	xor ax,ax
	int 0x16
    	ret
