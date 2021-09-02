gdt:

;0x0 Entrada Nula, em um GDT a primeira entrada é sempre nula

	dw 0		; Limit
	dw 0		; Base 15:00
	db 0		; Base 23:16
	db 0		; Flags
	db 0		; Flags e Limit 19:16
	db 0		; Base 31:24

; 0x8 CS LIMIT 0xFFFFF, BASE 0x00000000, DPL=0, G=1, S=1 code ou data

	dw 0xFFFF   	; Limit
	dw 0        	; Base 15:00
	db 0        	; Base 23:16
	db 0x9A     	; Flags
	db 0xCF     	; Flags e Limit 19:16
	db 0	    	; Base 31:24
	

; 0x10 DS LIMIT 0xFFFFF, BASE 0x00000000, DPL=0, G=1, S=1 code ou data

	dw 0xFFFF	; Limit
	dw 0		; Base 15:00
	db 0		; Base 23:16
	db 0x92	  	; Flags
	db 0xCF        	; Flags e Limit 19:16
	db 0           	; Base 31:24

;0x18 ;SEGMENTO DE CODIGO (CS 16-BITS) DPL 0, LIMIT 0XFFFF BASE 0X00000000 G 0 1B

	dw 0xFFFF
	dw 0x0
	db 0x0
	db 0x9E
	db 0x0
	db 0x0

;0x20 SEGMENTO DE DADOS (CD 16-BITS)  LIMIT 0XFFFF BASE 0X00000000 G 0 1B

	dw 0xFFFF
	dw 0x0
	db 0x0
	db 0x92
	db 0x0
	db 0x0


gdt_end:

; Crindo nosso ponteiro gdtr

gdtr:
	dw gdt_end - gdt -1	; LIMIT
	dd gdt			; BASE
