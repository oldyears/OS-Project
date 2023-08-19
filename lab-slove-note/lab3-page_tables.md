# 实验笔记

## 实验内容

### Print a page table

+ 实验准备
  + 阅读xv6 book Chapter 3
  + 内存布局代码：`kern/memlayout.h`
  + 大多数虚拟内存代码：`kern/vm.c`
  + 分配和释放物理内存代码：`kernel/kalloc.c`

+ 实验要求
  + 为了帮助您了解 RISC-V 页表，或许也为了帮助将来的调试，您的第一个任务是编写一个打印页表内容的函数。
  + 定义一个名为 vmprint() 的函数。它应该采用 pagetable_t 参数，并以下面描述的格式打印该页表。在 exec.c 中 return argc 之前插入 if(p->pid==1) vmprint(p->pagetable) ，以打印第一个进程的页表。如果您通过了 PTE 成绩打印输出测试，您将获得本作业的满分。
  + 第一行显示 vmprint 的参数。之后，每个 PTE 都有一行，包括引用树中更深层次的页表页面的 PTE。每条 PTE 行都缩进了一些“..”，表示其在树中的深度。每条 PTE 行显示其页表页中的 PTE 索引、pte 位以及从 PTE 中提取的物理地址。不要打印无效的 PTE。在上面的示例中，顶级页表页具有条目 0 和 255 的映射。条目 0 的下一级仅映射索引 0，而该索引 0 的底层具有条目 0、1 和 2 映射。您的代码可能会发出与上面显示的不同的物理地址。条目数和虚拟地址数应该相同。
  + 一些提示
    + 把`vmprint()`放在`kernel/vm.c`
    + 使用`kernel/riscv.h`末尾的宏
    + `freewlk`功能可能很不错
    + 在`kernel.defs.h`中定义`vmprint`的原型，以便可以在`exec.c`中调用它
    + 在`printf`调用中使用`%p`打印出完整的64位十六进制PTE和地址

+ 实验实现
  + 首先在`kernel/vm.c`中申明`vmprint()`函数

    + 此函数需要适用递归进行，根据不同的层级进行不同的处理，其`PTE2PA`代表将此`pagetable_t`向右取三位，其递归函数如下

    + ````C
      static void
      recur_pt(pagetable_t pt ,int level)
      {
        // 遍历
        for(int i = 0;i < 512; i++)
        {
          pte_t pte = pt[i];
          if(pte & PTE_V)
          {
            uint64 child = PTE2PA(pte);
            if(level == 0)
            {
              printf("..%d: pte %p pa %p\n", i, pte, child);
              recur_pt((pagetable_t)child, level + 1);
            }
            else if(level == 1)
            {
              printf(".. ..%d: pte %p pa %p\n", i, pte, child);
              recur_pt((pagetable_t)child, level + 1);
            }
            else
              printf(".. .. ..%d: pte %p pa %p\n", i, pte, child);
          }
        }
      }
      ````

    + 在`vmprint()`函数中进行递归调用即可

    + ````c
      int vmprint(pagetable_t pagetable)
      {
        // 首先打印第一行
        printf("page table %p\n",pagetable);
      
        // 递归打印PTE部分
        recur_pt(pagetable, 0);
        return 0;
      }
      ````

  + 在`kernel/def.h`中添加`vmprint()`的声明——`void vmprint(pagetable_t)`

    + 在`kernel/exec.c`中增加对`vmprint()`的调用

    + ````c
      if(p->pid == 1)
          vmprint(p->pagetable);
      ````

    + 

  + 最后运行结果如标准结果所示



### A kernel page table per process

+ 实验要求

  + 无论何时在内核执行时，xv6使用同一个内核页表。内核页表是一个物理地址的直接映射，因此内核虚拟地址x对应物理地址x。
  + xv6也有一个单独的页表给每个进程的用户地址空间，仅包含那个进程用户内存的映射，起始于虚拟地址0。
  + 因为内核页表不包含这些映射，用户地址在内核无效。因此，当内核需要使用一个用户指针传到system call时，内核必须首先翻译指针到物理地址。
  + 这个和下个实验的目的是为了允许内核直接解析用户指针。
  + 第一个任务是更改内核，为了当在内核执行时，每个进程使用它自己的内核页表拷贝。
  + 更改struct proc来让每个进程保持一个内核页表，更改scheduler()，当切换进程时切换内核页表。
  + 对于这一步，每个进程的内核页表应该和已存在的全局内核页表完全相同。
  + 读xv6书、理解作业一开始提到的代码如何起作用，将更容易正确地更改虚拟内存代码。
  + 页表设置中的bug会因为缺少映射导致缺陷，导致加载、存储会影响到不可预期的物理内存页，也会导致执行不正确的物理内存页。

