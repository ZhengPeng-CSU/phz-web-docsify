我们都知道，再linux中，进程是通过运行可执行文件得到，可执行文件也就是elf文件。linux中的进程都是通过fork得到的，那么每一个进程到低是怎么开始运行的呢？

# 0号进程
0号进程是linux启动的第一个进程，当系统中所有的进程起来后，0号进程也就蜕化为idle进程，当一个core上没有任务可运行时就会去运行idle进程。
0号进程同样也有task_struct定义，它是一个全局变量，定义在linux-master\init\init_task.c中，在我看的这个版本里面已经没有INIT_TASK这个宏了，而是直接定义的。
为什么说0号进程会退化成idle进程呢，我们先来简单理解一下内核的启动过程。大致把Linux启动流程分成，BIOS，Boot Loader，内核初始化。
（1）BIOS
BIOS是预先安装在主板上的固件，BIOS控制计算机硬件。BIOS在启动过程中的主要工作是硬件初始化，最主要的工作是POST(Power On Self Test，硬件自检)。POST测试主要是检查一些硬件的读写是否正常。POST主要不是用来做硬件诊断，它只是检查系统硬件是否存在并且加载一个BootLoader。
（2）boot loader
Master Boot Record, 是boot loader启动的起始位置，MBR是磁盘上第一个扇区。读取并执行第一个启动设备内 MBR 的 boot Loader（现在的 Linux 系统一般使用 grub2）。
boot loader自己的运行和加载启动内核镜像都是非常复杂的逻辑，目前不是我的学习重点，只需简单知道boot loader最终会调用内核的start_kernel接口，我理解调用到这个接口以后就是0号进程做的工作了。
（3）内核初始化
boolload最终会调用内核的start_kernel接口，完成内核初始化，这其中就包括0号idle进程转变，1号和2号进程创建。当0号进程执行完start_kernel的逻辑以后，就会创建1号进程和2号进程，然后自身转成idle进程，大概的调用逻辑是start_kernel -> arch_call_rest_init -> rest_init -> cpu_startup_entry -> do_idle，do_idle里面就是一个while循环了，也就是退化成idle进程了。

