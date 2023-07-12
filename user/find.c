#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "user.h"

char *
fmtname(char *path)
{
    static char buf[DIRSIZ + 1];
    char *p;

    // Find first character after last slash.
    for (p = path + strlen(path); p >= path && *p != '/'; p--)
        ;
    p++;

    // Return blank-padded name.
    if (strlen(p) >= DIRSIZ)
        return p;
    memmove(buf, p, strlen(p));
    buf[strlen(p)] = 0;
    return buf;
}

void find(char *path, char *dst_file)
{
    char buffer[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if ((fd = open(path, 0)) < 0)
    {
        fprintf(2, "find: cnanot open%s\n", path);
    }

    if (fstat(fd, &st) < 0)
    {
        fprintf(2, "find:cannot stat %s\n", path);
        close(fd);
        return;
    }

    switch (st.type)
    {
    case T_FILE:
        if (strcmp(dst_file, fmtname(path)) == 0)
            printf("%s/%s\n", path, dst_file);
        break;
    case T_DIR:
        if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buffer)
        {
            printf("ls: path too long\n");
            break;
        }
        strcpy(buffer, path);
        p = buffer + strlen(buffer);
        *p++ = '/';
        while (read(fd, &de, sizeof(de)) == sizeof(de))
        {
            if (de.inum == 0)
                continue;
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;
            if (stat(buffer, &st) < 0)
            {
                continue;
            }
            if (strcmp(de.name, ".") != 0 
            && strcmp(de.name, "..") != 0
            && st.type == T_DIR)
                find(buffer, dst_file);
            else if(st.type==T_FILE)
            {
            	if(strcmp(dst_file,de.name)==0)
            	printf("%s\n",buffer);
            }
        }
        break;
    }
}

int main(int argc, char **argv)
{
    // 首先判断合法性
    if (argc > 3)
    {
        fprintf(2, "error:too many parameters\n");
        exit(1);
    }

    // 得到其需找的目录
    find(argv[1], argv[2]);
    exit(0);
}