#include"common.h"
#include"string.h"
#include"elf.h"

elf_t elf_from_multiboot(multiboot_t* mb)
{
	int i;
	elf_t elf;
	elf_section_header_t *sh=(elf_section_header_t*)mb->addr;//段表
	
	uint32_t shstrtab=sh[mb->shndx].addr;//段表字符串表的地址
	for(i=0; i<mb->num; i++)//对所有mb->num个段
	{
		const char* name=(const char*)(shstrtab + sh[i].name);//段名
		if(strcmp(name, ".strtab")==0)//字符串表
		{
			elf.strtab=(const char*)sh[i].addr;//该段的地址
			elf.strtabsz=sh[i].size;//该段的大小
		}
		if(strcmp(name, ".symtab")==0)//符号表
		{
			elf.symtab=(elf_symbol_t*)sh[i].addr;
			elf.symtabsz=sh[i].size;
		}
	}
	return elf;
}

// 查询地址addr在ELF文件elf中的那个符号代表的函数里
const char *elf_lookup_symbol(uint32_t addr, elf_t *elf)
{
	int i;
	
	for(i=0; i<(elf->symtabsz/sizeof(elf_symbol_t)); i++)
	{
		if(ELF32_ST_TYPE(elf->symtab[i].info) != 0x2)
			continue;
		// 通过函数调用地址查到函数的名字(地址在该函数的代码段地址区间之内)
		if((addr >= elf->symtab[i].value)  &&  (addr<(elf->symtab[i].value+elf->symtab[i].size)) )
		{
			return (const char*)((uint32_t)elf->strtab+elf->symtab[i].name);
		}
	}
	return NULL;
}
