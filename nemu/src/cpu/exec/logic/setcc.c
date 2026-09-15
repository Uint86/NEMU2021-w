#include "cpu/exec/helper.h"

make_helper(setne_rm_b) {
	int len = decode_rm_b(eip + 1);
	uint8_t result = !cpu.eflags.ZF;

	write_operand_b(op_src, result);

	print_asm("setne %s", op_src->str);

	return len + 1;
}