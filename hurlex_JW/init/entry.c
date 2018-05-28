#include"string.h"
#include"console.h"
#include"debug.h"
#include "gdt.h"
#include "idt.h"
#include "timer.h"
#include "pmm.h"
#include"vmm.h"

// 内核初始化函数
void kern_init();

// 开启分页机制后的 Multiboot数据指针
multiboot_t* glb_mboot_ptr;

// 开启分页机制后的内核栈
char kern_stack[STACK_SIZE];

// 内核使用的临时页表和页目录，使用1MB以下的12KB来放置
// 该地址必须是页对齐的地址，内存0-640KB 肯定是空闲的
__attribute__((section(".init.data"))) pgd_t *pgd_tmp=(pgd_t*) 0x1000;
__attribute__((section(".init.data"))) pgd_t *pte_low=(pgd_t*)0x2000;
__attribute__((section(".init.data"))) pgd_t *pte_high=(pgd_t*)0x3000;

// 内核入口函数
// 指定函数的存储区段
__attribute__((section(".init.text"))) void kern_entry()
{
// 一个页表有1024个页表项
// 映射4MB范围的线性地址需要1张页表，一个页表项

	// 用于映射0-4MB的虚拟地址的页表项
	pgd_tmp[0]=(uint32_t) pte_low | PAGE_PRESENT | PAGE_WRITE;
	// 用于映射0xC0000000开始4MB的虚拟地址的页表项
	pgd_tmp[PGD_INDEX(PAGE_OFFSET)]=(uint32_t)pte_high | PAGE_PRESENT | PAGE_WRITE;
	
	
	// 映射低虚拟地址的一张页表，映射内核虚拟地址0-4MB到物理地址的前4MB
	int i;
	for(i=0; i<1024; i++)
		pte_low[i]=(i<<12)  |  PAGE_PRESENT |  PAGE_WRITE;
	
	// 映射高虚拟地址的一张页表，映射虚拟地址0xC0000000-0xC0400000 到物理地址的前4MB
	for(i=0; i<1024; i++)
		pte_high[i]=(i<<12) | PAGE_PRESENT | PAGE_WRITE;
	
	// 设置临时页表
	asm volatile("mov %0, %%cr3" : : "r" (pgd_tmp));
	
	
	// 启动分页，将cr0寄存器的分页位 置为1
	uint32_t cr0;	
	asm volatile("mov %%cr0, %0" : "=r" (cr0));
	cr0 |= 0x80000000;
	asm volatile("mov %0, %%cr0" : : "r" (cr0));
	
	// 启动分页后，所有寻址都将按照分页机制进行，包括剩余kern_entry（）函数本身，所以上文同时映射了0-4MB的虚拟地址
	
	// 切换内核栈
	uint32_t kern_stack_top=((uint32_t) kern_stack + STACK_SIZE) &  0xFFFFFFF0;
	asm volatile("mov %0, %%esp\n\t" "xor %%ebp, %%ebp" : : "r" (kern_stack_top));
	
	// 更新全局的multiboot指针
	glb_mboot_ptr=mboot_ptr_tmp + PAGE_OFFSET;
	
	// 调用内核初始化函数
	kern_init();
}

void kern_init()
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
	
	//show_memory_map();
	init_pmm();
	init_vmm();
	init_heap();
	
	printk_color(rc_black, rc_red, "\nThe Count of Physical Memory Page is: %u\n\n", phy_page_count);

/*
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
*/

	test_heap();
	
	printk("hlt...\n");
	while(1)
	{
		asm volatile("hlt");
	}
}


