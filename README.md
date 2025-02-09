# FS-DOS
Freney Studios Disk Operating System

# Thanks to Queso-Fuego!
The original code author

# Project Past 
The project started in 2023 summer,
called *Entelev*, but it didn't made
the final cut as Freney Studios Project
the OS market is full of rivals. The code
of Entelev is lost but it was an edited 
version of *Queso-Fuego* amazing work,
Entelev supported mouse, windows system,
blur design and a "start" menu. Now we
decided to re-create a DOS (So an OS without
window manager, gfx bloats) just to run 
.BIN files 

# Open Source Stuff
feel free to commit/branch/fork/eat
this repo and experimenting explosive
and mind-blowing OSDEV stuff!

# What changed?
- In FSDOS
  - Makefile .iso started support,
  - int header in stdlib "fixed"

- In Entelev
   - int header in stdlib "fixed"
   - added a graphical environment with texts using 8 bit font
   - window system
   - tried to add .bmp support (failed)
   - blur style taskbar
   - removed CLI interface
 
# How to compile
Note: this OS can be compiled on Linux, if you are using Windows, we advice a WLS against MSYS2

Make the binary files and run them in QEMU
  $ make bin
  $ make run_bin

Make thi .iso file (FSDOS.iso)
  $ make iso
  $ make run

# Requirements
make, nasm, gcc, qemu-system, genisoimage (not working)

Download them (Ubuntu/Linux)
  $ sudo apt-get install make nasm qemu-system genisoimage
  (Assuming GCC is already installed by system)

# Bugs
.ISO system doesn't work correctly!
(QEMU starts, tell that's loading the OS by CD/DVD
 and then it takes years, we didn't tested if you
 wait the right time, then it will word)