# 1号进程
1号和2号进程都是由0号进程创建的，在rest_init中，需要注意的是，1号进程最开始也是通过kernel_thread创建，而非fork创建，创建完以后1号进程还是个内核态进程，之后会从内核态转到用户态，后续所有用户态进程都是通过1号进程fork出来的。
```C++
noinline void __ref rest_init(void)
{
	struct task_struct *tsk;
	int pid;

	rcu_scheduler_starting();
	/*
	 * We need to spawn init first so that it obtains pid 1, however
	 * the init task will end up wanting to create kthreads, which, if
	 * we schedule it before we create kthreadd, will OOPS.
	 */
	pid = kernel_thread(kernel_init, NULL, CLONE_FS);
	/*
	 * Pin init on the boot CPU. Task migration is not properly working
	 * until sched_init_smp() has been run. It will set the allowed
	 * CPUs for init to the non isolated CPUs.
	 */
	rcu_read_lock();
	tsk = find_task_by_pid_ns(pid, &init_pid_ns);
	tsk->flags |= PF_NO_SETAFFINITY;
	set_cpus_allowed_ptr(tsk, cpumask_of(smp_processor_id()));
	rcu_read_unlock();

	numa_default_policy();
	pid = kernel_thread(kthreadd, NULL, CLONE_FS | CLONE_FILES);
	rcu_read_lock();
	kthreadd_task = find_task_by_pid_ns(pid, &init_pid_ns);
	rcu_read_unlock();

	/*
	 * Enable might_sleep() and smp_processor_id() checks.
	 * They cannot be enabled earlier because with CONFIG_PREEMPTION=y
	 * kernel_thread() would trigger might_sleep() splats. With
	 * CONFIG_PREEMPT_VOLUNTARY=y the init task might have scheduled
	 * already, but it's stuck on the kthreadd_done completion.
	 */
	system_state = SYSTEM_SCHEDULING;

	complete(&kthreadd_done);

	/*
	 * The boot idle thread must execute schedule()
	 * at least once to get things moving:
	 */
	schedule_preempt_disabled();
	/* Call into cpu_idle with preempt disabled */
	cpu_startup_entry(CPUHP_ONLINE);
}
```
kernel_thread的具体逻辑暂且先不管，我们重点看看1号进程是怎么转到用户态运行的，1号进程创建是pid = kernel_thread(kernel_init, NULL, CLONE_FS)，显然此时它还是个内核进程。
```C++
static int __ref kernel_init(void *unused)
{
	int ret;

	// 等待kthread线程启动完毕
	wait_for_completion(&kthreadd_done);

	kernel_init_freeable();
	......

	if (ramdisk_execute_command) {
		ret = run_init_process(ramdisk_execute_command);
		if (!ret)
			return 0;
		pr_err("Failed to execute %s (error %d)\n",
		       ramdisk_execute_command, ret);
	}

	/*
	 * We try each of these until one succeeds.
	 *
	 * The Bourne shell can be used instead of init if we are
	 * trying to recover a really broken machine.
	 */
	if (execute_command) {
		ret = run_init_process(execute_command);
		if (!ret)
			return 0;
		panic("Requested init %s failed (error %d).",
		      execute_command, ret);
	}

	if (CONFIG_DEFAULT_INIT[0] != '\0') {
		ret = run_init_process(CONFIG_DEFAULT_INIT);
		if (ret)
			pr_err("Default init %s failed (error %d)\n",
			       CONFIG_DEFAULT_INIT, ret);
		else
			return 0;
	}

	if (!try_to_run_init_process("/sbin/init") ||
	    !try_to_run_init_process("/etc/init") ||
	    !try_to_run_init_process("/bin/init") ||
	    !try_to_run_init_process("/bin/sh"))
		return 0;

	panic("No working init found.  Try passing init= option to kernel. "
	      "See Linux Documentation/admin-guide/init.rst for guidance.");
}
```
这里调用了kernel_init_freeable，我们看下这个里面的逻辑，
```C++
static noinline void __init kernel_init_freeable(void)
{
	/* Now the scheduler is fully set up and can do blocking allocations */
	gfp_allowed_mask = __GFP_BITS_MASK;

	/*
	 * init can allocate pages on any node
	 */
	set_mems_allowed(node_states[N_MEMORY]);

	cad_pid = get_pid(task_pid(current));

	smp_prepare_cpus(setup_max_cpus);

	workqueue_init();

	init_mm_internals();

	rcu_init_tasks_generic();
	do_pre_smp_initcalls();
	lockup_detector_init();

	smp_init();
	sched_init_smp();

	padata_init();
	page_alloc_init_late();
	/* Initialize page ext after all struct pages are initialized. */
	page_ext_init();

	do_basic_setup();

	kunit_run_all_tests();

	wait_for_initramfs();
	console_on_rootfs();

	/*
	 * check if there is an early userspace init.  If yes, let it do all
	 * the work
	 */
	if (init_eaccess(ramdisk_execute_command) != 0) {
		ramdisk_execute_command = NULL;
		prepare_namespace();
	}

	/*
	 * Ok, we have completed the initial bootup, and
	 * we're essentially up and running. Get rid of the
	 * initmem segments and start the user-mode stuff..
	 *
	 * rootfs is available now, try loading the public keys
	 * and default modules
	 */

	integrity_load_keys();
}
```
这里判断ramdisk_execute_command对应的文件是否存在，如果不存在则调用prepare_namespace，目前还看不大明白。转回到kernel_init_freeable，假如ramdisk_execute_command存在，就会通过run_init_process来运行，否则就会尝试运行内核默认的几个程序。try_to_run_init_process也是调用run_init_process，我们看这个逻辑，不妨假设init_filename就是/sbin/init，我目前的环境上可以看到它就是个软链接
lrwxrwxrwx 1 root root 20 3月  15  2023 /sbin/init -> /lib/systemd/systemd

```C++
static int run_init_process(const char *init_filename)
{
	const char *const *p;

	argv_init[0] = init_filename;
	pr_info("Run %s as init process\n", init_filename);
	pr_debug("  with arguments:\n");
	// init参数
	for (p = argv_init; *p; p++)
		pr_debug("    %s\n", *p);
	pr_debug("  with environment:\n");
	// init环境变量
	for (p = envp_init; *p; p++)
		pr_debug("    %s\n", *p);
	return kernel_execve(init_filename, argv_init, envp_init);
}
```
往下调用kernel_execve，kernel_execve逻辑比较复杂，暂且不关注，它是用来在内核空间执行一个可执行文件。

