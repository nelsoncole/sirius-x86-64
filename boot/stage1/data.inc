LDSBASE equ 0x600
LDSPARAMETERSBASE equ 0x4000
LDSVESABASE equ 0x4020

strerror db "BOOTLOADER ERROR, STAGE 1",0
strcmd1 db "Sirius Boot Manager Version 2.0",10,13,0
strcmd2 db "BM-SHELL:/> ",0

cmdinstall db "install system",0
cmdboot db "boot",0

dv_num db 0
uid dd 0
dst_dv_num db 0
dap times 16 db 0
dap2 times 16 db 0
vbe_mode_info_block: times 512 db 0

; partition

super_block_reserved dd 0
super_block_byte_per_sector dd 0
super_block_sector_por_blk dd 0
super_block_root_blk dd 0

; outro
data_sector dd 0
first_sector dd 0
first_blk dd 0
address_offset dw 0

file_sector_count dw 0
file_filename dw 0
file_address_di dw 0
file_address_bx dw 0
filesystem_address dw 0

filename  db "stage2.bin                                                                                      ",0 ;96 bytes
filename2 db "kernel.bin                                                                                      ",0 ;96 bytes
filename3 db "ap.bin                                                                                          ",0 ;96 bytes
