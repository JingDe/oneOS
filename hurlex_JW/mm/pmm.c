#include"multiboot.h"
#include"common.h"
#include"debug.h"
#include"pmm.h"

// 物理内存页面管理的栈
static uint32_t pmm_stack[PAGE_MAX_SIZE+1];

// 物理内存管理的栈指针
static uint32_t pmm_stack_top;

// 物理内存页的数量
uint32_t phy_page_count;

void show_memory_map()
{
	uint32_t mmap_addr=glb_mboot_ptr->mmap_addr;
	uint32_t mmap_length=glb_mboot_ptr->mmap_length;
	
	printk("Memory map:\n");
	
	mmap_entry_t *mmap=(mmap_entry_t*)mmap_addr;
	for(mmap=(mmap_entry_t*)mmap_addr; (uint32_t)mmap<mmap_addr+mmap_length; mmap++)
	{
		printk("base_addr=0x%X%08X, length=0x%X%08X, type=0x%0x\n",
			(uint32_t)mmap->base_addr_high, (uint32_t)mmap->base_addr_low,
			(uint32_t)mmap->length_high, (uint32_t)mmap->length_low,
			(uint32_t)mmap->type);
	}
}
