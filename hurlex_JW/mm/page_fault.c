#include"vmm.h"
#include"debug.h"

void page_fault(pt_regs* regs)
{	
	uint32_t cr2;
	asm volatile("mov %%cr2, %0" : "=r" (cr2));
	
	printk("Page fault at 0x%x, virtual faulting address 0x%x\n", regs->eip, cr2);
	printk("Error code: %x\n", regs->err_code);
	
	// bit 0为0指页面不存内存里
	if(!(regs->err_code  &  0x1))
		printk_color(rc_black, rc_red, "Because the page wasn't present.\n");
		
	// bit 1为0表示读错误，为1为写错误
	if(regs->err_code  &  0x2)
		printk_color(rc_black, rc_red, "Write error.\n");
	else
		printk_color(rc_black, rc_red, "Read error.\n");
	
	// bit 2为1表示在用户模式打断的，为0表示在内核模式打断的
	if(regs->err_code  &  0x4)
		printk_color(rc_black, rc_red, "In user mode.\n");
	else
		printk_color(rc_black, rc_red, "In kernel mode.\n");
		
	// bit 3 为 1 表示错误是由保留位覆盖造成的
	if (regs->err_code & 0x8) {
		printk_color(rc_black, rc_red, "Reserved bits being overwritten.\n");
	}
	// bit 4 为 1 表示错误发生在取指令的时候
	if (regs->err_code & 0x10) {
		printk_color(rc_black, rc_red, "The fault occurred during an instruction fetch.\n");
	}

	print_stack_trace();
	
	
	while (1);
}
