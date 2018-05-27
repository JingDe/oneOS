#include"types.h"
#include"console.h"
#include"debug.h"

int kern_entry()
{
	init_debug();
	init_gdt();
	init_idt();
	
	console_clear();
	printk_color(rc_black, rc_green, "Hello, OS kernel!\n");
	
	//panic("test");
	asm volatile ("int $0x3");
	asm volatile ("int $0x4");
	
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