# 2号进程
在rest_init中，创建完1号进程后会创建2号进程pid = kernel_thread(kthreadd, NULL, CLONE_FS | CLONE_FILES);
kthreadd是内核所有线程的祖先，内核线程的创建都是委托这个线程来完成的。
```C++
int kthreadd(void *unused)
{
	struct task_struct *tsk = current;

	/* Setup a clean context for our children to inherit. */
	set_task_comm(tsk, "kthreadd");
	ignore_signals(tsk);
	set_cpus_allowed_ptr(tsk, housekeeping_cpumask(HK_FLAG_KTHREAD));
	set_mems_allowed(node_states[N_MEMORY]);

	current->flags |= PF_NOFREEZE;
	cgroup_init_kthreadd();

	for (;;) {
		// 将当前状态设置为可中断
		set_current_state(TASK_INTERRUPTIBLE);
		// 如果没有线程需要创建，则主动出让cpu
		if (list_empty(&kthread_create_list))
			schedule();
		// 有线程需要创建，更新运行状态
		__set_current_state(TASK_RUNNING);

		spin_lock(&kthread_create_lock);
		while (!list_empty(&kthread_create_list)) {
			struct kthread_create_info *create;

			create = list_entry(kthread_create_list.next,
					    struct kthread_create_info, list);
			list_del_init(&create->list);
			spin_unlock(&kthread_create_lock);
			// 创建线程
			create_kthread(create);

			spin_lock(&kthread_create_lock);
		}
		spin_unlock(&kthread_create_lock);
	}

	return 0;
}
```
在create_kthread函数中会通过调用kernel_thread函数来创建新进程，且新进程的执行函数为kthread。新线程创建成功后，会一直睡眠（使用schedule主动让出CPU并睡眠），直到有人唤醒它（wake_up_process）;线程被唤醒后，并且不需要stop, 则执行指定的函数体（ threadfn(data) ）。

## 新进程的创建过程
我们都应该听过，linux下创建一个用户态新的进程是通过fork从父进程创建一个子进程，然后通过execve来执行子进程的程序，这里我们也主要来学习用户态进程创建的过程。
linux下fork对应的接口有三个：fork, vfork, clone

### vfork
vfork创建的子进程会先运行，只有当子进程退出以后原来的进程才会继续运行，且vfork创建的子进程会和父进程完全共享进程内存空间 比如下面这段代码。
```C++
int main() {
    std::cout << "main begin" << std::endl;
    int val = 666;
    auto pid = vfork();
    if (pid < 0) {
        std::cout << "vfork failed" << std::endl;
    } else if (pid == 0) {
        std::cout << "this is child" << std::endl;
        val = 777;
        std::cout << "val in child " << val << std::endl;
    } else {
        std::cout << "this is father" << std::endl;
        std::cout << "val in child " << val << std::endl;
    }

    std::cout << "main end" << std::endl;
    exit(0);
}
```
在子进程中修改变量，父进程中变量的值也随之改变。另外，特别需要注意的是，vofk创建的子进程必须通过exit的方式退出，而不能通过return退出，因为return以后会回收堆栈资源，而父进程再运行时访问或者退出又会回收一次，从而导致错误。
不过我目前接触的业务场景集中，还没有用过vfork，日后用到了再深入了解一下。

### fork
fork创建的子进程会复制父进程的页表，但是不新分配物理内存，将父子进程对应的页表项修改为只读，当有一方试图写共享的物理页，由于页表项属性是只读的会发生COW缺页异常，缺页异常处理程序会为写操作的一方分配新的物理页，并将原来共享的物理页内容拷贝到新页，然后建立新页的页表映射关系，这样写操作的进程就可以继续执行，不会影响另一方。
我们沿着glibc中fork的代码实现来看下具体过程，打上断点可以看到调用的是__libc_fork
#0  __libc_fork () at ../sysdeps/nptl/fork.c:49
#1  0x000055555555522c in main ()
```C++
pid_t
__libc_fork (void)
{
  if (multiple_threads)
    {
      call_function_static_weak (__nss_database_fork_prepare_parent,
				 &nss_database_data);

      _IO_list_lock ();

      /* Acquire malloc locks.  This needs to come last because fork
	 handlers may use malloc, and the libio list lock has an
	 indirect malloc dependency as well (via the getdelim
	 function).  */
      call_function_static_weak (__malloc_fork_lock_parent);
    }

  pid_t pid = _Fork ();
...
}
```
