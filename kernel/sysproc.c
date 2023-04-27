#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "fs.h"
#include "sleeplock.h"
#include "fcntl.h"
#include "file.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
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
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
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

  argint(0, &n);
  if(n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
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

uint64
sys_mmap(void)
{
  uint64 addr;
  uint length;
  int prot;
  int flag;
  int fd;
  uint offset;

  argaddr(0, &addr);
  argint(1, (int*)&length);
  argint(2, &prot);
  argint(3, &flag);
  argint(4, &fd);
  argint(5, (int*)&offset);

  struct proc *p = myproc();
  struct file *file = p->ofile[fd];

  if (prot & PROT_READ) {
    if (!file->readable) {
      return -1;
    }
  }

  if (prot & PROT_WRITE) {
    if (!file->writable && flag != MAP_PRIVATE) {
      return -1;
    }
  }

  for (int i = 0; i < NVMA; ++i) {
    if (!p->vma[i].length && p->vma[i].size >= length) {
      p->vma[i].file = file;
      p->vma[i].addr = p->vma[i].pos;
      p->vma[i].length = length;
      p->vma[i].prot = prot;
      p->vma[i].flag = flag;
      filedup(file);
      return p->vma[i].addr;
    }
  }

  return -1;
}

uint64
sys_munmap(void)
{
  uint64 addr;
  uint length;

  argaddr(0, &addr);
  argint(1, (int*)&length);

  struct proc *p = myproc();

  int index;
  for (index = 0; index < NVMA; ++index) {
    if (p->vma[index].addr <= addr && addr < p->vma[index].addr + p->vma[index].length) {
      break;
    }
  }

  if (index >= 16) {
    return -1;
  }

  if (length > p->vma[index].length) {
    return -1;
  }

  if (p->vma[index].flag == MAP_SHARED) {
    filewrite(p->vma[index].file, addr, length);
  }

  uint64 va;
  for (va = addr; va < addr + length; va += PGSIZE) {
    pte_t *pte = walk(p->pagetable, va, 0);

    if (*pte & PTE_V) {
      uvmunmap(p->pagetable, va, 1, 0);
    }
  }

  p->vma[index].addr = addr + length;
  p->vma[index].length -= length;

  if (p->vma[index].length == 0) {
    fileclose(p->vma[index].file);
  }

  return 0;
}