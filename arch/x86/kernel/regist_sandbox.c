#include<linux/kernel.h>
#include<linux/list.h>
#include<linux/errno.h>
#include<linux/slab.h>
#include<linux/sched.h>

struct pid_list {
	int pid;
	struct list_head list;	
};


struct syscall_num_list {
	int syscall_num;
	struct list_head list;
};


struct pid_list p_head = {
	.pid = -1,
};


struct syscall_num_list s_head = {
	.syscall_num = -1,
};


asmlinkage void sys_nothing(void){
	// nothing
}


asmlinkage struct pid_list *search_pid(int pid){
	struct list_head *tmp_ptr;
	struct pid_list *entry_ptr;
	
	printk("search_pid : called\n");
	
	list_for_each(tmp_ptr,&p_head.list){
		entry_ptr = list_entry(tmp_ptr,struct pid_list,list);
		if(entry_ptr->pid == pid){
			printk("search_pid : hit entry_ptr\n");
			return entry_ptr;
		}
	}

	printk("search_pid : no hit entry_ptr\n");
	return NULL;
}


asmlinkage int sys_regist_pid(int pid){
	struct pid_list *add_ptr;

	printk("sys_regist_pid : called\n");
	
	add_ptr = search_pid(pid);

	if(add_ptr != NULL){
		printk("sys_regist_pid : fail adding pid(%d)\n",pid);
		return -1;
	}else{
		add_ptr = kmalloc(sizeof(struct pid_list),GFP_KERNEL);
		add_ptr->pid = pid;
		list_add(&add_ptr->list,&p_head.list);
		printk("sys_regist_pid : success adding pid(%d)\n",pid);
	}

	return 0;
}


asmlinkage int sys_unregist_pid(int pid){
	struct pid_list *del_ptr;
	struct pid_list *tmp_ptr;

	printk("sys_unregist_pid : called\n");
	
	del_ptr = search_pid(pid);

	if(del_ptr != NULL){
		tmp_ptr = del_ptr;
		list_del(&del_ptr->list);
		printk("sys_unregist_pid : success removing pid(%d)\n",pid);
		kfree(tmp_ptr);
	}else{
		printk("sys_unregist_pid : fail removing pid(%d)\n",pid);
		return -1;
	}

	return 0;
}


asmlinkage struct syscall_num_list *search_syscall_num(int syscall_num){
	struct list_head *tmp_ptr;
	struct syscall_num_list *entry_ptr;
	
	printk("search_syscall_num : called\n");
	
	list_for_each(tmp_ptr,&s_head.list){
		entry_ptr = list_entry(tmp_ptr,struct syscall_num_list,list);
		if(entry_ptr->syscall_num == syscall_num){
			printk("search_syscall_num : hit entry_ptr\n");
			return entry_ptr;
		}
	}

	printk("search_syscall_num : no hit entry_ptr\n");
	return NULL;
}


asmlinkage int sys_regist_syscall(int syscall_num){
	struct syscall_num_list *add_ptr;

	printk("sys_regist_syscall : called\n");
	
	add_ptr = search_syscall_num(syscall_num);

	if(add_ptr != NULL){
		printk("sys_regist_syscall : fail adding syscall number(%d)\n",syscall_num);
		return -1;
	}else{
		add_ptr = kmalloc(sizeof(struct syscall_num_list),GFP_KERNEL);
		add_ptr->syscall_num = syscall_num;
		list_add(&add_ptr->list,&s_head.list);
		printk("sys_regist_syscall : success adding syscall number(%d)\n",syscall_num);
	}

	return 0;
}


asmlinkage int sys_unregist_syscall(int syscall_num){
	struct syscall_num_list *del_ptr;
	struct syscall_num_list *tmp_ptr;

	printk("sys_unregist_syscall : called\n");
	
	del_ptr = search_syscall_num(syscall_num);

	if(del_ptr != NULL){
		tmp_ptr = del_ptr;
		list_del(&del_ptr->list);
		printk("sys_unregist_syscall : success removing syscall number(%d)\n",syscall_num);
		kfree(tmp_ptr);
	}else{
		printk("sys_unregist_syscall : fail removing syscall number(%d)\n",syscall_num);
		return -1;
	}

	return 0;
}


int check_sandbox(int syscall_num){
	struct list_head *tmp_pptr,*tmp_sptr;
	struct pid_list *entry_pptr;
	struct syscall_num_list *entry_sptr;

	int pid = current->tgid;
	int detect_pflag = 0;
	int detect_sflag = 0;
	
	if(p_head.list.next == NULL && p_head.list.prev == NULL && s_head.list.next == NULL && s_head.list.prev == NULL){
		INIT_LIST_HEAD(&p_head.list);
		INIT_LIST_HEAD(&s_head.list);
		printk("check_sandbox : init_list_head about pid_list and syscall_num_list\n");
	}

	list_for_each(tmp_pptr,&p_head.list){
		entry_pptr = list_entry(tmp_pptr,struct pid_list,list);
		if(entry_pptr->pid == pid){
			detect_pflag = detect_pflag | 1;
		}
	}

	if(detect_pflag){
		list_for_each(tmp_sptr,&s_head.list){
			entry_sptr = list_entry(tmp_sptr,struct syscall_num_list,list);
			if(entry_sptr->syscall_num == syscall_num){
				detect_sflag = detect_sflag | 1;
			}
		}
	}

	if(detect_sflag & detect_pflag){
		printk("check_sandbox : detect target pid(%d) and syscall num(%d)\n",pid,syscall_num);
		detect_pflag = detect_pflag & 0;
		detect_sflag = detect_sflag & 0;
		return -1;
	}
	
	return 0;
}

