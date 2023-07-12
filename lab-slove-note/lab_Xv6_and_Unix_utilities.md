# xrags

## 实验要求
- 编写一个简单的 UNIX xargs 程序，从标准输入中读取行并为每一行运行一个命令，将该行作为命令的参数提供。你的解决方案应该放在 user/xargs.c 中。
  
## 实验提示
- 使用 fork() 和 exec() 在每一行输入上调用命令。在 parent 中使用 wait() 等待 child 完成命令。
- 要读取单个输入行，请一次读取一个字符，直到出现换行符('\n')。
- kernel/param.h 声明了 MAXARG ，如果你需要声明一个 argv 数组，这可能很有用。
- 将程序添加到 Makefile 的 UPROGS 中。
- 文件系统的变化在 qemu 的运行中持续存在。使用 make clean 然后再 make qemu 让一个干净的文件系统运行。
  
## 实验思路
- 首先得到|管道之前的输出，并保存，以便于后续的行处理
  - 使用重定向来得到，dup和exec
- 