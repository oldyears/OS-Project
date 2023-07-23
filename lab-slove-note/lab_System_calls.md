# 实验笔记

## System call tracing (moderate)

### 实验目的

+ 添加一个系统调用跟踪功能，该功能可以在以后的实验中为你提供帮助。
+ 你将创建一个新的 trace 系统调用来控制跟踪。
+ 它应该有一个参数，一个整数“mask(掩码)”，其指定要跟踪的系统调用。例如，为了跟踪 fork 系统调用，程序调用 trace (1 << SYS_fork) ，其中 SYS_fork 是来自 kernel/syscall.h 的系统调用号。
+ 如果掩码中设置了系统调用的编号，则必须修改 xv6 内核以在每个系统调用即将返回时打印出一行。
+ 该行应包含 进程 ID 、 系统调用名称 和 返回值 ；您不需要打印系统调用参数。 trace 系统调用应该为调用它的进程和它随后派生的任何子进程启用跟踪，但不应影响其他进程。

### 实验要求及提示
+ 将 $U/_trace 添加到 Makefile 的 UPROGS 中
+ 运行 make qemu ， 你将看到编译器无法编译 user/trace.c ，因为系统调用的用户空间存根还不存在：将系统调用的原型添加到 user/user.h ，将存根添加到 user/usys.pl ，以及将系统调用号添加到 kernel/syscall.h 中。 Makefile 调用 perl 脚本 user/usys.pl ，它生成 user/usys.S ，实际的系统调用存根，它使用 RISC-V ecall 指令转换到内核。修复编译问题后，运行 trace 32 grep hello README ；它会失败，因为你还没有在内核中实现系统调用。
+ 在 kernel/sysproc.c 中添加一个 sys_trace() 函数，该函数通过在 proc 结构中的新变量中记住其参数来实现新系统调用(请参阅 kernel/proc.h )。从用户空间检索系统调用参数的函数位于 kernel/syscall.c 中，你可以在 kernel/sysproc.c 中查看它们的使用示例。
+ 修改 fork() (参见 kernel/proc.c )以将跟踪的掩码从父进程复制到子进程。
+ 修改 kernel/syscall.c 中的 syscall() 函数以打印跟踪输出。你将需要添加要索引的系统调用名称数组。



### 实验思路

+ 这里按照提示照做即可，需要自己理解的点只有trace函数的编写，简单来说，





## Sysinfo (moderate)

### 实验要求

+ 在本实验中，您将添加一个系统调用 sysinfo ，它收集有关正在运行的系统信息。系统调用接受一个参数：一个指向 struct sysinfo 的指针(参见 kernel/sysinfo.h )。内核应该填写这个结构体的字段： freemem 字段应该设置为空闲内存的字节数， nproc 字段应该设置为状态不是 UNUSED 的进程数。我们提供了一个测试程序 sysinfotest ；如果它打印 “sysinfotest：OK” ，则实验结果通过测试。

### 实验提示

+ 将 $U/_sysinfotest 添加到 Makefile 的 UPROGS 中。

+ 运行 make qemu ， 你将看到编译器无法编译 user/sysinfotest.c 。添加系统调用 sysinfo ，按照与之前实验相同的步骤。要在 user/user.h 中声明 sysinfo() 的原型，您需要预先声明 struct sysinfo ：

  + ```struct sysinfo;
    struct sysinfo;
    int sysinfo(struct sysinfo *);
    ```

+ 修复编译问题后，运行 sysinfotest 会失败，因为你还没有在内核中实现系统调用。

+ sysinfo 需要复制一个 struct sysinfo 返回用户空间；有关如何使用 copyout() 执行此操作的示例，请参阅 sys_fstat() ( kernel/sysfile.c ) 和 filestat() ( kernel/file.c )。

+ 要收集空闲内存量，请在 kernel/kalloc.c 中添加一个函数。

+ 要收集进程数，请在 kernel/proc.c 中添加一个函数。



### 实验思路

+ 按照题目要求“内核应该填写这个结构体的字段： freemem 字段应该设置为空闲内存的字节数， nproc 字段应该设置为状态不是 UNUSED 的进程数”，其实状态不是UNUSED进程可以理解为状态为被使用——USED

+ 首先按照提示，将`$U_sysinfotest`添加到Makefile中

+ 运行`make qemu`，发现报错，按照之前`trace`的做法添加相应的系统调用

  + 在`user.h`中添加相应的声明`struct sysinfo;int sysinfo(struct sysinfo*);`即可
  + 在`user/user.pl`中添加`entry("sysinfo");`
  + 在`kernel/syscall.h`中添加`#define SYS_sysinfo 23`即可

+ 完成以上系统调用添加后，运行`make qemu`正常编译，但依旧运行失败，故接下来在内核中实现系统调用

  + 学习如何复制`struct sysinfo`返回用户空间，在`sys_fstat()`中，实际上是调用了`filestat(struct file *f, uint64 addr)`，此函数中可以看到`copyout(pagetable_t pagetable, uint64 dstva, char *src, uint64 len)`的使用，其作用为：将len个字节从src复制到给定页表中的虚拟地址dstva

  + 参考`trace`的实验步骤，在`kernel/syscall.c`中添加系统调用声明，并将其添加到系统调用的数组中

    + `extern uint64 sys_sysinfo(struct sysinfo*);`
    + `[SYS_sysinfo] sys_sysinfo,`
    + `"sysinfo",`

  + 具体的函数需要在`kernel/sysproc.c`中实现，在其中添加函数实现

  + ```c
    uint64
    sys_sysinfo(void)
    {
      uint64 freenum, procnum;
      freenum = getFreeMemory();
      procnum = getNumProc();
    
      uint64 st; // user pointer to struct stat
    
      if(argaddr(0, &st) < 0)
        return -1;
    
      struct sysinfo info = {freenum,procnum};
      if(copyout(myproc()->pagetable,st,(char *)&info,sizeof(info))<0)
        return -1;
      return 0;
    }
    ```

  + 还需添加头文件`#include "sysinfo.h"`

  + 这里千万注意要给`argaddr`的第一个参数置为0，否则直接失败

+ 实现收集空闲内存量，在`kernel/kalloc.c`中添加函数

  + ```c
    uint64 
    getFreeMemory()
    {
      struct run *r;
      uint64 num = 0;
    
      acquire(&kmem.lock);
      r = kmem.freelist;
      while(r)
      {
        num++;
        r = r->next;
      }
      release(&kmem.lock);
      
      return num * PGSIZE;
    }
    ```

+ 实现进程数，在`kernel/proc.c`中添加函数

  + ```c
    uint64
    getNumProc()
    {
      struct proc* p;
      uint64 num = 0;
    
      for(p=proc; p<&proc[NPROC]; p++)
      {
        acquire(&p->lock);
        if(p->state!=UNUSED)
          num++;
        release(&p->lock);
      }
      return num;
    
    }
    ```

  + 注这里查看进程状态需要**加锁**，没有加锁导致失败

+ 最后在`user/`中添加`sysinfo.c`以便于实现`sysinfo()`便于调用

+ ```c
  #include "kernel/types.h"
  #include "kernel/riscv.h"
  #include "kernel/sysinfo.h"
  #include "user/user.h"
  
  int main(int argc,char **argv)
  {
      if(argc!=1)
      {
          printf("2,Usage: %s need not param\n",argv[0]);
          exit(-1);
      }
  
      struct sysinfo info;
      sysinfo(&info);
      printf("free space: %d, used process: %d\n",info.freemem,info.nproc);
      
      exit(0);
  }
  ```

+ 

+ 