+ 提示

  + 在 struct proc 中为进程的内核页表添加一个字段。
  + 为新进程创建内核页表的一个合理方法是实现一个修改过的 kvminit 版本，它可以创建一个新的页表，而不是修改 kernel_pagetable。你需要在 allocproc 中调用这个函数。
  + 确保每个进程的内核页表都有该进程内核堆栈的映射。在未修改的 xv6 中，所有内核堆栈都在 procinit 中设置。你需要将部分或全部功能移至 allocproc。
  + 修改 scheduler()，将进程的内核页表加载到内核的 satp 寄存器中（请参阅 kvminithart 以获得灵感）。调用 w_satp() 后，不要忘记调用 sfence_vma()。
  + 当没有进程运行时，scheduler() 应使用 kernel_pagetable。
  + 在 freeproc 中释放进程的内核页表。
  + 您需要一种方法来释放页表，而不同时释放物理内存页。
  + vmprint 可能会在调试页表时派上用场。
  + 修改 xv6 函数或添加新函数没有问题；你可能至少需要在 kernel/vm.c 和 kernel/proc.c 中这样做（但不要修改 kernel/vmcopyin.c、kernel/stats.c、user/usertests.c 和user/stats.c）。
  + 缺少页表映射可能会导致内核遇到页面故障。内核将打印包含sepc=0x00000000XXXXXXXX 的错误信息。通过在 kernel/kernel.asm 中搜索 XXXXXXXX，可以找出故障发生的位置。

+ 具体实现

  + 在`kernel/proc.h`的`struct proc`新添加`kernelPagetable`

  + 修改`kernel/vm.c`新添加`vmmake()`方法来创建新的内核页表

    + ````c
      pagetable_t vmmake()
      {
        pagetable_t kernel_pt = (pagetable_t)kalloc();
        memset(kernel_pt, 0, PGSIZE);
      
        mappages(kernel_pt, UART0, PGSIZE, UART0, PTE_R | PTE_W);
      
        mappages(kernel_pt, VIRTIO0, PGSIZE, VIRTIO0, PTE_R | PTE_W);
      
        mappages(kernel_pt, PLIC, 0x400000, PLIC, PTE_R | PTE_W);
      
        mappages(kernel_pt, KERNBASE, (uint64)etext - KERNBASE, KERNBASE, PTE_R | PTE_X);
      
        mappages(kernel_pt, (uint64)etext, PHYSTOP - (uint64)etext, (uint64)etext, PTE_R | PTE_W);
      
        mappages(kernel_pt, TRAMPOLINE, PGSIZE, (uint64)trampoline, PTE_R | PTE_X);
      
        return kernel_pt;
      }
      ````

  + 在`kernel/vm.c`，修改`vminit()`方法，内部由之前的`vmmake()`实现，此处实现全局内核页表创建过程，并进行CLINT的映射

    + ````c
      void kvminit()
      {
        kernel_pagetable = vmmake();
      
        // CLINT
        mappages(kernel_pagetable, CLINT, 0x10000, CLINT, PTE_R | PTE_W);
      }
      ````

  + 在`kernel/proc.c`，修改`procinit()`函数，不再用此函数为每个进程分配内核栈

    + ````c
      void
      procinit(void)
      {
        struct proc *p;
        
        initlock(&pid_lock, "nextpid");
        for(p = proc; p < &proc[NPROC]; p++) {
            initlock(&p->lock, "proc");
      
            // Allocate a page for the process's kernel stack.
            // Map it high in memory, followed by an invalid
            // guard page.
            // char *pa = kalloc();
            // if(pa == 0)
            //   panic("kalloc");
            // uint64 va = KSTACK((int) (p - proc));
            // kvmmap(va, (uint64)pa, PGSIZE, PTE_R | PTE_W);
            // p->kstack = va;
        }
        // kvminithart();
      }
      ````

  + 在`kernel/proc.c`，修改`allocproc()`，在此处创建内核页表，并在内核页表上分配一个内核栈

    + ````
      // create a pagetable
        p->kernelPagetable = vmmake();
        char *pa = kalloc();
        if (pa == 0)
          panic("kalloc");
        uint64 va = TRAMPOLINE - 2 * PGSIZE;
        mappages(p->kernelPagetable, va, PGSIZE, (uint64)pa, PTE_R | PTE_W);
        p->kstack = va;
      
      ````

  + 在`kernel/proc.c`，修改`scheduler()`，在`swtch()`切换进程前修改`satp`，保证进程执行时使用的的是进程内核页表，切换完之后再修改`satp`为全局内核页表

    + ````c
      w_satp(MAKE_SATP(p->kernelPagetable));
      sfence_vma();
      swtch(&c->context, &p->context);
      
      kvminithart();
      ````

  + 在`kernel/vm.c`，`kvmpa()`函数会在进程执行期间被调用，此时需要修改为获取进程内核页表，而不是全局的页表

    + ````c
      pte = walk(myproc()->kernelPagetable, va, 0);
      ````

    + 并修改头文件，使`myproc()`可用`#include "spinlock.h" #include "proc.h"`

  + 在`kernel/proc.c`，修改`freeproc()`函数，添加对进程内核页表的资源释放

    + ````c
      if(p->kernelPagetable)
          proc_free_kernelPT(p->kstack, p->kernelPagetable);
        p->kernelPagetable = 0;
      ````

  + 在`kernel/proc.c`，新添加`proc_free_kernelPT()`函数，用于释放进程页表指向的物理内存，以及进程内核页表本身

    + ````c
      ````

    + 

  + 