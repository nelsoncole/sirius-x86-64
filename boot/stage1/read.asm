
strerr_read db "READ ERROR",0xa,0xd,0
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
	jc  .err
	popad
	ret

.err:	
	popad
	mov si,strerr_read
	call print
	ret
	
	
strerr_write db "WRITHE ERROR",0xa,0xd,0
write:
	; DI --> Segmento
	; BX --> Offset
	; EAX --> Starting sector LBA47
	
	pushad
	mov si,dap2
	mov [si + 0], byte 0x10	; Tamanho da DAP
	mov [si + 1], byte 0		; reservado
	mov [si + 2], word 1		; Sector count
	mov [si + 4], BX		; Offset
	mov [si + 6], DI		; Segment
	mov [si + 8], EAX		; Start sector LBA47
	mov [si +12], dword 0		; Starting sector LBA47
	mov dl, byte [dst_dv_num]
	mov al, 0
	mov ah, 0x43
	int 0x13
	jc  .err
	popad
	ret

.err:	
	popad
	mov si,strerr_write
	call print
	ret
	
	
read_mul:
	
	call read
	add eax,1
	add bx,word[super_block_byte_per_sector]
	loop read_mul
	
	ret
	
file_read:

; carregar o super bloco na memorio 0x7e00
	xor di,di
	mov bx,0x7e00
	mov eax, 1	;Sector 1
	call read
	

	mov esi,0x7e00
	mov eax,[esi + 24]
	mov dword[super_block_reserved],eax
	mov eax,[esi + 28]
	mov dword[super_block_byte_per_sector],eax
	mov eax,[esi + 32]
	mov dword[super_block_sector_por_blk],eax
	mov eax,dword[esi + 44]
	mov dword[super_block_root_blk],eax
	
; Carregar o directorio raiz
; calculando o data_sector

	mov eax, dword[super_block_reserved]
	mov dword[data_sector],eax
	
; calculando o root_dir
	mov eax,dword[super_block_root_blk]
	mul dword[super_block_sector_por_blk]	
	add eax,dword[data_sector]
	mov dword[first_sector],eax

; ler o bloco do directorio raiz = 8KiB por default
	xor di,di
	mov bx,word[filesystem_address]
	mov eax, dword[first_sector] ; root_sector
	mov ecx, dword[super_block_sector_por_blk]
	call read_mul

; pesquisar o arquivo stage2.bin
	
	xor dx,dx
	mov si,word[filesystem_address]
	mov di,word[file_filename]
	mov bx, 64
	xor cx,cx
._1:	
	mov al,byte[si]
	mov ah,byte[di]
	cmp al,ah
	jnz .next_entry
	inc cx
	inc si
	inc di
	cmp cx,96
	je ._sucessfull
	jmp ._1


.next_entry:
	xor cx,cx
	add dx,128
	mov di,word[file_filename]
	mov si,word[filesystem_address]
	add si,dx
	cmp bx,0
	je ._error
	sub bx,1
	xor cx,cx
	jmp ._1
._error:
	mov ax, 1
	ret
	
._sucessfull:

	mov si,word[filesystem_address]
	add si,dx
	mov eax,[si+107]
	mov dword[first_blk],eax
	
; ler o arquivo
; eax = LBA, ecx = count
._next_blk:

	add word[file_sector_count],1	
	mov eax,dword[first_blk]
	mul dword[super_block_sector_por_blk]
	add eax,dword[data_sector]
	
	mov ecx, dword[super_block_sector_por_blk]	

	mov di,word[file_address_di]
	mov bx,word[file_address_bx]
	add bx,word[address_offset]
	call read_mul
	
	mov ax,word[super_block_sector_por_blk]
	mul word[super_block_byte_per_sector]
	
	mov bx,0xF000
	sub bx,ax
	add bx,0x1000
	
	cmp word[address_offset],bx
	je .new_offset
	
	add word[address_offset], 0x2000
	jmp .cur_offset
.new_offset:

	mov word[address_offset],0
	add word[file_address_di],0x1000
	
.cur_offset:
	
	
; verificar eof
	
	mov eax, 4
	mul dword[first_blk]
	xor edx,edx
	div dword[super_block_byte_per_sector]
	add eax,dword[super_block_reserved]
	
; carregar o 1 sector da area BLK[] 
	xor di,di
	mov bx,0x7e00
	call read
	
	xor edx,edx
	mov eax, dword[super_block_byte_per_sector]
	mov ecx, 4
	div ecx
	
	xor edx,edx
	mov ecx,eax
	mov eax,dword[first_blk]
	div ecx	
	
	mov ax, 4
	mul dx
	
	mov si,0x7e00
	add si,ax
	mov eax,[si]
	
	mov dword[first_blk],eax
	cmp eax, -1
	je ._end
	jmp ._next_blk
._end:			
	xor ax, ax
	ret
	
