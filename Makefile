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
	dd if=/dev/zero of=SIRIUS.VHD bs=512 count=65537 conv=noerror,sync
	./fs -f SIRIUS.VHD
	./fs -g bin/stage1.bin SIRIUS.VHD
	./fs -p bin SIRIUS.VHD
	./fs -p system SIRIUS.VHD
	./fs -p user SIRIUS.VHD
	./fs -g bin/stage2.bin SIRIUS.VHD
	./fs -g bin/kernel.bin SIRIUS.VHD
	./fs -g bin/ap.bin SIRIUS.VHD
	./fs -g README.md SIRIUS.VHD
	#./fs -g font/font.psf SIRIUS.VHD    
	./fs -g bin/windowX.bin SIRIUS.VHD
	./fs -g bin/term.bin SIRIUS.VHD
	./fs -g bin/shell.bin SIRIUS.VHD
	./fs -g bin/launcher.bin SIRIUS.VHD
	./fs -g bin/explore.bin SIRIUS.VHD
	./fs -g bin/editor.bin SIRIUS.VHD
	./fs -g bmp/logo.bmp SIRIUS.VHD
	./fs -g bmp/folder.bmp SIRIUS.VHD
	./fs -g bmp/console.bmp SIRIUS.VHD
	./fs -g bmp/trm.bmp SIRIUS.VHD
	./fs -g bmp/edit.bmp SIRIUS.VHD
	./fs -g bmp/file.bmp SIRIUS.VHD
	./fs -g bmp/text.bmp SIRIUS.VHD
	./fs -g bin/test.bin SIRIUS.VHD
	./fs -g bin/telnet.bin SIRIUS.VHD
	./fs -g bin/chat.bin SIRIUS.VHD
	./fs -g bin/pci.bin SIRIUS.VHD
	./fs -g bin/cat.bin SIRIUS.VHD
	./fs -g bin/lua.bin SIRIUS.VHD
	./fs -g bin/nanojpeg.bin SIRIUS.VHD
	./fs -g bmp/w.ppm SIRIUS.VHD
	#./fs -g bmp/b.ppm SIRIUS.VHD
	./fs -g bmp/w.jpg SIRIUS.VHD
	./fs -g bmp/ubuntu.ttf SIRIUS.VHD
	./fs -g test.lua SIRIUS.VHD
	#./fs -g beep/beep.wav SIRIUS.VHD
	#./fs -g arquivo.txt SIRIUS.VHD
	#./fs -g bin/a.bin SIRIUS.VHD
	./footer footer.data SIRIUS.VHD
	

clean:
	rm bin/*.bin
	rm fs
	rm copy
	rm footer.data
	rm footer
	rm SIRIUS.VHD

test:
	qemu-system-x86_64 -m 512 \
	-smp 2 -s -machine q35 -enable-kvm -cpu host -drive file=SIRIUS.VHD,format=raw,bus=0 \
	-device ich9-intel-hda -device hda-duplex -net nic,model=pcnet -net user #-net nic,model=e1000 -net user
test2:
	qemu-system-x86_64 -smp 2 -m 512 \
	-s -machine q35 -enable-kvm -cpu host -trace enable=usb* -device usb-ehci,id=ehci \
	-drive if=none,id=usbstick,file=SIRIUS.VHD -device usb-storage,bus=ehci.0,drive=usbstick
	
fs:
	gcc -Wall fs.c -o fs
	gcc -Wall copy.c -o copy

footer:
	$(NASM) -f bin footer.asm -o footer.data
	gcc -Wall footer.c -o footer 
	
vbox_install:
	./copy SIRIUS.VHD "/home/nelson/VirtualBox VMs/sirius/"*.vhd
	
install:
	sudo ./copy SIRIUS.VHD /dev/sdb
	
push:
	git add ./
	git commit -m "Revisão básica"	
	git push -u origin main
	
