#include "cpu/exec/helper.h"

make_helper(call_si_l) {
	int len = decode_si_l(eip + 1);

	cpu.esp -= 4;
	swaddr_write(cpu.esp, 4, eip + len + 1);

	cpu.eip += op_src->simm;

	print_asm("call %x", eip + len + 1 + op_src->simm);

	return len + 1;
}