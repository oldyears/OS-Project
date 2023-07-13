#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"
#include "kernel/fs.h"

#define NULL 0

// 得到所有命令行参数
void get_instr(char **instr, char *buffer)
{
    char *start = buffer;
    char *p = buffer;

    for (int i = 0; *p != '\0'; p++)
    {
        // 当读取到换行符时则进行子进程申请并执行命令
        if (*p == '\n')
        {
            // 得到行指令
            instr[i] = malloc(sizeof(p - start + 1));
            memcpy(instr[i], start, p - start);
            instr[i][p - start] = 0;
            start = p + 1;

            i++;
        }
    }
}


int main(int argc, char **argv)
{
    // 首先处理之前语句的输出
    char buffer[512];
    read(0, buffer, sizeof(buffer));

    // 对buffer行处理
    char *instr[MAXARG];
    get_instr(instr, buffer);

    // 循环处理
    for(int i = 0; instr[i] != NULL;i++)
    {
        // 在子进程中执行命令
        if(fork() == 0)
        {
            char* argv_child[4];
            
            argv_child[0] = malloc(strlen(argv[1]) + 1);
            strcpy(argv_child[0],argv[1]);
            
            argv_child[1] = malloc(strlen(argv[2]) + 1);
            strcpy(argv_child[1],argv[2]);
            
            argv_child[2] = malloc(strlen(instr[i]) + 1);
            strcpy(argv_child[2],instr[i]);
            
            argv_child[3] = 0;
            
            exec(argv_child[0],argv_child);
            
            exit(0);
        }
        else
        {
            wait(0);
        }
        // printf("%s\n",instr[i]);
    }

    exit(0);
}
