ASFLAGS64  :=-f elf64 -O0
CFLAGS64 :=-Wall -O0 -m64 -std=c11 -ffreestanding -mcmodel=large -mno-red-zone \
-nostdinc -fno-stack-protector -I ./gui/include -I ./lib/libc/include -I ./lib/sys/include #-mno-mmx -mno-sse -mno-sse2


LDFLAGS64  :=-m elf_x86_64 -T lib/link.ld -Map app.map
AS=nasm
CC=gcc
LD=ld

net = dns.o
sys = file.o cfs.o alloc.o pipe.o path.o socket.o inet.o $(net)
time = time.o gmtime.o localtime.o strftime.o clock.o
errno = errno.o 
stdlib = itoa.o i2hex.o malloc.o calloc.o free.o realloc.o div.o ldiv.o lldiv.o strtol.o atoi.o atol.o strtoul.o\
	exit.o getenv.o qsort.o system.o strtod.o ftoa.o atof.o strtof.o strtold.o abs.o ltoa.o utoa.o srand.o rand.o
	
stdio = stdio.o fopen.o fclose.o fflush.o putc.o getc.o fputc.o fgetc.o fgets.o fputs.o fread.o fwrite.o \
	remove.o rewind.o fseek.o feof.o ftell.o vfprintf.o fprintf.o vsprintf.o sprintf.o snprintf.o vsnprintf.o \
	putchar.o puts.o printf.o sscanf.o perror.o vfscanf.o fscanf.o scanf.o ungetc.o getchar.o freopen.o tmpnam.o \
	rename.o
ctype = ctype.o tolower.o toupper.o
string = memcmp.o memcpy.o memset.o strcasecmp.o strcat.o strchr.o strcmp.o strcpy.o strcspn.o string.o \
	strlen.o strncasecmp.o strncmp.o strncpy.o strpbrk.o strrchr.o strsep.o strspn.o strtok.o strstr.o \
	strdup.o memmove.o strcoll.o strerror.o
math = math.o pow.o sqrt.o atan.o acos.o asin.o sin.o tan.o cos.o floor.o ceil.o exp.o log.o fmax.o fmin.o fminf.o min.o max.o fabs.o lbm.o
locale = locale.o

signal = signal.o
setjmp = setjmp.o longjmp.o

GUI = gui.o font8x16.o window.o bmp.o font.o border.o
GUI2 = editbox.o mouse.o menubox.o obj.o message.o dialog.o button.o listbox.o terminalbox.o

HEADER = setup.o crt0.o

LUA_OBJS= hash.o inout.o lex_yy.o opcode.o table.o y_tab.o lua.o iolib.o mathlib.o strlib.o

LUA_Z = lua.o lapi.o lcode.o lctype.o ldebug.o ldo.o ldump.o lfunc.o lgc.o llex.o lmem.o lobject.o \
	lopcodes.o lparser.o lstate.o lstring.o ltable.o ltm.o lundump.o lvm.o lzio.o lauxlib.o \
	lbaselib.o lcorolib.o ldblib.o liolib.o lmathlib.o loadlib.o loslib.o lstrlib.o ltablib.o \
	lutf8lib.o linit.o  #luac.o
	
LUA_Z_OBJS=	\
	lapi.o \
	lauxlib.o \
	lbuffer.o \
	lbuiltin.o \
	ldo.o \
	lfunc.o \
	lgc.o \
	llex.o \
	lmem.o \
	lobject.o \
	lparser.o \
	lstate.o \
	lstring.o \
	ltable.o \
	ltm.o \
	lundump.o \
	lvm.o \
	lzio.o \
	linit.o \
	ldblib.o \
	liolib.o \
	lstrlib.o \
	lmathlib.o

NASM_OBJS = nasm.o nasmlib.o ver.o raa.o saa.o rbtree.o float.o insnsa.o insnsb.o assemble.o labels.o hashtbl.o crc64.o parser.o outform.o outlib.o nulldbg.o outbin.o outaout.o outcoff.o outelf.o outelf32.o outelf64.o outobj.o outas86.o outrdf2.o outdbg.o outieee.o outmacho32.o outmacho64.o preproc.o quote.o pptok.o macros.o listing.o eval.o exprlib.o stdscan.o strfunc.o tokhash.o regvals.o regflags.o

LIBC = $(string) $(ctype) $(stdio) $(stdlib) $(errno) $(time) $(math) $(locale) $(signal) $(setjmp)

LAUNCHER = $(HEADER) launcher.o wallpaper.o $(GUI)  $(sys) $(LIBC)

TERM = $(HEADER) term.o $(GUI) $(GUI2) $(sys) $(LIBC)

_SHELL = $(HEADER) shell.o dir.o $(GUI) $(sys) $(LIBC)

EXPLORE = $(HEADER) explore.o $(sys) $(GUI) $(GUI2) $(LIBC)

EDITOR = $(HEADER) editor.o $(GUI) $(GUI2) $(sys) $(LIBC)


PCI = $(HEADER) pci.o $(sys) $(GUI) $(LIBC)

TEST = $(HEADER) test.o $(sys) $(GUI) $(LIBC)

CHAT = $(HEADER) chat.o $(sys) $(GUI) $(LIBC)

CAT = $(HEADER) cat.o $(sys) $(GUI) $(LIBC)

LUA = $(HEADER) $(LUA_OBJS) $(sys) $(GUI) $(LIBC) 

#LUA = $(HEADER) lua.o $(LUA_Z_OBJS) $(sys) $(GUI) $(LIBC) 

NANOJPEG = $(HEADER) nanojpeg.o $(sys) $(GUI) $(LIBC) 

NASM = $(HEADER) $(NASM_OBJS) $(sys) $(GUI) $(LIBC)

