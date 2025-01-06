all: run

kernel.bin: kernel-entry.o kernel.o interrupts.o display.o isr.o idt.o ports.o keyboard.o
	x86_64-elf-ld -m elf_i386 -o $@ -Ttext 0x1000 $^ --oformat binary

kernel-entry.o: kernel-entry.asm
	nasm $< -f elf32 -o $@

kernel.o: kernel.c
	x86_64-elf-gcc -m32 -ffreestanding -Wall -Wextra -nostdlib -c $< -o $@

interrupts.o: interrupt.asm
	nasm $< -f elf32 -o $@

ports.o: ports.c
	x86_64-elf-gcc -m32 -ffreestanding -Wall -Wextra -nostdlib -c $< -o $@

display.o: display.c
	x86_64-elf-gcc -m32 -ffreestanding -Wall -Wextra -nostdlib -c $< -o $@

idt.o: idt.c
	x86_64-elf-gcc -m32 -ffreestanding -Wall -Wextra -nostdlib -c $< -o $@

isr.o: isr.c
	x86_64-elf-gcc -m32 -ffreestanding -Wall -Wextra -nostdlib -c $< -o $@


keyboard.o: keyboard.c
	x86_64-elf-gcc -m32 -ffreestanding -Wall -Wextra -nostdlib -c $< -o $@

mbr.bin: mbr.asm
	nasm $< -f bin -o $@

os-image.bin: mbr.bin kernel.bin
	cat $^ > $@

run: os-image.bin
	qemu-system-i386 -fda os-image.bin

clean:
	rm -f *.bin *.o *.dis
