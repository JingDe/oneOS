/*
	这个脚本告诉ld程序如何构造内核映像文件

	首先声明内核程序的入口地址是符号“start”	
	然后声明第一个段.text段以及起始地址0x100000(1MB)
	接着声明已初始化数据段.data和未初始化数据段.bss段
	采用4096字节的页对齐方式
	Linux GCC增加额外的数据段.rodata，只读的已初始化数据段，放置常量等
	为了简化，与.data段放置在一起
*/

ENTRY(start)
SECTIONS
{
	.=0x100000;
	
	.text:
	{
		*(.text)
		. = ALIGN(4096);
	}
	.data
	{
		*(.data)
		*(.rodata)
		. =ALIGN(4096);
	}
	.bss:
	{
		*(.bss)
		.=ALIGN(4096);
	}
	.stab:
	{
		*(.stab)
		. =ALIGN(4096)
	}
	.stabstr:
	{
		*(.stabstr)
		. =ALIGN(4096)
	}
	/DISCARD/ : { *(.comment) *(.eh_frame)}
}
