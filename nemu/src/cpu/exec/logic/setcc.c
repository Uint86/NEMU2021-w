#include "cpu/exec/helper.h"

make_helper(sete_rm_b) {
	int len = decode_rm_b(eip + 1);
	uint8_t result = cpu.eflags.ZF;

	write_operand_b(op_src, result);

	print_asm("sete %s", op_src->str);

	return len + 1;
}

make_helper(setne_rm_b) {
	int len = decode_rm_b(eip + 1);
	uint8_t result = !cpu.eflags.ZF;

	write_operand_b(op_src, result);

	print_asm("setne %s", op_src->str);

	return len + 1;
}

make_helper(seta_rm_b) {
	int len = decode_rm_b(eip + 1);
	uint8_t result = !cpu.eflags.CF && !cpu.eflags.ZF;

	write_operand_b(op_src, result);

	print_asm("seta %s", op_src->str);

	return len + 1;
}

make_helper(setge_rm_b) {
	int len = decode_rm_b(eip + 1);
	uint8_t result = cpu.eflags.SF == cpu.eflags.OF;

	write_operand_b(op_src, result);

	print_asm("setge %s", op_src->str);

	return len + 1;
}