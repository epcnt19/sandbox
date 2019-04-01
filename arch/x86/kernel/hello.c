#include<linux/kernel.h>

void hello(int number){
	int getpid_syscall_num = 39;
	int testgetpid_syscall_num = 336;

	if (number == testgetpid_syscall_num){
		printk("hello : testgetpid_syscall_num : %d\n",number);
	}

	//return 0;
}
