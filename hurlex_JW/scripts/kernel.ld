/*
	这个脚本告诉ld程序如何构造内核映像文件

	首先声明内核程序的入口地址是boot.s文件里的符号“start”	
	然后声明第一个段.text段以及起始地址0x100000(1MB)
	接着声明已初始化数据段.data和未初始化数据段.bss段
	采用4KB 页对齐方式
	Linux GCC增加额外的数据段.rodata，只读的已初始化数据段，放置常量等
	为了简化，与.data段放置在一起
*/

ENTRY(start)
SECTIONS
{
	PROVIDE( kern_start = 0xC0100000 );
	. =0x100000;
	.init.text :
	{
		*(.init.text)
		. = ALIGN(4096);
	}
	.init.data :
	{
		*(.init.data)
		. = ALIGN(4096);
	}
	
	. += 0xC0000000;
	.text : AT(ADDR(.text) - 0xC0000000)
	{
		*(.text)
		. = ALIGN(4096);
	}
	.data : AT(ADDR(.data) - 0xC0000000)
	{
		*(.data)
		*(.rodata)
		. = ALIGN(4096);
	}
	.bss : AT(ADDR(.bss) - 0xC0000000)
	{
		*(.bss)
		. = ALIGN(4096);
	}
	.stab : AT(ADDR(.stab) - 0xC0000000)
	{
		*(.stab)
		. = ALIGN(4096);
	}
	.stabstr : AT(ADDR(.stabstr) - 0xC0000000)
	{
		*(.stabstr)
		. = ALIGN(4096);
	}
	PROVIDE( kern_end = . );
	
	/DISCARD/ : { *(.comment) *(.eh_frame) }
}
