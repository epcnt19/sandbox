#include<linux/kernel.h>
#include<linux/sched.h>

asmlinkage int sys_test_getpid(void){
	//printk("called sys_test_getpid\n");
	return task_tgid_vnr(current);
}