.PHONY: all term.bin shell.bin explore.bin editor.bin cat.bin test.bin chat.bin lua.bin nasm.bin nanojpeg.bin pci.bin png
all: launcher.bin term.bin shell.bin explore.bin editor.bin cat.bin test.bin chat.bin nanojpeg.bin pci.bin #lua.bin

launcher.bin: $(LAUNCHER)
	$(LD) $(LDFLAGS64) -o $@ $^
	#mv *.bin ../bin
	#rm *.o
	
term.bin: $(TERM)
	$(LD) $(LDFLAGS64) -o $@ $^
	#mv *.bin ../bin
	#rm *.o
	
shell.bin: $(_SHELL)
	$(LD) $(LDFLAGS64) -o $@ $^
	#mv *.bin ../bin
	#rm *.o

explore.bin: $(EXPLORE)
	$(LD) $(LDFLAGS64) -o $@ $^
	#mv *.bin ../bin
	#rm *.o
	
editor.bin: $(EDITOR)
	$(LD) $(LDFLAGS64) -o $@ $^
	#mv *.bin ../bin
	#rm *.o
cat.bin: $(CAT)
	$(LD) $(LDFLAGS64) -o $@ $^
	#mv *.bin ../bin
	#rm *.o
	
test.bin: $(TEST)
	$(LD) $(LDFLAGS64) -o $@ $^
	#mv *.bin ../bin
	#rm *.o

chat.bin: $(CHAT)
	$(LD) $(LDFLAGS64) -o $@ $^
	#mv *.bin ../bin
	#rm *.o

pci.bin: $(PCI)
	$(LD) $(LDFLAGS64) -o $@ $^
	mv *.bin ../bin
	rm *.o
	
nasm.bin: $(NASM)
	$(LD) $(LDFLAGS64) -o $@ $^
	#mv *.bin ../bin
	#rm *.o
	
nanojpeg.bin: $(NANOJPEG)
	$(LD) $(LDFLAGS64) -o $@ $^
	#mv *.bin ../bin
	#rm *.o

lua.bin: $(LUA)
	$(LD) $(LDFLAGS64) -o $@ $^
	mv *.bin ../bin
	rm *.o
	
	
%.o: %.asm
	$(AS) $(ASFLAGS64) $< -o $@
	
%.o: lib/%.asm
	$(AS) $(ASFLAGS64) $< -o $@

%.o: lib/libc/setjmp/%.asm
	$(AS) $(ASFLAGS64) $< -o $@

%.o: lib/libc/math/%.asm
	$(AS) $(ASFLAGS64) $< -o $@

%.o: %.c
	$(CC) $(CFLAGS64) -c -o $@ $<
	
%.o: lib/%.c
	$(CC) $(CFLAGS64) -c -o $@ $<

%.o: gui/%.c
	$(CC) $(CFLAGS64) -c -o $@ $<
	
%.o: lib/libc/string/%.c
	$(CC) $(CFLAGS64) -c -o $@ $<
%.o: lib/libc/ctype/%.c
	$(CC) $(CFLAGS64) -c -o $@ $<
%.o: lib/libc/stdio/%.c
	$(CC) $(CFLAGS64) -c -o $@ $<
%.o: lib/libc/stdlib/%.c
	$(CC) $(CFLAGS64) -c -o $@ $<
%.o: lib/libc/errno/%.c
	$(CC) $(CFLAGS64) -c -o $@ $<
%.o: lib/libc/math/%.c
	$(CC) $(CFLAGS64) -c -o $@ $<

%.o: lib/libc/time/%.c
	$(CC) $(CFLAGS64) -c -o $@ $<
	
%.o: lib/libc/locale/%.c
	$(CC) $(CFLAGS64) -c -o $@ $<
	
%.o: lib/libc/signal/%.c
	$(CC) $(CFLAGS64) -c -o $@ $<
	
%.o: lib/libc/setjmp/%.c
	$(CC) $(CFLAGS64) -c -o $@ $<
	
%.o: lib/sys/%.c
	$(CC) $(CFLAGS64) -c -o $@ $<

%.o: lib/sys/socket/%.c
	$(CC) $(CFLAGS64) -c -o $@ $<

%.o: lib/sys/arpa/%.c
	$(CC) $(CFLAGS64) -c -o $@ $<

%.o: lib/sys/net/%.c
	$(CC) $(CFLAGS64) -c -o $@ $<
	
#%.o: lua3/%.c
#	$(CC) $(CFLAGS64) -c -o $@ $<
	
#%.o: lua5/src/%.c
#	$(CC) $(CFLAGS64) -c -o $@ $< 
	
%.o: lua1/%.c
	$(CC) $(CFLAGS64) -c -o $@ $< 
	
NANOJPEG_CFLAGS := -D_NJ_EXAMPLE_PROGRAM
%.o:nanojpeg/%.c
	$(CC) $(CFLAGS64) $(NANOJPEG_CFLAGS) -c -o $@ $<

NASM_CFLAGS =-Wall -O3 -m64 -std=c11 -pedantic -DHAVE_CONFIG_H -W -ffreestanding -mcmodel=large -mno-red-zone \
-nostdlib -nostdinc -fno-stack-protector -I ./gui/include -I ./lib/libc/include -I ./lib/sys/include -I ./nasm
%.o: nasm/%.c
	$(CC) $(NASM_CFLAGS) -c -o $@ $< 
#NASM/OUTPUT
%.o:nasm/output/%.c
	$(CC) $(NASM_CFLAGS) -c -o $@ $<
		
png:	
	$(CC) $(CFLAGS64) png/*.c -c -I ./png -I ./png/zlib
	
