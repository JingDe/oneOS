; 将GDT地址载入GDTR

[GLOBAL gdt_flush]

gdt_flush:
	mov eax, [esp+4]	;参数gdt_ptr存入eax寄存器
	lgdt [eax]			;加载到GDTR
	
	mov ax, 0x10		;加载我们的数据段描述符
	mov ds, ax			;更新所有可以更新的段寄存器
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	jmp 0x08:.flush		;远跳转，0x08是我们的代码段描述符
						;对应gdt_ptr第二项
						; 远跳目的是清空流水线并串行化处理器
.flush:
	ret
	
