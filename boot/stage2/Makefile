ASFLAGS  :=-f elf32 -O0
CFLAGS32 :=-Wall -O0 -m32 -std=c11 -ffreestanding -nostdlib -nostdinc -I ./include
LDFLAGS  :=-m elf_i386 -Map stage2.map -T link.ld
AS=nasm
CC=gcc
LD=ld


USB= ehci.o bulk.o hub.o 

objs= stage2.o main.o data.o stdlib.o string.o gui.o font8x16.o stdio.o vsprintf.o gdt.o idt.o vetor.o exception.o irq.o pci.o ata.o ahci.o storage.o fs.o cpuid.o msr.o paging.o bootblock.o pic.o ps2.o mouse.o keyboard.o $(USB)

.PHONY: stage2.bin
	
stage2.bin: $(objs)
	$(LD) $(LDFLAGS) -o $@ $^
	mv *.bin ../../bin
	rm *.o
#stage2
%.o: %.asm
	$(AS) $(ASFLAGS) $< -o $@
	
%.o: %.c
	$(CC) $(CFLAGS32) -c -o $@ $<

%.o: lib/%.c
	$(CC) $(CFLAGS32) -c -o $@ $<
	
%.o: gui/%.c
	$(CC) $(CFLAGS32) -c -o $@ $<
	
%.o: drive/%.c
	$(CC) $(CFLAGS32) -c -o $@ $<
%.o: drive/usb/%.c
	$(CC) $(CFLAGS32) -c -o $@ $<

%.o: fs/%.c
	$(CC) $(CFLAGS32) -c -o $@ $<
