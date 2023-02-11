[bits 16]
.Cookie:
    db 'conectix'
.Features:
    dd 0
.PlatFormVersion:
    dd 0x00001000
.DataOffset:
    dd 0xFFFFFFFF, 0xFFFFFFFF
.TimeStamp:
    dd 0x5868091E
.CreatorApp:
    dd 0x61766D6A
.CreatorVersion:
    dd 0x0A000400
.CreatorOS:
    dd 0x6B326957
.OriginalSize:
    dd 0, 0x00000002
.CurrentSize:
    dd 0, 0x00000002
.DiskGeometry:
    dw 0xC303       ;# Cylinders
    db 0x4          ;# Heads
    db 0x11         ;# Sectors
.DiskType:
    dd 0x02000000   ;# Fixed
.CheckSum:
    dd 0xC2E7FFFF   ;# VHD igual ou acima de 32Mb
.UniqueID:
    db 'S',  'I',  'R',  'I'
    db 'U',  'S',  0x20, 0x20
    db 0x88, 0x88, 0x88, 0x88
    db 0x88, 0x88, 0x88, 0x88
.SaveState:
    db 0
.Reserved:
    times 427 db 0      ;# Padded to 512 bytes?

