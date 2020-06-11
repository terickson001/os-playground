CC = /usr/local/i386elfgcc/bin/i386-elf-gcc
GDB = /usr/local/i386elfgcc/bin/i386-elf-gdb

ODIR = obj

# Returns unique strings from list
uniq = $(if $1,$(firstword $1) $(call uniq,$(filter-out $(firstword $1),$1)))

SRCS_ := $(shell find . -regex ".*\.\(c\|asm\)" -printf "%P ")
SRCS := $(filter-out $(wildcard util/*), $(SRCS_))
DEPS := $(shell find . -regex ".*\.h" -printf "%P ")

VPATH := $(patsubst %/,%,$(realpath $(call uniq,$(dir $(SRCS_)))))

OBJS = $(addprefix $(ODIR)/, $(patsubst %.c,%.o,$(patsubst %.asm,%.asm.o,$(notdir $(SRCS)))))

WARN = -Wall -Wextra
NOWARN = -Wno-unused-function
CFLAGS = -lgcc -ffreestanding -g3 -m32 -fno-builtin -fno-exceptions ${WARN} ${NOWARN} -I.

BINARY = isofiles/boot/kernel.elf
MODULES = $(addprefix isofiles/boot/, initrd.img)
##### Kernel #####

os.img: ${BINARY} ${MODULES} isofiles/boot/grub/grub.cfg
	grub-mkrescue -o $@ isofiles

${BINARY}: ${OBJS}
	i386-elf-gcc -T linker.ld -O2 -nostdlib -g -o $@ $^

run: os.img
	qemu-system-i386 -no-reboot -no-shutdown -cdrom $<

debug: os.img
	qemu-system-i386 -s -S -no-reboot -no-shutdown -cdrom $< -d guest_errors,int &
	sleep 0.2
	gdb -ex "target remote localhost:1234" -ex "symbol-file ${BINARY}"


##### Utils #####
CC_UTIL = gcc
CFLAGS_UTIL = -g3 -Wall -Wextra -I. -m32

INITRD_FILES = $(wildcard initrd/*)
isofiles/boot/initrd.img: mkinitrd ${INITRD_FILES}
	./mkinitrd ${INITRD_FILES}

mkinitrd: util/mkinitrd.c ${DEPS}
	${CC_UTIL} ${CFLAGS_UTIL} $< -o $@

##### General #####
	
${ODIR}/%.o: %.c ${DEPS}
	${CC} ${CFLAGS} -ffreestanding -c $< -o $@

${ODIR}/%.asm.o: %.asm
	nasm $< -f elf -g -o $@

clean:
	rm -rf *.bin *.dis ${ODIR}/*.o *.elf
