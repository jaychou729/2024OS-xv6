#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
    int parent_fd[2];
    int child_fd[2];
    pipe(parent_fd);
    pipe(child_fd);
    char buf[8];//存储传输的数据
    //使⽤ fork() 创建⼦进程，并通过返回值判断当前进程是否为⼦进程（pid为0）
    if (fork() == 0) {
        // child process
        read(parent_fd[0], buf, 4);
        printf("%d: received %s\n", getpid(), buf);
        write(child_fd[1], "pong", strlen("pong"));
    }
    else {
        // parent process
        write(parent_fd[1], "ping", strlen("ping"));
        wait(0);
        read(child_fd[0], buf, 4);
        printf("%d: received %s\n", getpid(), buf);
    }
    exit(0);
}
