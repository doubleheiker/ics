#include "cpu/exec.h"


make_EHelper(operand_size);

make_EHelper(nop);
make_EHelper(inv);
make_EHelper(nemu_trap);

/*control.c*/
make_EHelper(jcc);
make_EHelper(call);
make_EHelper(ret);

/*data-mov.c*/
make_EHelper(mov);
make_EHelper(push);
make_EHelper(pop);
make_EHelper(lea);
make_EHelper(movzx);
make_EHelper(movsx);

/*arith.c*/
make_EHelper(add);
make_EHelper(sub);
make_EHelper(adc);
make_EHelper(cmp);

/*logic.c*/
make_EHelper(test);
make_EHelper(xor);
make_EHelper(or);
make_EHelper(and);
make_EHelper(setcc);
make_EHelper(sar);
