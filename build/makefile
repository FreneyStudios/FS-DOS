# Make variables
CC = gcc
#CC = clang

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
C_FILES != find $(SRCDIR) -maxdepth 1 -name "*.c" ! -name "calculator.c" ! -name "editor.c" -exec basename -s ".c" {} \;
EXES = calculator editor
FONTS_BDF != find $(FONTDIR) -name "*.bdf" -exec basename -s ".bdf" {} \;

# Make final OS.bin binary - padding out to full 1.44MB "floppy" img
OS: $(FONTS_BDF) asm_files $(C_FILES) $(EXES)
	@mkdir -p $(BINDIR)fs_root/sys/bin
	@mv $(BINDIR)[!OS]*.bin $(BINDIR)fs_root/sys/bin
	@$(CC) ./make_disk.c -o ./make_disk.bin -I../include	
	@./make_disk.bin

# Assemble assembly source files into binary files,
#   as fonts or other targets can make additional .asm files, need to process at runtime
asm_files: 
	@for file in `find $(SRCDIR) -maxdepth 1 -name "*.asm" -exec basename -s ".asm" {} \;`; do \
		nasm -f bin -o $(BINDIR)$$file.bin $(SRCDIR)$$file.asm 1>/dev/null;\
		size=$$(($$(wc -c < $(BINDIR)$$file.bin)));\
		echo "$$file" "$$size ($$(printf '0x%02X' $$((size / 512))) sectors)";\
	done

# Compile C source files into PIE executable files, and pad out their size to next 512 byte sector 
$(EXES):
	@$(CC) -c $(EXE_CFLAGS) -o $@.o $(SRCDIR)$@.c
	@ld -melf_i386 -e main -pie $@.o -o $(BINDIR)$@.bin
	@rm -f $@.o
	@size=$$(($$(wc -c < $(BINDIR)$@.bin)));\
	newsize=$$((size - $$((size % 512)) + 512));\
	echo "$@" "$$size ($$(printf '0x%02X' $$((size / 512))) sectors) ->" "$$newsize ($$(printf '0x%02X' $$((newsize / 512))) sectors)";\
	dd if=/dev/zero of=$(BINDIR)$@.bin bs=1 seek=$$size count=$$((newsize - size)) status=none;

# Compile C source files into binary files, and pad out their size to next 512 byte sector 
$(C_FILES):
	@$(CC) -c $(CFLAGS) -o $@.o $(SRCDIR)$@.c
	@ld -melf_i386 -T$@.ld $@.o --oformat binary -o $(BINDIR)$@.bin
	@rm -f $@.o
	@size=$$(($$(wc -c < $(BINDIR)$@.bin)));\
	newsize=$$((size - $$((size % 512)) + 512));\
	echo "$@" "$$size ($$(printf '0x%02X' $$((size / 512))) sectors) ->" "$$newsize ($$(printf '0x%02X' $$((newsize / 512))) sectors)";\
	dd if=/dev/zero of=$(BINDIR)$@.bin bs=1 seek=$$size count=$$((newsize - size)) status=none;

# Create the ISO file from OS.bin, renaming it to iso9660.bin for compatibility with genisoimage
iso: OS
	@mkdir -p $(BINDIR)/iso/boot/grub
	@mv $(BINDIR)/OS.bin $(BINDIR)/iso/boot/grub/iso9660.bin  # Rename OS.bin to iso9660.bin
	@echo "set timeout=0" > $(BINDIR)/iso/boot/grub/grub.cfg
	@echo "set default=0" >> $(BINDIR)/iso/boot/grub/grub.cfg
	@echo "menuentry \"FSDOS\" {" >> $(BINDIR)/iso/boot/grub/grub.cfg
	@echo "    multiboot /boot/grub/iso9660.bin" >> $(BINDIR)/iso/boot/grub/grub.cfg  # Use iso9660.bin in grub.cfg
	@echo "    boot" >> $(BINDIR)/iso/boot/grub/grub.cfg
	@echo "}" >> $(BINDIR)/iso/boot/grub/grub.cfg
	# Copy GRUB boot image files
	@mkdir -p $(BINDIR)/iso/boot/grub/i386-pc
	@cp /usr/lib/grub/i386-pc/* $(BINDIR)/iso/boot/grub/i386-pc/
	@genisoimage -R -b boot/grub/iso9660.bin -no-emul-boot -boot-load-size 4 -boot-info-table -o $(BINDIR)/FSDOS.iso $(BINDIR)/iso

# Make the BIN image (this is essentially the same as the OS target)
bin: OS
	@echo "OS.bin has been created in $(BINDIR)"

# Run OS directly through QEMU with the BIN file
run_bin: bin
	qemu-system-i386 \
	-monitor stdio \
	-m 512M \
	-drive format=raw,file=$(BINDIR)/OS.bin,if=ide,index=0,media=disk \
	-rtc base=localtime,clock=host,driftfix=slew \
	-audiodev id=alsa,driver=alsa \
	-machine pcspk-audiodev=alsa
	#-enable-kvm \

# Launch OS through QEMU with the ISO
run: iso
	qemu-system-i386 \
	-monitor stdio \
	-m 512M \
	-drive format=raw,file=$(BINDIR)/FSDOS.iso,if=ide,index=0,media=cdrom \
	-rtc base=localtime,clock=host,driftfix=slew \
	-audiodev id=alsa,driver=alsa \
	-machine pcspk-audiodev=alsa
	#-enable-kvm \

# Launch OS through bochs
bochs:
	bochs -qf $(BINDIR).bochsrc

clean:
	rm -f $(BINDIR)*.bin ./*.o $(BINDIR)/FSDOS.iso $(BINDIR)/iso -r

# Make program to convert fonts
make_font_pgm:
	@make -C $(FONTDIR) 1>/dev/null

# Convert bdf fonts to OS asm font files
$(FONTS_BDF): make_font_pgm
	@$(FONTDIR)bdf_to_asm.bin $(FONTDIR)$@.bdf
	@mv $(FONTDIR)$@.asm $(SRCDIR)
