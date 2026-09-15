#include "cpu/exec/helper.h"

make_helper(je_si_b) {
	int len = decode_si_b(eip + 1);

	if (cpu.eflags.ZF) {
		cpu.eip += op_src->simm;
	}

	print_asm("je %x", eip + len + 1 + op_src->simm);

	return len + 1;
}