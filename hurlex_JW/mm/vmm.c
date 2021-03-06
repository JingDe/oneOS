// MMU 的页目录表和页表的地址是物理地址

#include"idt.h"
#include"string.h"
#include"debug.h"
#include"vmm.h"
#include"pmm.h"

// 内核页目录区域
// 每一个值表示的是页表开始的物理地址
pgd_t pgd_kern[PGD_SIZE] __attribute__((aligned(PAGE_SIZE)));

// 内核页表区域
// 128张页表，pte_kern[i]表示第i张页表，是pte_t数组
static pte_t pte_kern[PTE_COUNT][PTE_SIZE] __attribute__((aligned(PAGE_SIZE)));

void init_vmm()
{
	// 0xC0000000 3GB虚拟地址在页目录的索引
	uint32_t kern_pte_first_idx=PGD_INDEX(PAGE_OFFSET);
	
	// 对128个页目录项设置为相应页表基地址（物理地址）
	uint32_t i, j;
	for(i=kern_pte_first_idx, j=0; i<PTE_COUNT+kern_pte_first_idx; i++, j++)
	{
		// 此处是内核虚拟地址，MMU 需要物理地址，所以减去偏移，下同
		pgd_kern[i]=((uint32_t)pte_kern[j] - PAGE_OFFSET) | PAGE_PRESENT | PAGE_WRITE;
	}
	
	// 对128*1024个页表项设置为指向连续4KB的物理页面的首地址
	uint32_t *pte=(uint32_t*) pte_kern;
	// 不映射第0页，便于跟踪NULL指针
	for(i=1; i<PTE_COUNT * PTE_SIZE; i++)
	{
		pte[i]=(i<<12) | PAGE_PRESENT | PAGE_WRITE;
	}

	
	// 注册页错误中断的处理函数（14是页故障的中断号）
	register_interrupt_handler(14, &page_fault);
	
	// 更换页目录
	uint32_t pgd_kern_phy_addr=(uint32_t) pgd_kern - PAGE_OFFSET;
	switch_pgd(pgd_kern_phy_addr);
}

// 更换当前的页目录
void switch_pgd(uint32_t pd)
{
	asm volatile("mov %0, %%cr3" : : "r" (pd));
}

// 使用flags指出的页权限，把虚拟地址va映射到物理地址pa
void map(pgd_t *pgd_now, uint32_t va, uint32_t pa, uint32_t flags)
{
	uint32_t pgd_idx=PGD_INDEX(va);
	uint32_t pte_idx=PTE_INDEX(va);
	
	// 页表地址
	pte_t *pte=(pte_t*) (pgd_now[pgd_idx] & PAGE_MASK);
	if(!pte)
	{
		// 若页表不存在，分配一个页面，并添加一个页目录项
		pte=(pte_t*) pmm_alloc_page();
		pgd_now[pgd_idx]=(uint32_t) pte | PAGE_PRESENT | PAGE_WRITE;
		
		// 转换到内核线性地址并清0
		pte=(pte_t*) ((uint32_t) pte + PAGE_OFFSET);
		bzero(pte, PAGE_SIZE);
	}
	else
	{
		// 转换到内核线性地址
		pte=(pte_t*)((uint32_t)pte + PAGE_OFFSET);
	}
	
	pte[pte_idx]=(pa &  PAGE_MASK) | flags;
	
	// 通知CPU更新页表缓存
	asm volatile("invlpg (%0)" : : "a" (va));
}

void unmap(pgd_t *pgd_now, uint32_t va)
{
	uint32_t pgd_idx=PGD_INDEX(va);
	uint32_t pte_idx=PTE_INDEX(va);
	
	pte_t *pte=(pte_t*)(pgd_now[pgd_idx] & PAGE_MASK);
	
	if(!pte)
		return;
	
	// 转换到内核线性地址
	pte=(pte_t*)((uint32_t) pte + PAGE_OFFSET);
	
	
	pte[pte_idx]=0;
	
	// 通知CPU更新页表缓存
	asm volatile("invlpg (%0)" : : "a" (va));
}

// 如果虚拟地址va映射到物理地址则返回1
// 同时如果pa不是空指针则把物理地址写入pa参数
uint32_t get_mapping(pgd_t *pgd_now, uint32_t va, uint32_t *pa)
{
	uint32_t pgd_idx=PGD_INDEX(va);
	uint32_t pte_idx=PTE_INDEX(va);
	
	pte_t *pte=(pte_t*) (pgd_now[pgd_idx]  &  PAGE_MASK);
	if(!pte)
		return 0;
		
	// 转换到内核线性地址
	pte=(pte_t*) ((uint32_t) pte + PAGE_OFFSET);
	
	// 如果地址有效而且指针不为NULL，则返回地址
	if(pte[pte_idx] !=0  &&  pa)
	{
		*pa=pte[pte_idx]  &  PAGE_MASK;
		return 1;
	}
	
	return 0;
}

