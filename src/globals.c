#include "C/stdint.h"
#include "C/stdbool.h"
#include "C/stdio.h"
#include "fs/fs.h"
#include "fs/fs_impl.h"
#include "interrupts/idt.h"
#include "interrupts/pic.h"
#include "memory/malloc.h"
#include "memory/physical_memory_manager.h"
#include "memory/virtual_memory_manager.h"
#include "gfx/2d_gfx.h"

// from fs_impl.h
char *current_dir;
inode_t current_dir_inode;
inode_t current_parent_inode;
uint8_t temp_sector[FS_SECTOR_SIZE];
uint8_t temp_block[FS_BLOCK_SIZE];
inode_t root_inode;
superblock_t superblock;

// from idt.h
idt_entry_32_t idt32[256];
idtr_32_t idtr32;

// from pic.h
uint32_t *sleep_timer_ticks;
fs_datetime_t *datetime;

// from malloc.h
malloc_block_t *malloc_list_head = 0;
uint32_t malloc_virt_address     = 0;
uint32_t malloc_phys_address     = 0;
uint32_t total_malloc_pages      = 0;

// from physical_memory_manager.h
uint32_t *memory_map = 0;
uint32_t max_blocks  = 0;
uint32_t used_blocks = 0;

// from virtual_memory_manager.h
page_directory *current_page_directory = 0;

// from stdio.h
uint8_t *write_buffer = 0;
uint32_t len = 0;

// from 2d_gfx.h
vbe_mode_info_t *gfx_mode;
user_gfx_info_t *user_gfx_info;
