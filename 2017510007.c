#include <linux/init.h> 
#include <linux/module.h> //try_module_get, module_put
#include <linux/kernel.h>// Needed for KERN_INFO
#include <linux/fs.h> //file_operations
#include <asm/uaccess.h> //memory access
#include <linux/sched/signal.h> 
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <linux/proc_fs.h>
#include <linux/syscalls.h>

////what i wrote inside syscalls.h /////////////
// asmlinkage long sys_2017510007(void);
// asmlinkage long sys_mysyscall(char *arg, char *pid, char *userBuf);

/////what i wrote inside syscall_64.tbl//////////////
//335	64	2017510007		sys_2017510007
//336	common	mysyscall		__x64_sys_mysyscall

// I got errors trying to compile. 
//Although I researched and looked at
//the forum part on Sakai but unfortunately 
//I could not figure out. The errors I received 
//are in the error.png screenshot.


//system call define 
SYSCALL_DEFINE3(mysyscall, char __user *, arg, char __user *, pid, char __user *, userB)
{
    int return_val = 0;
	char param_arg[16] = {'\0'};
	int pNum = toInt(pid);
	if(copy_from_user(param_arg, arg, 16) == 0)
		printk(KERN_INFO "Argument is taken!");

    //functions work according to the arguments entered
	if(stringcheck(param_arg,"-all") == 0)                             //if entered argument is -all
		return_val = all();
	else if( stringcheck(param_arg,"-p") == 0)                         //if entered argument is -p
		return_val = cmdline();
	else if( stringcheck(param_arg,"-k") == 0)                         //if entered argument is -k
		return_val = kill_proc();
	else
		return_val = no_argument();   //if there is no argument
    
    return_val = copy_to_user(userB, param_arg, 16);
    if(return_val == 0){
    printk(KERN_INFO "%s\nhas been copied to usr_write_back_dst!", param_arg);
    }

	return return_val;
}
////////////////////////////////////////////
//without argument function
int no_argument(void)
{    
   printk("Right Usage:\n-all prints some information (process id and its argument/s) about all processes\n-p takes process id and prints the details of it\n-k takes process id\n");
    return 0;
}
////////////////////////////////////
//cmdline function
int cmdline(char __user *, pid)
{
  struct file *f;

    char buf[128];

    mm_segment_t fs;
    int i;

    //The part of placing the incoming id in the path with using strcpy and strcat
    char first[50], second[50];
    strcpy(first, "/proc/");
    strcat (first, pid);
    strcpy(second, "/stat");
    strcat(first, second )
    for(i=0;i<128;i++)
        buf[i] = 0;
    printk(KERN_INFO "My module is loaded\n");
    f = filp_open(first, O_RDONLY, 0);

    if(f == NULL)
        printk(KERN_ALERT "filp_open error!!.\n");
    else{
        fs = get_fs();
        set_fs(get_ds());
        f->f_op->read(f, buf, 128, &f->f_pos);
        set_fs(fs);

        //printk(KERN_INFO "buf:%s\n",buf);

        int unused;
        char comm[1000];
        int pid;
        char state;
        int ppid;
        sscanf(buf, "%s", comm);
        printk(KERN_INFO "cmdline = %s\n", comm);
    }

    filp_close(f,NULL);

    return 0;

}
///////////////////////////////////////////////
//all function
char buffer[256];
char * get_task_state(long state){
    printk(buffer, "Unknow type: %ld\n", state);
    return buffer;
}
int all(void){

    //Reaching ids with task_struct pattern
	struct task_struct *task_list;
	for_each_process(task_list){
    		pr_info("UID:[%d]\t  PPID:[%d]\t  PID:[%d]\t Process: %s\t  State:%ld\n",
            	task_list->real_cred->uid.val, task_ppid_nr(task_list), task_list->pid, task_list->comm, 
            	get_task_state(task_list->state));	
	}
	return 0;
}
MODULE_LICENSE("GPL");
////////////////////////////////////////////

//kill function
int kill_proc(char __user *, pid)
{
    int pnum = toInt(pid);
    printk(KERN_INFO "My module is loaded\n");
    if(kill_pid(find_vpid(pnum),SIGKILL,1) == 0)
    {
        printk(KERN_INFO "Process %d killed\n", pnum);
    }

    return 0;
}
//////////////////////////////////////////////////

// cleanup module
void cleanup_module(void) 
{ printk(KERN_INFO "My module is unloaded\n"); 
}