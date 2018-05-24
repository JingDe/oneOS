; boot.s 内核从这里开始
; 遵守grub multiboot规范

MBOOT_HEADER_MAGIC equ 0x1BADB002 ; Multiboot魔数

MBOOT_PAGE_ALIGN equ 1<<0 ; Multiboot flags域的0位表示所有的引导模块将按页边界对齐（4KB）
MBOOT_MEM_INFO equ 1<<1 ; 1位表示multiboot信息结构的mem_*域包括可用内存的信息
; 告诉GRUB把内存空间的信息包含在multiboot信息结构中

; 定义multiboot的flags域
MBOOT_HEADER_FLAGS equ MBOOT_PAGE_ALIGN | MBOOT_MEM_INFO

;
MBOOT_CHECKSUM equ -(MBOOT_HEADER_MAGIC+MBOOT_HEADER_FLAGS)

;符合multiboot规范的OS映像需要这样一个multiboot头
; Multiboot 头的分布必须如下表所示：
; ----------------------------------------------------------
; 偏移量  类型  域名        备注
;
;   0     u32   magic       必需
;   4     u32   flags       必需 
;   8     u32   checksum    必需 
;

[BITS 32]		; 所有代码以32 bit的方式编译


section .text

	; 在代码段的起始位置设置符合multiboot规范的标记
dd MBOOT_HEADER_MAGIC	; GRUB会通过这个魔数判断该映像是否支持
dd MBOOT_HEADER_FLAGS	; GRUB的加载时选项
dd MBOOT_CHECKSUM		; 检测数值，其含义在定义处

[GLOBAL start]			; 内核代码入口，提供该声明给ld链接器
[GLOBAL glb_mboot_ptr]	; 全局的struct multiboot *变量
[EXTERN kern_entry]		; 声明内核c代码的入口函数


start:
	cli					; 此时还没有设置好保护模式的中断处理，要关闭中断
	
	mov esp, STACK_TOP	; 设置内核栈地址
	mov ebp, 0			; 帧指针修改为0
	and esp, 0FFFFFFF0H	; 栈地址按照16字节对齐
	mov [glb_mboot_ptr], ebx	; 将ebx中存储的指针存入全局变量
	call kern_entry		; 调用内核入口函数
	
stop:
	hlt					; 停机指令，
	jmp stop			; 

	
section .bss			; 未初始化的数据段从这里开始
stack:
	resb 32768			; 内核栈
glb_mboot_ptr:			; 全局multiboot结构体指针
	resb 4

STACK_TOP equ $-stack-1	; 内核栈顶
