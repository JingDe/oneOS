#include"types.h"
#include"console.h"
#include"debug.h"
#include "gdt.h"
#include "idt.h"
#include "timer.h"
#include "pmm.h"

int kern_entry()
{
	init_debug();
	init_gdt();
	init_idt();
	
	console_clear();
	printk_color(rc_black, rc_green, "Hello, OS kernel!\n");
	
	//panic("test");
	//asm volatile ("int $0x3");
	//asm volatile ("int $0x4");
	
	init_timer(200);
	
	// 开始中断
	//asm volatile ("sti");
	
	printk("kernel in memory start: 0x%08X\n", kern_start);
	printk("kernel in memory end:   0x%08X\n", kern_end);
	printk("kernel in memory used:   %d KB\n\n", (kern_end - kern_start) / 1024);
	
	show_memory_map();
	init_pmm();
	
	printk_color(rc_black, rc_red, "\nThe Count of Physical Memory Page is: %u\n\n", phy_page_count);

	uint32_t alloc_addr = NULL;
	printk_color(rc_black, rc_light_brown, "Test Physical Memory Alloc :\n");
	alloc_addr=pmm_alloc_page();
	printk_color(rc_black, rc_light_brown, "Alloc Physical Addr: 0x%08X\n", alloc_addr);
	alloc_addr=pmm_alloc_page();
	printk_color(rc_black, rc_light_brown, "Alloc Physical Addr: 0x%08X\n", alloc_addr);
	alloc_addr=pmm_alloc_page();
	printk_color(rc_black, rc_light_brown, "Alloc Physical Addr: 0x%08X\n", alloc_addr);
	alloc_addr=pmm_alloc_page();
	printk_color(rc_black, rc_light_brown, "Alloc Physical Addr: 0x%08X\n", alloc_addr);
	
	return 0;
}

void print_hello()
{
	uint8_t *input=(uint8_t *)0xB8000;
	uint8_t color=(0 << 4) | (15  & 0x0F);
	
	*input++='H';	*input++=color;
	*input++='e';	*input++=color;
	*input++='l';	*input++=color;
	*input++='l';	*input++=color;
	*input++='o';	*input++=color;
	*input++=',';	*input++=color;
	*input++='w';	*input++=color;
	*input++='o';	*input++=color;
	*input++='r';	*input++=color;
	*input++='l';	*input++=color;
	*input++='d';	*input++=color;
}
