ASFLAGS64  :=-f elf64 -O0
CFLAGS64 :=-Wall -O0 -m64 -std=c11 -ffreestanding -mcmodel=large -mno-red-zone -fno-stack-protector \
-nostdlib -nostdinc -I ./include

LDFLAGS64  :=-m elf_x86_64 -Map kernel.map -T link.ld
AS=nasm
CC=gcc
LD=ld


GUI = gui.o compositor.o font8x16.o
FILE = std.o cfs.o pipe.o socket.o
DRIVER = pci.o  i965.o cursor.o acpi.o hpet.o lpc.o pit.o ps2.o mouse.o keyboard.o ahci.o ide.o \
storage.o rtc.o hda.o speaker.o e1000.o ethernet.o arp.o udp.o ipv4.o dhcp.o tcp.o checksum.o
LIB = string.o vsprintf.o stdio.o stdlib.o
OBJ = setup.o main.o io.o gdt.o tss.o idt.o vetor.o exception.o data.o server.o smp.o delay.o \
paging.o mm.o cpuid.o msr.o lvt.o irq.o apic.o ioapic.o intr.o sleep.o thread.o exectve.o syscall.o asyscall.o \
console.o exit.o sse.o $(DRIVER) $(GUI) $(FILE) $(LIB) sse_memcpy.o


.PHONY: all kernel.bin ap.bin a.bin
all: kernel.bin ap.bin
	
kernel.bin: $(OBJ)
	$(LD) $(LDFLAGS64) -o $@ $^
	mv *.bin ../bin
	rm *.o

%.o: %.asm
	$(AS) $(ASFLAGS64) $< -o $@
	
%.o: %.c
	$(CC) $(CFLAGS64) -c -o $@ $<

%.o: lib/%.c
	$(CC) $(CFLAGS64) -c -o $@ $<

%.o: gui/%.c
	$(CC) $(CFLAGS64) -c -o $@ $<

%.o: driver/%.c
	$(CC) $(CFLAGS64) -c -o $@ $<
		
%.o: driver/intel/%.c
	$(CC) $(CFLAGS64) -c -o $@ $<
	
%.o: driver/acpi/%.c
	$(CC) $(CFLAGS64) -c -o $@ $<

%.o: driver/pit/%.c
	$(CC) $(CFLAGS64) -c -o $@ $<
	
%.o: driver/ps2/%.c
	$(CC) $(CFLAGS64) -c -o $@ $<
	
%.o: driver/storage/%.c
	$(CC) $(CFLAGS64) -c -o $@ $<

%.o: driver/rtc/%.c
	$(CC) $(CFLAGS64) -c -o $@ $<
	
%.o: driver/audio/%.c
	$(CC) $(CFLAGS64) -c -o $@ $<

%.o: driver/net/%.c
	$(CC) $(CFLAGS64) -c -o $@ $<
	
%.o: file/%.c
	$(CC) $(CFLAGS64) -c -o $@ $<

ap.bin: 
	nasm -f bin ap.asm -o ap.bin
	mv ap.bin ../bin
