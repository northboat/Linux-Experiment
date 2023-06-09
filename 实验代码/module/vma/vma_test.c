#include <linux/module.h> 
#include <linux/init.h> 
#include <linux/mm.h> 
#include <linux/sched.h> 
 
static int pid; 
module_param(pid, int, 0644); 
 
static void printit(struct task_struct *tsk) { 
	struct mm_struct *mm; 
	struct vm_area_struct *vma; 
 	int j = 0; 
 	unsigned long start, end, length; 
 
	mm = tsk->mm; 
	pr_info("mm_struct addr = 0x%p\n", mm); 
 	vma = mm->mmap; 

	/* 使用 mmap_sem 读写信号量进行保护 */ 
	down_read(&mm->mmap_sem); 
	pr_info("vmas: vma start end length\n");
    
 	while (vma) { 
 		j++; 
  		start = vma->vm_start; 
 		end = vma->vm_end; 
 		length = end - start; 
 		pr_info("%6d: %16p %12lx %12lx %8ld\n", 
 				j, vma, start, end, length); 
 		vma = vma->vm_next; 
 	}
	up_read(&mm->mmap_sem); 
} 

static int __init vma_init(void) { 
 	struct task_struct *tsk; 
 	/* 如果插入模块时未定义 pid 号，则使用当前 pid */ 
 	if (pid == 0) { 
 		tsk = current; 
 		pid = current->pid; 
 		pr_info("using current process\n"); 
    } else { 
		tsk = pid_task(find_vpid(pid), PIDTYPE_PID); 
	} 
	if (!tsk) 
		return -1; 
	pr_info(" Examining vma's for pid=%d, command=%s\n", pid, tsk->comm); 
	printit(tsk); 
	return 0; 
} 

static void __exit vma_exit(void) { 
	pr_info("Module exit\n"); 
}

module_init(vma_init); 
module_exit(vma_exit); 
MODULE_LICENSE("GPL"); 
MODULE_AUTHOR("Mr Yu"); 
MODULE_DESCRIPTION("vma test"); 
