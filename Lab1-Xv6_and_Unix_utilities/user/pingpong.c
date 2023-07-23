#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"

int main(int argc, char **argv)
{
    // 首先检查是否合法
    if (argc >= 2)
    {
        fprintf(2, "error: too many parameters\n");
        exit(1);
    }

    // 首先创建ripe
    int p_to_c[2];
    int c_to_p[2];
    pipe(p_to_c);
    pipe(c_to_p);

    // 进行管道信息传输

    char buffer[] = "pingpong";
    if (fork() == 0) // 1. 若是child则给pipe写数据
    {
        read(p_to_c[0], buffer, 4);
        fprintf(2, "%d: received ping\n", getpid());
        write(c_to_p[1], buffer, 4);
        exit(0);
    }
    else // 2. 若是parent则从pipe读数据
    {
        write(p_to_c[1], buffer, 4);
        wait(0);
        read(c_to_p[0], buffer, 4);
        fprintf(2, "%d: received pong\n", getpid());
        exit(0);
    }

    exit(0);
}