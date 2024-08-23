#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void prime(int input_fd);

int main(int argc, char const *argv[])
{
    // 定义描述符
    int parent_fd[2];
    // 创建管道
    pipe(parent_fd);
     // 创建进程
    if (fork())
    {
        close(parent_fd[0]);
        int i;
        // 将数字 2 到 35 写入管道
        for (i = 2; i < 36; i++)
        {
            write(parent_fd[1], &i, sizeof(int));
        }
        close(parent_fd[1]);
    }
    else
    {
        close(parent_fd[1]);
        // 子进程调用 prime 函数处理输入
        prime(parent_fd[0]);
    }
    wait(0);
    exit(0);
}

void prime(int input_fd)
{
    int base;
    //如果从管道读取的数据为空，说明已经没有数字可处理，退出函数
    if (read(input_fd, &base, sizeof(int)) == 0)
    {
        exit(0);
    }
    printf("prime %d\n", base);

    //如果还有数字可处理，创建新的子进程
    int p[2];
    pipe(p);
    if (fork() == 0)
    {
        close(p[1]);
        prime(p[0]);
    }
    else
    {
        close(p[0]);
        int n;
        int eof;
        do
        {
            eof = read(input_fd, &n, sizeof(int));
            // 如果 n 不能被 base 整除，则将 n 写入管道
            if (n % base != 0)
            {
                write(p[1], &n, sizeof(int));
            }
        } while (eof);

        close(p[1]);
    }
    wait(0);
    exit(0);
}