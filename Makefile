AS      = as
LD      = ld
CC      = gcc
AR      = ar
MAKE    = make
NASM    = nasm

.PHONY: all stage0 stage1 stage2 kernel mout_vhd clean test test2 fs vbox_install install push user
all: stage0 stage1 stage2 kernel user fs mout_vhd #test clean
stage0:
	$(MAKE) -C boot/stage0/
stage1:
	$(MAKE) -C boot/stage1/
stage2:
	$(MAKE) -C boot/stage2/
kernel:
	$(MAKE) -C kernel/
user:
	$(MAKE) -C user/
	
mout_vhd:
	dd if=/dev/zero of=disk.vhd bs=1024 count=8192 conv=noerror,sync
	./fs -f disk.vhd
	./fs -g bin/stage1.bin disk.vhd
	./fs -p bin disk.vhd
	./fs -p system disk.vhd
	./fs -p user disk.vhd
	./fs -g bin/stage2.bin disk.vhd
	./fs -g bin/kernel.bin disk.vhd
	./fs -g bin/ap.bin disk.vhd
	./fs -g README.md disk.vhd
	#./fs -g font/font.psf disk.vhd
	./fs -g bin/term.bin disk.vhd
	./fs -g bin/shell.bin disk.vhd
	./fs -g bin/launcher.bin disk.vhd
	./fs -g bin/explore.bin disk.vhd
	./fs -g bin/editor.bin disk.vhd
	./fs -g bmp/logo.bmp disk.vhd
	./fs -g bmp/folder.bmp disk.vhd
	./fs -g bmp/console.bmp disk.vhd
	./fs -g bmp/edit.bmp disk.vhd
	./fs -g bmp/file.bmp disk.vhd
	./fs -g bmp/text.bmp disk.vhd
	./fs -g bin/test.bin disk.vhd
	./fs -g bin/pci.bin disk.vhd
	./fs -g bin/cat.bin disk.vhd
	./fs -g bin/lua.bin disk.vhd
	./fs -g bin/nanojpeg.bin disk.vhd
	./fs -g bmp/w.ppm disk.vhd
	#./fs -g bmp/b.ppm disk.vhd
	./fs -g bmp/w.jpg disk.vhd
	./fs -g bmp/ubuntu.ttf disk.vhd
	./fs -g test.lua disk.vhd
	#./fs -g beep/beep.wav disk.vhd
	#./fs -g arquivo.txt disk.vhd
	#./fs -g bin/a.bin disk.vhd
	

clean:
	rm bin/*.bin
	rm fs
	rm copy
	rm disk.vhd

test:
	qemu-system-x86_64 -m 512 \
	-smp 2 -s -machine q35 -enable-kvm -cpu host -drive file=disk.vhd,format=raw,bus=0 \
	-device ich9-intel-hda -device hda-duplex
test2:
	qemu-system-x86_64 -smp 2 -m 512 \
	-s -machine q35 -enable-kvm -cpu host -trace enable=usb* -device usb-ehci,id=ehci \
	-drive if=none,id=usbstick,file=disk.vhd -device usb-storage,bus=ehci.0,drive=usbstick
	
fs:
	gcc -Wall -C fs.c -o fs
	gcc -Wall -C copy.c -o copy
	
	
vbox_install:
	./copy disk.vhd "/home/nelson/VirtualBox VMs/Kinguio/"*.vhd
	
install:
	sudo ./copy disk.vhd /dev/sdb
	
push:
	git add ./
	git commit -m "commit++"	
	git push -u origin main
	
