NASM    = nasm

.PHONY: stage0.bin

stage0.bin: stage0.asm
	$(NASM) -f bin -o $@ $<
	mv *.bin ../../bin
