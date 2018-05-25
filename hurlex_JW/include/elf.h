#ifndef INCLUDE_ELF_H_
#define INCLUDE_ELF_H_

#include"types.h"
#include"multiboot.h"

#define ELF32_ST_TYPE(i) ((i)&0xf)

// ELF的section header，段描述符结构
typedef struct elf_section_header_t{
	uint32_t name;//段的名字。段名是字符串，位于一个叫做.shstrtab的段表字符串表。name是段名字符串在.shstrtab中的偏移
	uint32_t type;//段的类型
	uint32_t flags;
	uint32_t addr;//段在执行时的虚拟地址
	uint32_t offset;//文件中的偏移量
	uint32_t size;//字节大小
	uint32_t link;//到其他段的链接
	uint32_t info;//额外的段信息
	uint32_t addralign;//段对齐
	uint32_t entsize;//如果包含table，entry的大小
}__attribute__((packed)) elf_section_header_t;


// ELF格式符号
typedef struct elf_symbol_t{
	uint32_t name;//符号在字符串表中的偏移
	uint32_t value;
	uint32_t size;
	uint8_t info;
	uint8_t other;
	uint16_t shndx;
}__attribute__((packed)) elf_symbol_t;


typedef struct elf_t{
	elf_symbol_t *symtab;//符号表
	uint32_t symtabsz;//符号表的字节大小
	
	const char *strtab;//字符串表
	uint32_t strtabsz;//字符串表的字节大小
}elf_t;

/*
1, ELF格式包含一个section header表，每个元素就是一个elf_section_header_t，描述一个section。
multiboot_t的addr变量，指向这个表。

(1) ELF有一个段的addr指向内容存储所有段的名字。multiboot_t的shndx变量，是该段的索引。

2, ELF格式包含一个字符串表。

3, ELF格式包含一个符号表。

*/

// 从multiboot_t结构获得ELF信息
elf_t elf_from_multiboot(multiboot_t *mb);

// 根据地址查找ELF符号
const char *elf_lookup_symbol(uint32_t addr, elf_t *elf);

#endif
