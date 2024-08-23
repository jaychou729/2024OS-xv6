#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "date.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;


  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}



int
sys_pgaccess(void)
{
  uint64 address;// 用于存储用户提供的虚拟地址
  int len;// 用于存储要检查的页数
  uint64 mask_address; // 用于存储位掩码的用户空间缓冲区地址
  uint32 mask = 0;
  if (argaddr(0, &address) < 0)// 解析第一个参数，即虚拟地址
    return -1;
  if (argint(1, &len) < 0 || len > 32)// 解析第二个参数，即要检查的页数
    return -1;
  if (argaddr(2, &mask_address) < 0)// 解析第三个参数，即位掩码缓冲区地址
    return -1;
  struct proc* proc = myproc();
  for(int i=0; i<len; ++i) {
    pte_t* pte = walk(proc->pagetable, address+i*PGSIZE, 0);
    if (*pte & PTE_A) {
      mask |= 1<<i;
      *pte &= ~PTE_A;//清除 PTE_A 访问位,否则，它将一直保持设置状态，无法确定自上次调用 pgaccess() 以来是否访问了该页面
    }
  }
  if (copyout(proc->pagetable, mask_address, (char*)&mask, 4) < 0)//内核中的位掩码缓冲区的内容复制到用户空间的指定位置
    return -1;
  return 0;
}


uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
