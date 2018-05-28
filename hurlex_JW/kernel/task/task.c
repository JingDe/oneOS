#include"gdt.h"
#include"pmm.h"
#include"vmm.h"
#include"heap.h"
#include"task.h"
#include"sched.h"
#include"string.h"
#include"debug.h"

// 全局pid值
pid_t now_pid=0;

// 内核线程创建
int32_t kernel_thread(int (*fn)(void*), void *arg)
{
	struct task_struct *new_task=(struct task_struct*) kmalloc(STACK_SIZE);//8192
	assert(new_task!=NULL, "kern_thread: kmalloc error");
	
	// 将栈低端结构信息初始化为0
	bzero(new_task, sizeof(struct task_struct));
	
	new_task->state=TASK_RUNNABLE;
	new_task->stack=current;				//??
	new_task->pid=now_pid++;
	new_task->mm=NULL;
	
	uint32_t *stack_top=(uint32_t*)((uint32_t) new_task + STACK_SIZE);
	
	*(--stack_top)=(uint32_t)kthread_exit;
	*(--stack_top)=(uint32_t)arg;	
	*(--stack_top)=(uint32_t)fn; 
	// *(--stack_top)=(uint32_t)kthread_exit; //否则，切换到该线程将弹出执行函数地址kthread_exit
	
	new_task->context.esp=(uint32_t)new_task+STACK_SIZE-sizeof(uint32_t)*3;
	/* switch_to.s的ret指令返回之前，执行现场已被切换，esp指针指向的栈已被切换。
	所以ret指令弹出的返回地址就是另一个执行流之前调用schedule之前保存的返回地址了。
	这里就是构造出一个切换后可以弹出执行地址的初始栈来实现调用fn */
	
	// 设置新任务的标志寄存器未屏蔽中断
	new_task->context.eflags=0x200;
	
	// 将new_task插入到running_proc_head末尾
	new_task->next=running_proc_head;
	
	struct task_struct *tail=running_proc_head;
	assert(tail !=NULL, "Must init sched!");
	
	while(tail->next != running_proc_head)
		tail=tail->next;		
	tail->next=new_task;
	
	return new_task->pid;
}

void kthread_exit()
{
	register uint32_t val asm("eax");
	printk("Thread exited with value %d\n", val);
	while(1);
}
