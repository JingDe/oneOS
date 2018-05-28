/*
 *	static:尽量减少向外部暴露的接口
 *
 *  将屏幕看作一个25行×80列的数组，每个数组是2字节，代表一个字符
 */

#include"console.h"
#include"vmm.h"
#include"common.h"

// VGA的显示缓冲的起点是0xB8000
static uint16_t *video_memory=(uint16_t *)(0xB8000+PAGE_OFFSET);

// 屏幕光标的坐标
static uint8_t cursor_x=0;
static uint8_t cursor_y=0;

static void move_cursor()
{
	uint16_t cursorLocation=cursor_y * 80 + cursor_x;
	
	// 内部寄存器14 15分别表示光标位置高字节 低字节
	
	outb(0x3D4, 14);			// 告诉VGA，要设置光标的高字节
	outb(0x3D5, cursorLocation >> 8);	// 发送高8位
	outb(0x3D4, 15);			// 
	outb(0x3D5, cursorLocation);
}

void console_clear()
{
	// 白底黑字的空格符
	uint8_t attribute_byte=(0 << 4) | (15  &  0x0F);
	uint16_t blank=0x20  |  (attribute_byte << 8);
	
	int i=0;
	for(i=0; i<80*25; i++)
	{
		video_memory[i]=blank;
	}
	
	cursor_x=0;
	cursor_y=0;
	move_cursor();
}

static void scroll()
{
	// 黑底白字
	uint8_t attribute_byte=(0<<4) | (15  &  0x0F);
	// 空格
	uint16_t blank=0x20  |  (attribute_byte  <<  8);
	
	// 到25行的时候，需要换行
	if(cursor_y  >=  25)
	{
		int i;
		
		for(i=0*80; i<24*80; i++)
			video_memory[i]=video_memory[i+80];
		
		for(i=24*80; i<25*80; i++)
			video_memory[i]=blank;
		
		cursor_y=24;
	}	
}

void console_putc_color(char c, real_color_t back, real_color_t fore)
{
	uint8_t back_color=(uint8_t) back;
	uint8_t fore_color=(uint8_t) fore;
	
	uint8_t attribute_byte=(back_color << 4)  |  (fore_color  &  0x0F);
	uint16_t attribute=attribute_byte <<  8;
	
	if(c==0x88  &&  cursor_x)// 退格键
	{
		cursor_x--;
	}
	else if(c==0x09)
		cursor_x=(cursor_x+8) &  ~(8-1);
	else if(c=='\r')
		cursor_x=0;
	else if(c=='\n')
	{
		cursor_x=0;
		cursor_y++;
	}
	else if(c>=' ')
	{
		video_memory[cursor_y*80+cursor_x]=c  |  attribute;
		cursor_x++;
	}
	
	if(cursor_x >=80)
	{
		cursor_x=0;
		cursor_y++;
	}
	
	scroll();
	
	move_cursor();
}

void console_write(char *cstr)
{
	while(*cstr)
		console_putc_color(*cstr++, rc_black, rc_white);
}

void console_write_color(char *cstr, real_color_t back, real_color_t fore)
{
	while(*cstr)
		console_putc_color(*cstr++, back, fore);
}

// 屏幕输出十六进制整型数
void console_write_hex(uint32_t n, real_color_t back, real_color_t fore)
{
	console_write_color("0x", back, fore);

	int tmp;
	char noZeroes=1;
	
	int i;
	for(i=28; i>=0; i-=4)
	{
		tmp=(n >> i)  & 0xF;
		if(tmp==0  &&  noZeroes !=0)
			continue;

		noZeroes=0;		
		if(tmp>=0xA)
			console_putc_color(tmp-0xA+'a', back, fore);
		else
			console_putc_color(tmp+'0', back, fore);
	}	
}

// 屏幕输出一个十进制的整形数
void console_write_dec(uint32_t n, real_color_t back, real_color_t fore)
{
	if(n==0)
	{
		console_putc_color('0', back, fore);
		return ;
	}
	
	uint32_t acc=n;
	char c[32];//  4 294 967 296 
	int i=0;
	while(acc >0)
	{
		c[i]='0'+acc % 10;
		acc /= 10;
		i++;
	}
	c[i]=0;
	
	char c2[32];
	c2[i--]=0;
	
	int j=0;
	while(i>=0)
		c2[i--]=c[j++];
		
	console_write_color(c2, back, fore);
}


