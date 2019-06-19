#include "cpu/exec.h"

extern void raise_intr(uint8_t NO, vaddr_t ret_addr);

void diff_test_skip_qemu();
void diff_test_skip_nemu();

make_EHelper(lidt) {
  //TODO();
  cpu.idtr.limit = vaddr_read(id_dest->addr, 2);
  if (decoding.is_operand_size_16) {
	  cpu.idtr.base = vaddr_read(id_dest->addr+2, 3);
  }
  else cpu.idtr.base = vaddr_read(id_dest->addr+2, 4);
  
  //printf("base: 0x%08x, limit: 0x%08x\n", cpu.idtr.base, cpu.idtr.limit);
  print_asm_template1(lidt); 
}

make_EHelper(mov_r2cr) {
  if (id_dest->reg == 0) {
	  cpu.cr0.val = id_src->val;
  }
  else if (id_dest->reg == 3) {
	  cpu.cr3.val = id_src->val;
  }
  else {
	  printf("unsupported control reg\n");
	  assert(0);
  }

  print_asm("movl %%%s,%%cr%d", reg_name(id_src->reg, 4), id_dest->reg);
}

make_EHelper(mov_cr2r) {
  //TODO();
  if (id_dest->reg == 0) {
	  t0 = cpu.cr0.val;
  }
  else if (id_dest->reg == 3) {
	  t0 = cpu.cr3.val;
  }
  else {
	  printf("unsupported control reg\n");
	  assert(0);
  }
  operand_write(id_dest, &t0);

  print_asm("movl %%cr%d,%%%s", id_src->reg, reg_name(id_dest->reg, 4));

#ifdef DIFF_TEST
  diff_test_skip_qemu();
#endif
}

make_EHelper(int) {
  //TODO();
  raise_intr(id_dest->val, decoding.seq_eip);

  print_asm("int %s", id_dest->str);

#ifdef DIFF_TEST
  diff_test_skip_nemu();
#endif
}

make_EHelper(iret) {
  //TODO();
  rtl_pop(&t0);
  decoding.jmp_eip = t0;
  decoding.is_jmp = 1;
  rtl_pop(&t0);
  cpu.cs = (uint16_t)t0;
  rtl_pop(&cpu.EFLAGS);

  print_asm("iret");
}

uint32_t pio_read(ioaddr_t, int);
void pio_write(ioaddr_t, int, uint32_t);

make_EHelper(in) {
  //TODO();
  t1 = pio_read(id_src->val, id_dest->width);
  operand_write(id_dest, &t1);

  print_asm_template2(in);

#ifdef DIFF_TEST
  diff_test_skip_qemu();
#endif
}

make_EHelper(out) {
  //TODO();
  pio_write(id_dest->val, id_dest->width, id_src->val);

  print_asm_template2(out);

#ifdef DIFF_TEST
  diff_test_skip_qemu();
#endif
}
