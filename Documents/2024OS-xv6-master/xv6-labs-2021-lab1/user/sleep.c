#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])//argc:命令行总参数个数 argc[0]表示程序全名  以后的参数是命令行后面跟的用户输入的参数
{
  if (argc != 2)
  {
     // 使用 user/printf.c 中 fprintf(int fd, const char *fmt, ...) 函数格式化输出
    fprintf(2, "Usage: sleep time\n");
    exit(1);
  }
  // 把字符串型参数转换为整型
  int i = atoi(argv[1]);
  sleep(i);
  exit(0);
}
