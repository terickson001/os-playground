CC = /usr/local/i386elfgcc/bin/i386-elf-gcc

ODIR = obj

VPATH = kernel:drivers:cpu:libc

SRCS = $(filter-out $(wildcard boot/*.asm), $(wildcard **/*.c **/*.asm))
DEPS = $(wildcard **/*.h)

OBJS=$(addprefix $(ODIR)/, $(patsubst %.c,%.o,$(patsubst %.asm,%.o,$(notdir $(SRCS)))))

CFLAGS = -g -m32 -fno-builtin -fno-exceptions -Wall -Wextra

os-image.bin: boot/boot.bin kernel.bin
	cat $^ > os-image.bin

kernel.bin: ${ODIR}/kernel_entry.o ${OBJS} 
	i386-elf-ld -o $@ -T linker.ld $^ --oformat binary

kernel.elf: ${ODIR}/kernel_entry.o ${OBJS}
	i386-elf-ld -o $@ -T linker.ld $^ 

run: os-image.bin
	qemu-system-i386 -drive file=os-image.bin,format=raw,if=floppy

debug: os-image.bin kernel.elf
	qemu-system-i386 -s -S -drive file=os-image.bin,format=raw,if=floppy -d guest_errors,int &
	gdb -ex "target remote localhost:1234" -ex "symbol-file kernel.elf"


${ODIR}/%.o: %.c ${DEPS}
	${CC} ${CFLAGS} -ffreestanding -c $< -o $@

${ODIR}/%.o: %.asm
	nasm $< -f elf -o $@


${ODIR}/kernel_entry.o: boot/kernel_entry.asm
	nasm $< -f elf -o $@
boot.bin: boot/%.asm
	nasm $< -f bin -o $@

clean:
	rm -rf *.bin *.dis ${ODIR}/*.o os-image.bin *.elf
