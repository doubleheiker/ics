#include "cpu/exec.h"
#include "memory/mmu.h"

void raise_intr(uint8_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */

  //TODO();
  vaddr_t addr;
  addr = NO * 8 + cpu.idtr.base;
  printf("NO: %d, base: 0x%08x, addr: 0x%08x\n", NO, cpu.idtr.base, addr);

  union {
	  GateDesc gd;
	  struct {
		  uint32_t low;
		  uint32_t high;
	  };
  } t;
  t.low = vaddr_read(addr, 4);
  t.high = vaddr_read(addr + 4, 4);

  printf("t.low: 0x%08x, t.high: 0x%08x\n", t.low, t.high);

  /*if (t.gd.present == 0) {
	  printf("invaild P");
	  assert(0);//invaild p
  }*/

  rtl_push(&cpu.EFLAGS);
  t0 = cpu.cs;
  rtl_push(&t0);
  rtl_push(&ret_addr);
  
  decoding.is_jmp = 1;
  decoding.jmp_eip = (t.gd.offset_15_0 && 0xffff) + ((t.gd.offset_31_16 && 0xffff)<<16);
  printf("jmp: %x\n", decoding.jmp_eip);
  cpu.eflags.IF = 0;
}

void dev_raise_intr() {
}
