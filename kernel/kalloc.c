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
  struct spinlock lock;
  struct run *freelist;
} kmem[NCPU];

void
kinit()
{
  for(int i=0;i<NCPU;i++)
  initlock(&kmem[i].lock, "kmem");

  // for (int i = 0; i < NCPU; i++) {
  //   char name[9] = {0};
  //   snprintf(name, 8, "kmem-%d", i);
  //   initlock(&kmem[i].lock, name);
  // }
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

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;

  push_off();
  acquire(&kmem[cpuid()].lock);
  r->next = kmem[cpuid()].freelist;
  kmem[cpuid()].freelist = r;
  release(&kmem[cpuid()].lock);
  pop_off();
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;
  int cpu_id = cpuid();
  push_off();
  acquire(&kmem[cpu_id].lock);
  r = kmem[cpu_id].freelist;
  if(r)
    kmem[cpu_id].freelist = r->next;
  release(&kmem[cpu_id].lock);
  if(r == 0) 
  {
    // for(int i=0;i<NCPU;i++)
    // {
    //   int next_cpu = (cpu_id + i) %NCPU;
    //   acquire(&kmem[next_cpu].lock);
    //   r = kmem[next_cpu].freelist;
    //   if(r) 
    //   {
    //     kmem[next_cpu].freelist = r->next;
    //     release(&kmem[next_cpu].lock);
    //     break;
    //   }
    //   release(&kmem[next_cpu].lock);
    // }

    for(int i=0;i<NCPU;i++)
    {
      if(i == cpu_id) continue;
      int next_cpu = i;
      acquire(&kmem[next_cpu].lock);
      r = kmem[next_cpu].freelist;
      if(r) 
      {
        kmem[next_cpu].freelist = r->next;
        release(&kmem[next_cpu].lock);
        break;
      }
      release(&kmem[next_cpu].lock);
    }

  }
  if(r)
    memset((char*)r, 5, PGSIZE); // fill with junk
  pop_off();
  return (void*)r;
}
