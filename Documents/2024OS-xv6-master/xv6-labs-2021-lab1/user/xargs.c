#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
  char inputBuf[32]; // 记录上一个命令的输入
  char charBuf[320]; // 存储所有标记字符的缓冲区
  char* charBufPointer = charBuf;
  int charBufSize = 0;

  char *commandToken[32]; // 使用空格(' ')分隔输入后记录的标记
  int tokenSize = argc - 1; // 记录标记数量（初始值为argc - 1，因为xargs不会被执行）
  int inputSize = -1;

  // 首先将初始argv参数复制到commandToken
  for(int tokenIdx=0; tokenIdx<tokenSize; tokenIdx++)
    commandToken[tokenIdx] = argv[tokenIdx+1];


  while((inputSize = read(0, inputBuf, sizeof(inputBuf))) > 0) {
    for(int i = 0; i < inputSize; i++) {
      char curChar = inputBuf[i];
      if(curChar == '\n') { // 如果读取到'\n'，执行命令
        charBuf[charBufSize] = 0; // 在标记的末尾设置'\0'
        commandToken[tokenSize++] = charBufPointer;
        commandToken[tokenSize] = 0; // 在数组末尾设置空指针

        if(fork() == 0) { // 创建子进程执行命令
          exec(argv[1], commandToken);
        }
        wait(0);
        tokenSize = argc - 1; // 初始化
        charBufSize = 0;
        charBufPointer = charBuf;
      }
      else if(curChar == ' ') {
        charBuf[charBufSize++] = 0; // 标记字符串的结尾
        commandToken[tokenSize++] = charBufPointer;
        charBufPointer = charBuf + charBufSize; // 切换到新字符串的起始位置
      }
      else {
        charBuf[charBufSize++] = curChar;
      }
    }
  }
  exit(0);
}