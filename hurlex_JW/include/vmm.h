#ifndef INCLUDE_VMM_H_
#define INCLUDE_VMM_H_

#include"types.h"
#include"idt.h"


// 内核的偏移地址 3GB
#define PAGE_OFFSET 0xC0000000


// 以下三个量表示页目录数据和页表数据的前三个比特位

/*
 * P--位0是存在标志，用于指明表项对地址转换是否有效。P=1表示有效。
 * 在页转换过程中，如果涉及的页目录或页表的表项无效，则会导致异常。
 * 如果P=0，那么除表示表项无效外，其余位可供程序自由使用。
 * 例如操作系统可以使用这些位来保存已存储在磁盘上的页面的序号
 */
#define PAGE_PRESENT 0x1

/*
 * R/W--位1是读写标志。如果等于1，表示页面可以被读、写或执行。
 * 如果为0，表示页面只读或可执行。
 * 当处理器运行在超级用户特权级（级别0,1,2）时，则R/W位不起作用
 * 页目录项中R/W位对其所映射的所有页面起作用
 */
#define PAGE_WRITE 0x2

/**
 * U/S--位2是用户/超级用户（User/Supervisor）标志。
 * 如果为1，那么运行在任何特权级上的程序都可以访问该页面。
 * 如果为0，那么页面只能被运行在超级用户特权级（0、1或2）上的程序访问。
 * 页目录项中的U/S位对其所映射的所有页面起作用。
 */
#define PAGE_USER 0x4

// 虚拟分页大小
#define PAGE_SIZE 4096

// 页掩码，用于4KB对齐
#define PAGE_MASK 0xFFFFF000

// 获得一个地址的页目录项
#define PGD_INDEX(x)  (((x)>>22) &  0x3FF)

// 获得一个地址的页表项
#define PTE_INDEX(x)  (((x)>>12)  &  0x3FF)

// 获得一个地址的页内偏移
#define OFFSET_INDEX(x) ((x) &  0xFFF)

// 页目录数据类型
// 一个页目录数据表示一个页表的首地址
typedef uint32_t pgd_t;

// 页表数据类型
// 一个页表数据表示一个物理页的首地址
typedef uint32_t pte_t;

// 页目录表成员数, 1024, 一个页有1024个页目录项
#define PGD_SIZE (PAGE_SIZE/sizeof(pgd_t)) 

// 页表成员数, 1024，一个页有1024个页表项
#define PTE_SIZE (PAGE_SIZE/sizeof(pte_t))

// 映射 512MB 内存所需要的页表数, 128
// 1个页表映射1024*4KB字节，512MB/(1024*4KB)=128
#define PTE_COUNT 128

// 内核页目录区域
extern pgd_t pgd_kern[PGD_SIZE];

// 初始化虚拟内存管理
void init_vmm();

// 更换当前的页目录
void switch_pgd(uint32_t pd);

// 使用flags指出的页权限，把物理地址pa映射到虚拟地址va
void map(pgd_t *pgd_now, uint32_t va, uint32_t pa, uint32_t flags);

// 取消虚拟地址va的物理映射
void unmap(pdg_t *pgd_now, uint32_t va);

// 如果虚拟地址va映射到物理地址则返回1
// 同事如果pa不是空指针则把物理地址写入pa参数
uint32_t get_mapping(pgd_t *pgd_now, uint32_t va, uint32_t *pa);

// 页错误中断的函数处理
void page_fault(pt_regs *regs);//参数是寄存器类型

#endif
