#include "cpu/exec/helper.h"

make_helper(push_r_l) {
	int len = decode_r_l(eip + 1);

	cpu.esp -= 4;
	swaddr_write(cpu.esp, 4, op_src->val);

	print_asm("pushl %s", op_src->str);

	return len + 1;
}

make_helper(push_rm_l) {
	int len = decode_rm_l(eip + 1);
	uint32_t value = op_src->val;

	cpu.esp -= 4;
	swaddr_write(cpu.esp, 4, value);

	print_asm("pushl %s", op_src->str);

	return len + 1;
}

make_helper(push_i_l) {
	int len = decode_i_l(eip + 1);

	cpu.esp -= 4;
	swaddr_write(cpu.esp, 4, op_src->val);

	print_asm("pushl %s", op_src->str);

	return len + 1;
}

make_helper(push_si_b) {
	int len = decode_si_b(eip + 1);

	cpu.esp -= 4;
	swaddr_write(cpu.esp, 4, op_src->val);

	print_asm("pushl %s", op_src->str);

	return len + 1;
}