// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
  struct run *next;
};

struct {
  struct spinlock lock[NCPU];
  struct run *freelist[NCPU];
  int free_page_count[NCPU];
} kmem;


static char *lock_name[] = {
  "kmem 0",
  "kmem 1",
  "kmem 2",
  "kmem 3",
  "kmem 4",
  "kmem 5",
  "kmem 6",
  "kmem 7",
};

void
kinit()
{
  for (int i = 0; i < NCPU; i++) {
    initlock(&kmem.lock[i], lock_name[i]);
    kmem.free_page_count[i] = 0;
  }
  freerange(end, (void*)PHYSTOP);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by pa,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *r;
  int cpu_number;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;

  push_off();
  cpu_number = cpuid();

  acquire(&kmem.lock[cpu_number]);
  r->next = kmem.freelist[cpu_number];
  kmem.freelist[cpu_number] = r;
  kmem.free_page_count[cpu_number]++;
  release(&kmem.lock[cpu_number]);
  pop_off();
}

struct run *
steal(int cpu_number) {
  struct run *r = 0;
  for (int i = 0; i < 8 * NCPU; i++) {
    cpu_number += 1;
    cpu_number %= NCPU;
    acquire(&kmem.lock[cpu_number]);
    if (kmem.free_page_count[cpu_number] == 0) {
      release(&kmem.lock[cpu_number]);
      continue;
    }
    r = kmem.freelist[cpu_number];
    kmem.freelist[cpu_number] = r->next;
    kmem.free_page_count[cpu_number]--;
    release(&kmem.lock[cpu_number]);
    break;
  }
  return r;
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;
  int cpu_number;

  push_off();
  cpu_number = cpuid();

  acquire(&kmem.lock[cpu_number]);
  r = kmem.freelist[cpu_number];
  if(r) {
    kmem.freelist[cpu_number] = r->next;
    kmem.free_page_count[cpu_number]--;
  }
  release(&kmem.lock[cpu_number]);

  if(!r) {
    r = steal(cpu_number);
  }
  pop_off();

  if(r)
    memset((char*)r, 5, PGSIZE); // fill with junk
  return (void*)r;
}
