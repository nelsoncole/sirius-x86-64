[bits 16]
[org 0x8000]

	jmp 0:start
	
	%include "data.inc"
	%include "print.asm"
	%include "getchar.asm"
	%include "read.asm"
	%include "getpm.asm"
	%include "a20.asm"
	%include "vesa.asm"
	%include "edid.asm"
	%include "gdt.asm"
	%include "strcmp.asm"
	%include "cmd.asm"

start:
	cli
	mov ax,cs
	mov ds,ax
	mov es,ax
	mov ax,0x9000 
	mov ss,ax  ; stack = 0x90000
	xor ax,ax
	mov sp,ax
	sti

; Salvar informações de disco
	mov byte[dv_num],dl
	mov dword[uid], ebx
	mov byte[dst_dv_num], 0x81
	
	
; cmd
	call cmd
	cli
	
; BIOSes Get Device Parameters
	call GetDeviceParameters

; Habilita Gate A20
	call gatea20

; Testa a Gate A20 

; Carrega uid

	mov dword[uid], 0

; Carregar o setup ou stage2.bin


	mov word[file_sector_count], 0

	mov di, filename
	mov word[file_filename],di
	mov word[filesystem_address],0x1000
	mov word[file_address_di],0x1000
	mov word[file_address_bx],0
	
	mov word[address_offset],0
	
	call file_read
	cmp ax,0
	je ._ok
	mov si, strerror
	call print
	
	xor ax,ax
	int 0x16
	int 0x19
._ok:
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; Carregar ap.bin
;
	mov word[file_sector_count], 0
	
	mov di, filename3
	mov word[file_filename],di
	
	mov word[filesystem_address],0x1000
	mov word[file_address_di],0x4000
	mov word[file_address_bx],0
	
	mov word[address_offset],0
	
	call file_read
	cmp ax,0
	je ._ok3
	mov si, strerror
	call print
	
	xor ax,ax
	int 0x16
	int 0x19
._ok3:
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;
;
; Carregar o kernel.bin, NOTA: esta solução é provisória.
;
;
	mov word[file_sector_count], 0
	;jmp ._ok2 ; controla a leitura do kernel em modo bios
	mov di, filename2
	mov word[file_filename],di
	
	mov word[filesystem_address],0x1000
	mov word[file_address_di],0x5000
	mov word[file_address_bx],0
	
	mov word[address_offset],0
	
	call file_read
	cmp ax,0
	je ._ok2
	mov si, strerror
	call print
	
	xor ax,ax
	int 0x16
	int 0x19
._ok2:


;
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; Definir o modo VESA
	call edid
	call vesa_vbe_mode
	
; Mascara interrupções e desabilita NMI
	
	cli
	in al,0x70
	or al,0x80
	out 0x70,al

; Instala a GDT
    mov eax,gdtr
	db 0x66		; Execute GDT 32-Bits
	lgdt [eax]

; Habilita o PE 
	mov eax,cr0
	or eax,1
	mov cr0,eax

; Pula para modo protegido

	xor edx,edx
	mov dl,byte[dv_num]
	mov ebx,dword[uid]
	
	xor ecx,ecx
	mov cx, word[file_sector_count]
	
	jmp dword 0x8:ModoProtegido	; aqui vamos pular do stage1 para o setup
	
bits 32
print32:
	xor edi,edi
	pushad
	mov edi,[_vmm]
.next:
	lodsb
	cmp al,0
	jz .end
	mov ah,0x2
	cld
	stosw
	jmp .next
.end:
	popad
	ret

_vmm dd 0xb8000

ModoProtegido:
    	mov eax,0x10
	mov ds,ax
	mov es,ax
	mov fs,ax
	mov gs,ax
	mov ss,ax
	mov esp, 0x90000
	mov ebp, 0
	
	mov eax, 0x11000

	push ebx
	push ecx
	push edx
	push ss
	push esp
	pushf
	push cs
    	push eax
	iretd
		
	hlt
