NASM    = nasm

.PHONY: stage1.bin

stage1.bin: stage1.asm
	$(NASM) -f bin -o $@ $<
	mv *.bin ../../bin
