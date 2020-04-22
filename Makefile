CC = /usr/local/i386elfgcc/bin/i386-elf-gcc
GDB = /usr/local/i386elfgcc/bin/i386-elf-gdb

ODIR = obj

VPATH = boot:kernel:drivers:cpu:libc

SRCS = $(wildcard **/*.c **/*.asm)
DEPS = $(wildcard **/*.h)

OBJS=$(addprefix $(ODIR)/, $(patsubst %.c,%.o,$(patsubst %.asm,%.o,$(notdir $(SRCS)))))

CFLAGS = -g3 -m32 -fno-builtin -fno-exceptions -Wall -Wextra

kernel.elf: ${OBJS}
	i386-elf-ld -T linker.ld -nostdlib -g -o kernel.elf $^

run: kernel.elf
	qemu-system-i386 -kernel kernel.elf

debug: kernel.elf
	qemu-system-i386 -s -S -kernel kernel.elf -d guest_errors,int &
	sleep 0.2
	gdb -ex "target remote localhost:1234" -ex "symbol-file kernel.elf"


${ODIR}/%.o: %.c ${DEPS}
	${CC} ${CFLAGS} -ffreestanding -c $< -o $@

${ODIR}/%.o: %.asm
	nasm $< -f elf -g -o $@

clean:
	rm -rf *.bin *.dis ${ODIR}/*.o os-image.bin *.elf
