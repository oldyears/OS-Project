#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"


int main(int argc,char** argv)
{
    int i = 0;

    if (argc < 2)
    {
        fprintf(2, "usage: sleep time...\n");
        exit(1);
    }
    if (argc > 2)
    {
        fprintf(2, "error: please input right format\n");
        exit(2);
    }
    sleep(atoi(argv[1]));
    exit(0);
}