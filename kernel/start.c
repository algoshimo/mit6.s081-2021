#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"

void main();

// entry.S needs one stack per CPU.
__attribute__ ((aligned (16))) char stack0[4096 * NCPU];

// scratch area for timer interrupt, one per CPU.
uint64 mscratch0[NCPU * 32];

// assembly code in kernelvec for machine-mode timer interrupt.
extern void machinevec();

// entry.S jumps here in machine mode on stack0.
void
start()
{
  // set M Previous Privilege mode to Supervisor, for mret.
  unsigned long x = r_mstatus();
  x &= ~MSTATUS_MPP_MASK;
  x |= MSTATUS_MPP_S;
  w_mstatus(x);

  // set M Exception Program Counter to main, for mret.
  // requires gcc -mcmodel=medany
  w_mepc((uint64)main);

  // disable paging for now.
  w_satp(0);

  // delegate all interrupts and exceptions to supervisor mode.
  w_medeleg(0xffff);
  w_mideleg(0xffff);

  // set up to receive timer interrupts in machine mode,
  // which arrive at machinevec in kernelvec.S,
  // which turns them into software interrupts for
  // devintr() in trap.c.
  int id = r_mhartid();
  // ask the CLINT for a timer interrupt 10,000 cycles from now.
  *(uint64*)CLINT_MTIMECMP(id) = *(uint64*)CLINT_MTIME + 10000;
  // prepare information in scratch[] for machinevec.
  // scratch[0..3] : space for machinevec to save registers.
  // scratch[4] : address of CLINT MTIMECMP register.
  // scratch[5] : desired interval (in cycles) between timer interrupts.
  uint64 *scratch = &mscratch0[32 * id];
  scratch[4] = CLINT_MTIMECMP(id);
  scratch[5] = 10000000;
  w_mscratch((uint64)scratch);
  // set the machine-mode trap handler.
  w_mtvec((uint64)machinevec);
  // enable machine-mode interrupts.
  w_mstatus(r_mstatus() | MSTATUS_MIE);
  // enable machine-mode timer interrupts.
  w_mie(r_mie() | MIE_MTIE);

  // keep each CPU's hartid in its tp register, for cpuid().
  w_tp(id);

  // call main() in supervisor mode.
  asm volatile("mret");
}
