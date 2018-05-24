# Makefile
# 默认使用GCC、nasm

#patsubst 处理所有在C_SOURCES
C_SOURCES=$(shell find . -name "*.c")
C_OBJECTS=$(patsubst %.c, %.o, $(C_SOURCES))
S_SOURCES=$(shell find . -name "*.s")
S_OBJECTS=$(patsubst %.s, %.o, $(S_SOURCES))

CC=gcc
LD=ld
ASM=nasm

# 编译标志：生成32位代码；添加调试信息；不使用标准库的头文件；要求gcc不主动使用内建函数；不使用栈保护等检测；
C_FLAGS=-c -Wall -m32 -ggdb -gstabs -nostdinc -fno-builtin -fno-stack-protector -I include
# 链接标志：使用链接脚本scripts/kernel.ld 生成i386架构下elf格式的可执行文件；不链接标准库
LD_FLAGS=-T scripts/kernel.ld -m elf_i386 -nostdlib
ASM_FLAGS=-f elf -g -F stabs

all: $(S_OBJECTS) $(C_OBJECTS) link update_image

# 变量 $< 表示第一个依赖文件
# $^ 表示所有依赖文件
# $@ 表示目标文件
.c.o:
	@echo 编译代码文件 $<
	$(CC) $(C_FLAGS) $< -o $@
	
.s.o:
	@echo 编译汇编文件 $<
	$(ASM) $(ASM_FLAGS)	$<
	
link:
	@echo 链接内核文件
	$(LD) $(LD_FLAGS) $(S_OBJECTS) $(C_OBJECTS) -o hx_kernel


.PHONY:clean
clean:
	$(RM) $(S_OBJECTS) $(C_OBJECTS) hx_kernel
	
# 更新floppy.img的hx_kernel内核文件
.PHONY:update_image
update_image:
	mount floppy.img /mnt/kernel
	cp hx_kernel /mnt/kernel/hx_kernel
	sleep 1
	umount /mnt/kernel
	
.PHONY:mount_image
mount_image:
	mount floppy.img /mnt/kernel
	
.PHONY:umount_image
umount_image:
	umount /mnt/kernel
	
.PHONY:qemu
qemu:
	qemu -drive file=floppy.img,format=raw,index=0,if=floppy
	#qemu -fda floppy.img -boot a
	
.PHONY:bochs
bochs:
	bochs -f scripts/bochsrc.txt
	
.PHONY:debug
debug:
	#qemu -S -s -fda floppy.img -boot a &
	qemu -S -s -drive file=floppy.img,format=raw,index=0,if=floppy &
	sleep 1
	cgdb -x scripts/gdbinit
	
