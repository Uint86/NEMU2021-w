#include "cpu/exec/helper.h"

make_helper(pop_r_l) {
	int len = decode_r_l(eip + 1);
	uint32_t value = swaddr_read(cpu.esp, 4);

	cpu.esp += 4;
	write_operand_l(op_src, value);

	print_asm("popl %s", op_src->str);

	return len + 1;
}