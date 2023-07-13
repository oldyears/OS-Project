#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"

// 进行重定向
void change_pipe(int pipe_p[], int fd)
{
    close(fd);
    dup(pipe_p[fd]);
    close(pipe_p[0]);
    close(pipe_p[1]);
}

void get_prime()
{
    int first, next;
    int pipe_p[2];

    // 首先接受第一个左侧数据并打印
    if (read(0, &first, sizeof(int)))
    {
        fprintf(2, "prime %d\n", first);
        pipe(pipe_p);

        if (fork() == 0)
        {
            change_pipe(pipe_p, 1);
            // 循环读取后续数据
            while (1)
            {
                int n = read(0, &next, sizeof(int));
                if (n <= 0)
                    break;
                if (next % first != 0) // 判断是否能被第一个数据除尽，不能则传给右邻居
                    write(1, &next, sizeof(int));
            }
        }
        else
        {
            wait(0);
            change_pipe(pipe_p, 0);
            get_prime();
        }
    }
}

int main(int argc, char **argv)
{
    // 首先判断合法性
    if (argc > 1)
    {
        fprintf(2, "error:too many parameters\n");
        exit(1);
    }

    // 创建管道
    int pipe_p[2];
    pipe(pipe_p);

    if (fork() == 0) // 第一个子进程负责输入数据
    {
        change_pipe(pipe_p, 1);
        for (int i = 2; i <= 35; i++)
            write(1, &i, sizeof(int));
    }
    else
    {
        wait(0);
        change_pipe(pipe_p, 0);
        get_prime();
    }
    exit(0);
}
