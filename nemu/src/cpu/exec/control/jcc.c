#include "cpu/exec/helper.h"

make_helper(je_si_b) {
	int len = decode_si_b(eip + 1);

	if (cpu.eflags.ZF) {
		cpu.eip += op_src->simm;
	}

	print_asm("je %x", eip + len + 1 + op_src->simm);

	return len + 1;
}

make_helper(jbe_si_b) {
	int len = decode_si_b(eip + 1);

	if (cpu.eflags.CF || cpu.eflags.ZF) {
		cpu.eip += op_src->simm;
	}

	print_asm("jbe %x", eip + len + 1 + op_src->simm);

	return len + 1;
}

make_helper(jle_si_b) {
	int len = decode_si_b(eip + 1);

	if (cpu.eflags.ZF || (cpu.eflags.SF != cpu.eflags.OF)) {
		cpu.eip += op_src->simm;
	}

	print_asm("jle %x", eip + len + 1 + op_src->simm);

	return len + 1;
}

make_helper(jne_si_b) {
	int len = decode_si_b(eip + 1);

	if (!cpu.eflags.ZF) {
		cpu.eip += op_src->simm;
	}

	print_asm("jne %x", eip + len + 1 + op_src->simm);

	return len + 1;
}

make_helper(jge_si_b) {
	int len = decode_si_b(eip + 1);

	if (cpu.eflags.SF == cpu.eflags.OF) {
		cpu.eip += op_src->simm;
	}

	print_asm("jge %x", eip + len + 1 + op_src->simm);

	return len + 1;
}

make_helper(jl_si_b) {
	int len = decode_si_b(eip + 1);

	if (cpu.eflags.SF != cpu.eflags.OF) {
		cpu.eip += op_src->simm;
	}

	print_asm("jl %x", eip + len + 1 + op_src->simm);

	return len + 1;
}

make_helper(jg_si_b) {
	int len = decode_si_b(eip + 1);

	if (!cpu.eflags.ZF &&
			cpu.eflags.SF == cpu.eflags.OF) {
		cpu.eip += op_src->simm;
	}

	print_asm("jg %x", eip + len + 1 + op_src->simm);

	return len + 1;
}

make_helper(jbe_si_l) {
	int len = decode_si_l(eip + 1);

	if (cpu.eflags.CF || cpu.eflags.ZF) {
		cpu.eip += op_src->simm;
	}

	print_asm("jbe %x", eip + len + 1 + op_src->simm);

	return len + 1;
}

make_helper(js_si_b) {
	int len = decode_si_b(eip + 1);

	if (cpu.eflags.SF) {
		cpu.eip += op_src->simm;
	}

	print_asm("js %x", eip + len + 1 + op_src->simm);

	return len + 1;
}