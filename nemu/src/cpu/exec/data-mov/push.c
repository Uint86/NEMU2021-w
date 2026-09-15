#include "cpu/exec/helper.h"

make_helper(push_r_l) {
	int len = decode_r_l(eip + 1);

	cpu.esp -= 4;
	swaddr_write(cpu.esp, 4, op_src->val);

	print_asm("pushl %s", op_src->str);

	return len + 1;
}