[bits 16]
[org 0x7c00]
	cli
	mov ax,cs
	mov ds,ax
	xor ax,ax
    mov ss,ax
	mov sp,0x700
	jmp 0:start
	
	dv_num db 0
	reserved dd 0
	byte_per_sector dd 0
	sector_por_blk dd 0
	size_of_blk dd 0
	root_blk dd 0
	data_sector dd 0
	root_dir dd 0
	
	strerr db "BOOTLOADER ERROR, STAGE 0",0xd,0xa,0
	dap times 16 db 0;
	
	
start:
	sti
	mov byte[dv_num],dl
	mov ax,3
	int 0x10
	
;Verificar ext BIOS EDDs

	mov bx,0x55aa
	mov dl,byte[dv_num]
	mov ah,0x41
	int 0x13
	jc err
	cmp bx,0xaa55
	jne err
	
	
;Ler o super bloco	
	xor di,di
	mov bx,0x7e00
	mov eax,1
	call read
	
	mov esi,0x7e00
	mov eax,[esi+24]
	mov dword[reserved],eax
	
	mov eax,[esi+28]
	mov dword[byte_per_sector],eax
	
	mov eax,[esi+32]
	mov dword[sector_por_blk],eax
	
	mov eax,[esi+40]
	mov dword[size_of_blk],eax
	
	mov eax,[esi+44]
	mov dword[root_blk],eax
	
; calculando o data_sector

	mov eax, dword[reserved]
	mov dword[data_sector],eax

; calculando o root_dir
	mov eax,dword[root_blk]
	mul dword[sector_por_blk]	
	add eax,dword[data_sector]
	mov dword[root_dir],eax

;Ler o primeiro sector do root_dir	
	xor di,di
	mov bx,0x7e00
	mov eax,dword[root_dir]
	call read
	mov esi,0x7e00
	
; Ler o stage1
	mov eax,dword[esi+107]
	mul dword[sector_por_blk]
	add eax,dword[data_sector]
	
	mov ecx,eax	

	mov eax, dword[esi+111];
	xor edx,edx
	div dword[byte_per_sector]
	add eax,1
	
	xchg eax,ecx
	
	xor di,di
	mov bx,0x8000
loading:	
	
	call read
	add eax,1
	add bx,word[byte_per_sector]
	loop loading
	
	
; Execute o stage1
	xor dx,dx
	mov dl,byte[dv_num]
	mov ebx,dword[uid]
	
	push ebx
	push dx
	jmp 0:0x8000
	

err:
	mov si,strerr
	call print
	xor ax,ax
	int 0x16
	int 0x19
	
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

read:
	; DI --> Segmento
	; BX --> Offset
	; EAX --> Starting sector LBA47
	
	pushad
	mov si,dap
	mov [si + 0], byte 0x10	; Tamanho da DAP
	mov [si + 1], byte 0		; reservado
	mov [si + 2], word 1		; Sector count
	mov [si + 4], BX		; Offset
	mov [si + 6], DI		; Segment
	mov [si + 8], EAX		; Start sector LBA47
	mov [si +12], dword 0		; Starting sector LBA47
	mov dl, byte [dv_num]
	mov ah, 0x42
	int 0x13
	jc  err_
	popad
	ret

err_:	
	popad
	jmp err

times (0x1b8 - ($-$$)) nop

uid times 0x04 db 0x80		; Unique Disk ID
rsv times 0x02 db 0		    ; Optional, reserved 0x0000

; First Partition Entry
pt1:
.flag:      db 0x80
.startH:    db 0
.startC:    db 0
.startS:    db 0

.osType:    db 0xEF     ; efi
.endH:      db 0
.endC:      db 0
.endS:      db 0

.startLBA:  dd 1

.partitionSize: dd 65536    ; in sectors. almost 32 mb

pt2 times 0x10 db 0        	; Second Partition Entry
pt3 times 0x10 db 0         ; Third Partition Entry
pt4 times 0x10 db 0         ; Fourth Partition Entry

times (510 - ($-$$)) nop
dw 0xaa55
