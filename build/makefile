# Make variables
CC = gcc
AS = nasm

# CFLAGS and EXE_CFLAGS setup
CFLAGS = -std=c17 -m32 -march=i386 -mgeneral-regs-only -ffreestanding -fno-stack-protector \
    -fno-builtin -nostdinc -fno-pie \
    -Wall -Wextra -Wno-pointer-sign -Wno-interrupt-service-routine -I../include/

EXE_CFLAGS = -std=c17 -m32 -march=i386 -mgeneral-regs-only -ffreestanding -fno-stack-protector \
    -fno-builtin -nostdinc -fPIE \
    -Wall -Wextra -Wno-pointer-sign -Wno-interrupt-service-routine -I../include/

SRCDIR = ../src/
BINDIR = ../bin/
FONTDIR = ../src/fonts/

# Kernel sources
KRNL_C_SOURCES = kernel.c mouse.c 3rdstage.c globals.c gui/window.c
KRNL_ASM_SOURCES = interrupts/irq_stubs.S
KRNL_OBJS = $(patsubst %.c,$(BINDIR)%.o,$(KRNL_C_SOURCES)) $(patsubst %.S,$(BINDIR)%.o,$(KRNL_ASM_SOURCES))

# Other C files (not part of the kernel)
C_FILES =

# Standalone executables
EXES =

# Bootloader assembly files
ASM_FILES = 2ndstage bootSect ter-u16n ter-u18n ter-u32n testfont

# Font files
FONTS_BDF != find $(FONTDIR) -name "*.bdf" -exec basename -s ".bdf" {} \;

# Main target
OS: $(FONTS_BDF) $(patsubst %,$(BINDIR)%.bin,$(ASM_FILES)) $(patsubst %,$(BINDIR)%.bin,$(C_FILES)) $(patsubst %,$(BINDIR)%.bin,$(EXES)) $(BINDIR)kernel.bin
	@mkdir -p $(BINDIR)fs_root/sys/bin
	@mv $(BINDIR)bootSect.bin $(BINDIR)fs_root/sys/bin/bootSect.bin
	@mv $(BINDIR)2ndstage.bin $(BINDIR)fs_root/sys/bin/2ndstage.bin
	@mv $(BINDIR)kernel.bin $(BINDIR)fs_root/sys/bin/kernel.bin
	@$(CC) ./make_disk.c -o ./make_disk.bin -I../include	
	@./make_disk.bin

# Rule to link the kernel
$(BINDIR)kernel.bin: $(KRNL_OBJS)
	@ld -melf_i386 -Tkernel.ld $(KRNL_OBJS) --oformat binary -o $(BINDIR)kernel.bin
	@echo "kernel linked"

# Rule to compile kernel C files
$(BINDIR)%.o: $(SRCDIR)%.c
	@mkdir -p $(@D)
	@$(CC) -c $(CFLAGS) -o $@ $<

# Rule to assemble kernel assembly files
$(BINDIR)interrupts/irq_stubs.o: $(SRCDIR)interrupts/irq_stubs.S
	@mkdir -p $(BINDIR)interrupts
	@$(AS) -f elf32 -o $@ $<

# Rule for other C files
$(patsubst %,$(BINDIR)%.bin,$(C_FILES)): $(BINDIR)%.bin: $(SRCDIR)%.c
	@mkdir -p $(BINDIR)
	@$(CC) -c $(CFLAGS) -o $(BINDIR)$*.o $<
	@ld -melf_i386 -T$*.ld $(BINDIR)$*.o --oformat binary -o $(BINDIR)$*.bin
	@rm -f $(BINDIR)$*.o

# Rule for standalone executables
$(patsubst %,$(BINDIR)%.bin,$(EXES)): $(BINDIR)%.bin: $(SRCDIR)%.c
	@mkdir -p $(BINDIR)
	@$(CC) -c $(EXE_CFLAGS) -o $(BINDIR)$*.o $<
	@ld -melf_i386 -e main -pie $(BINDIR)$*.o -o $(BINDIR)$*.bin
	@rm -f $(BINDIR)$*.o

# Rule for bootloader assembly files
$(patsubst %,$(BINDIR)%.bin,$(ASM_FILES)): $(BINDIR)%.bin: $(SRCDIR)%.asm
	@mkdir -p $(BINDIR)
	@$(AS) -f bin -o $@ $<
	@size=$$(($$(wc -c < $@)));\
	echo "$*" "$$size ($$(printf '0x%02X' $$((size / 512))) sectors)";\

# ISO generation and run targets remain the same
iso: OS
	@mkdir -p $(BINDIR)/iso/boot/grub
	@mv $(BINDIR)/OS.bin $(BINDIR)/iso/boot/grub/iso9660.bin
	@echo "set timeout=0" > $(BINDIR)/iso/boot/grub/grub.cfg
	@echo "set default=0" >> $(BINDIR)/iso/boot/grub/grub.cfg
	@echo "menuentry \"FSDOS\" {" >> $(BINDIR)/iso/boot/grub/grub.cfg
	@echo "    multiboot /boot/grub/iso9660.bin" >> $(BINDIR)/iso/boot/grub/grub.cfg
	@echo "    boot" >> $(BINDIR)/iso/boot/grub/grub.cfg
	@echo "}" >> $(BINDIR)/iso/boot/grub/grub.cfg
	@mkdir -p $(BINDIR)/iso/boot/grub/i386-pc
	@cp /usr/lib/grub/i386-pc/* $(BINDIR)/iso/boot/grub/i386-pc/
	@genisoimage -R -b boot/grub/iso9660.bin -no-emul-boot -boot-load-size 4 -boot-info-table -o $(BINDIR)/FSDOS.iso $(BINDIR)/iso

bin: OS
	@echo "OS.bin has been created in $(BINDIR)"

run_bin: bin
	qemu-system-i386 \
	-m 512M \
	-drive format=raw,file=$(BINDIR)/OS.bin,if=ide,index=0,media=disk \
	-rtc base=localtime,clock=host,driftfix=slew

run: iso
	qemu-system-i386 \
	-m 512M \
	-drive format=raw,file=$(BINDIR)/FSDOS.iso,if=ide,index=0,media=cdrom \
	-rtc base=localtime,clock=host,driftfix=slew

clean:
	rm -rf $(BINDIR) ./*.o

make_font_pgm:
	@make -C $(FONTDIR) 1>/dev/null

$(FONTS_BDF): make_font_pgm
	@$(FONTDIR)bdf_to_asm.bin $(FONTDIR)$@.bdf
	@mv $(FONTDIR)$@.asm $(SRCDIR)
