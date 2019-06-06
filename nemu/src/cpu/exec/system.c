#include "cpu/exec.h"

extern void raise_intr(uint8_t NO, vaddr_t ret_addr);

void diff_test_skip_qemu();
void diff_test_skip_nemu();

make_EHelper(lidt) {
  //TODO();
  /*cpu.idtr.limit = vaddr_read(id_dest->addr, 2);
  if (decoding.is_operand_size_16) {
	  cpu.idtr.base = vaddr_read(id_dest->addr+2, 3);
  }
  else cpu.idtr.base = vaddr_read(id_dest->addr+2, 4);
  
  //printf("base: 0x%08x, limit: 0x%08x\n", cpu.idtr.base, cpu.idtr.limit);
  print_asm_template1(lidt);*/
  rtl_lm(&t0, &id_dest->addr, 2); 
  cpu.idtr.limit = t0;//先取低16位设置limit
  t1 = id_dest->addr + 2;//跳过limit 
  rtl_lm(&t0, &t1, 4);//取高32位 
  cpu.idtr.base = (t0 & (decoding.is_operand_size_16 ? 0x00FFFFFF : 0xFFFFFFFF));//去掉高8位     
  print_asm_template1(lidt); 
}

make_EHelper(mov_r2cr) {
  TODO();

  print_asm("movl %%%s,%%cr%d", reg_name(id_src->reg, 4), id_dest->reg);
}

make_EHelper(mov_cr2r) {
  TODO();

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
  TODO();

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
