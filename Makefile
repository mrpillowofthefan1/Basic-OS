all: run

kernel.bin: kernel-entry.o kernel.o
	x86_64-elf-ld -m elf_i386 -o $@ -Ttext 0x1000 $^ --oformat binary

kernel-entry.o: kernel-entry.asm
	nasm $< -f elf32 -o $@

kernel.o: kernel.c
	x86_64-elf-gcc -m32 -ffreestanding -Wall -Wextra -nostdlib -c $< -o $@

mbr.bin: mbr.asm
	nasm $< -f bin -o $@

os-image.bin: mbr.bin kernel.bin
	cat $^ > $@

run: os-image.bin
	qemu-system-i386 -fda $<

clean:
	rm -f *.bin *.o *.dis
