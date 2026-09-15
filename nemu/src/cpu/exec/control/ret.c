#include "cpu/exec/helper.h"

make_helper(ret) {
	swaddr_t target = swaddr_read(cpu.esp, 4);

	cpu.esp += 4;
	cpu.eip = target - 1;

	print_asm("ret");

	return 1;
}

make_helper(ret_i_w) {
	int len = decode_i_w(eip + 1);
	uint16_t imm = op_src->imm;

	swaddr_t target = swaddr_read(cpu.esp, 4);

	cpu.esp += 4;
	cpu.esp += imm;

	cpu.eip = target - (len + 1);

	print_asm("ret $0x%x", imm);

	return len + 1;
}