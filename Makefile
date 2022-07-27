AS      = as
LD      = ld
CC      = gcc
AR      = ar
MAKE    = make
NASM    = nasm

.PHONY: all stage0 stage1 stage2 kernel mout_vhd clean test test2 fs footer vbox_install install push user
all: stage0 stage1 stage2 kernel user fs footer mout_vhd #test clean
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
	dd if=/dev/zero of=Sirius.vhd bs=512 count=65537 conv=noerror,sync
	./fs -f Sirius.vhd
	./fs -g bin/stage1.bin Sirius.vhd
	./fs -p bin Sirius.vhd
	./fs -p system Sirius.vhd
	./fs -p user Sirius.vhd
	./fs -g bin/stage2.bin Sirius.vhd
	./fs -g bin/kernel.bin Sirius.vhd
	./fs -g bin/ap.bin Sirius.vhd
	./fs -g README.md Sirius.vhd
	#./fs -g font/font.psf disk.vhd
	./fs -g bin/term.bin Sirius.vhd
	./fs -g bin/shell.bin Sirius.vhd
	./fs -g bin/launcher.bin Sirius.vhd
	./fs -g bin/explore.bin Sirius.vhd
	./fs -g bin/editor.bin Sirius.vhd
	./fs -g bmp/logo.bmp Sirius.vhd
	./fs -g bmp/folder.bmp Sirius.vhd
	./fs -g bmp/console.bmp Sirius.vhd
	./fs -g bmp/trm.bmp Sirius.vhd
	./fs -g bmp/edit.bmp Sirius.vhd
	./fs -g bmp/file.bmp Sirius.vhd
	./fs -g bmp/text.bmp Sirius.vhd
	./fs -g bin/test.bin Sirius.vhd
	./fs -g bin/telnet.bin Sirius.vhd
	./fs -g bin/chat.bin Sirius.vhd
	./fs -g bin/pci.bin Sirius.vhd
	./fs -g bin/cat.bin Sirius.vhd
	./fs -g bin/lua.bin Sirius.vhd
	./fs -g bin/nanojpeg.bin Sirius.vhd
	./fs -g bmp/w.ppm Sirius.vhd
	#./fs -g bmp/b.ppm Sirius.vhd
	./fs -g bmp/w.jpg Sirius.vhd
	./fs -g bmp/ubuntu.ttf Sirius.vhd
	./fs -g test.lua Sirius.vhd
	#./fs -g beep/beep.wav Sirius.vhd
	#./fs -g arquivo.txt Sirius.vhd
	#./fs -g bin/a.bin Sirius.vhd
	./footer footer.data Sirius.vhd
	

clean:
	rm bin/*.bin
	rm fs
	rm copy
	rm footer.data
	rm footer
	rm Sirius.vhd

test:
	qemu-system-x86_64 -m 512 \
	-smp 2 -s -machine q35 -enable-kvm -cpu host -drive file=Sirius.vhd,format=raw,bus=0 \
	-device ich9-intel-hda -device hda-duplex -net nic,model=pcnet -net user #-net nic,model=e1000 -net user
test2:
	qemu-system-x86_64 -smp 2 -m 512 \
	-s -machine q35 -enable-kvm -cpu host -trace enable=usb* -device usb-ehci,id=ehci \
	-drive if=none,id=usbstick,file=Sirius.vhd -device usb-storage,bus=ehci.0,drive=usbstick
	
fs:
	gcc -Wall fs.c -o fs
	gcc -Wall copy.c -o copy

footer:
	$(NASM) -f bin footer.asm -o footer.data
	gcc -Wall footer.c -o footer 
	
vbox_install:
	./copy Sirius.vhd "/home/nelson/VirtualBox VMs/sirius/"*.vhd
	
install:
	sudo ./copy Sirius.vhd /dev/sdb
	
push:
	git add ./
	git commit -m "Época de férias"	
	git push -u origin main
	
