#ifndef INCLUDE_MULTIBOOT_H_
#define INCLUDE_MULTIBOOT_H_

#include"types.h"

typedef struct multiboot_t{
	uint32_t flags;//版本信息
	
	/*可用内存
	 *低端内存的首地址是0，高端内存的首地址是1M
	 */
	uint32_t mem_lower;//低端内存的大小，单位K
	uint32_t mem_upper;//高端内存的大小,单位K
	
	uint32_t boot_device;//指出引导程序从那个BIOS磁盘设备载入的OS映像
	uint32_t cmdline;//内核命令行
	uint32_t mods_count;//boot模块列表
	uint32_t mods_addr;
	
	/*
	 * ELF格式内核映像的section头表。
	 * 包括每项的大小、一共有几项以及作为名字索引的字符串。
	 * 对应于可执行可链接格式ELF的program头中的shdr_*项
	 * 所有的section都会被载入，ELF section头的物理地址域指向所有的section在内存中的位置
	 */
	uint32_t num;//段的个数
	uint32_t size;
	uint32_t addr;//段描述符表
	uint32_t shndx;//段表字符串表.shstrtab在段描述符表中的下标
	
	// 由BIOS提供的内存分布的缓冲区的地址和长度
	uint32_t mmap_length;
	uint32_t mmap_addr;
	
	uint32_t drivers_length;
	uint32_t drivers_addr;
	uint32_t config_table;
	uint32_t boot_loader_name;
	uint32_t apm_table;
	uint32_t vbe_control_info;
	uint32_t vbe_mode_info;
	uint32_t vbe_mode;
	uint32_t vbe_interface_seg;
	uint32_t vbe_interface_off;
	uint32_t vbe_interface_len;
}__attribute__((packed)) multiboot_t;//取消优化对齐

typedef struct mmap_entry_t{
	uint32_t size;
	uint32_t base_addr_low;
	uint32_t base_addr_high;
	uint32_t length_low;
	uint32_t length_hight;
	uint32_t type;
}__attribute__((packed)) mmap_entry_t;

// boot.s中定义，存放ebx中存储的指针
extern multiboot_t* glb_mboot_ptr;

#endif
