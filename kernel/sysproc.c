#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

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

  backtrace();
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

/*
periodically alerts a process as it uses CPU time.
calls the handler function every tick periods
*/
uint64
sys_sigalarm(void)
{
    int ticks;
    uint64 handler;

    argint(0, &ticks);
    argaddr(1, &handler);

    struct proc *p = myproc();

    p->alarmInterval = ticks;
    p->alarmHandler = (void*) handler;
    p->alarmCount = ticks;

    return 0;
}

/*
to be called at the end of alarm handler function
restores registers to their state before the handler function is called
*/
uint64
sys_sigreturn(void)
{
    struct proc *p = myproc();

    p->trapframe->t0 = p->trapframe->alarm_t0;
    p->trapframe->t2 = p->trapframe->alarm_t2;
    p->trapframe->t3 = p->trapframe->alarm_t3;
    p->trapframe->t4 = p->trapframe->alarm_t4;
    p->trapframe->t5 = p->trapframe->alarm_t5;
    p->trapframe->t6 = p->trapframe->alarm_t6;
    p->trapframe->a0 = p->trapframe->alarm_a0;
    p->trapframe->a1 = p->trapframe->alarm_a1;
    p->trapframe->a2 = p->trapframe->alarm_a2;
    p->trapframe->a3 = p->trapframe->alarm_a3;
    p->trapframe->a4 = p->trapframe->alarm_a4;
    p->trapframe->a5 = p->trapframe->alarm_a5;
    p->trapframe->a6 = p->trapframe->alarm_a6;
    p->trapframe->a7 = p->trapframe->alarm_a7;

    p->handlerActive = 0;

    return p->trapframe->a0;
}
