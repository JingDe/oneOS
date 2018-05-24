#include"types.h"
#include"console.h"

int kern_entry()
{
	console_clear();
	console_write_color("Hello, OS kernel!\n", rc_black, rc_green);
	
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
