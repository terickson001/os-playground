CC = /usr/local/i386elfgcc/bin/i386-elf-gcc

ODIR = obj

SRCS = $(wildcard kernel/*.c drivers/*.c cpu/*.c libc/*.c)
DEPS = $(wildcard kernel/*.h drivers/*.h cpu/*.h libc/*.h)

OBJS=$(addprefix $(ODIR)/, $(notdir $(SRCS:.c=.o)))

CFLAGS = -g -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -Wall -Wextra -Werror

os-image.bin: boot/boot.bin kernel.bin
	cat $^ > os-image.bin

kernel.bin: ${ODIR}/kernel_entry.o ${ODIR}/interrupt.o ${OBJS} 
	i386-elf-ld -o $@ -Ttext 0x1000 $^ --oformat binary

kernel.elf: ${ODIR}/kernel_entry.o ${ODIR}/interrupt.o ${OBJS}
	i386-elf-ld -o $@ -Ttext 0x1000 $^ 

run: os-image.bin
	qemu-system-i386 -fda os-image.bin

debug: os-image.bin kernel.elf
	qemu-system-i386 -s -S -fda os-image.bin -d guest_errors,int &
	gdb -ex "target remote localhost:1234" -ex "symbol-file kernel.elf"

${ODIR}/%.o: kernel/%.c ${DEPS}
	${CC} ${CFLAGS} -ffreestanding -c $< -o $@
${ODIR}/%.o: drivers/%.c ${DEPS}
	${CC} ${CFLAGS} -ffreestanding -c $< -o $@
${ODIR}/%.o: cpu/%.c ${DEPS}
	${CC} ${CFLAGS} -ffreestanding -c $< -o $@
${ODIR}/%.o: libc/%.c ${DEPS}
	${CC} ${CFLAGS} -ffreestanding -c $< -o $@
${ODIR}/interrupt.o: cpu/interrupt.asm
	nasm $< -f elf -o $@

${ODIR}/%.o: boot/%.asm
	nasm $< -f elf -o $@

%.bin: %.asm
	nasm $< -f bin -o $@

clean:
	rm -rf *.bin *.dis ${ODIR}/*.o os-image.bin *.elf
