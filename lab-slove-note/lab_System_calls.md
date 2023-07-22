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

+ 这里
