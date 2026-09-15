#include "cpu/exec/helper.h"

/* ==================== 短跳转：rel8 ==================== */

make_helper(je_si_b) {
	int len = decode_si_b(eip + 1);

	if (cpu.eflags.ZF) {
		cpu.eip += op_src->simm;
	}

	print_asm("je %x", eip + len + 1 + op_src->simm);

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

make_helper(jbe_si_b) {
	int len = decode_si_b(eip + 1);

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

make_helper(jns_si_b) {
	int len = decode_si_b(eip + 1);

	if (!cpu.eflags.SF) {
		cpu.eip += op_src->simm;
	}

	print_asm("jns %x", eip + len + 1 + op_src->simm);

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

make_helper(jge_si_b) {
	int len = decode_si_b(eip + 1);

	if (cpu.eflags.SF == cpu.eflags.OF) {
		cpu.eip += op_src->simm;
	}

	print_asm("jge %x", eip + len + 1 + op_src->simm);

	return len + 1;
}

make_helper(jle_si_b) {
	int len = decode_si_b(eip + 1);

	if (cpu.eflags.ZF ||
			cpu.eflags.SF != cpu.eflags.OF) {
		cpu.eip += op_src->simm;
	}

	print_asm("jle %x", eip + len + 1 + op_src->simm);

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

make_helper(jb_si_b) {
	int len = decode_si_b(eip + 1);

	if (cpu.eflags.CF) {
		cpu.eip += op_src->simm;
	}

	print_asm("jb %x", eip + len + 1 + op_src->simm);

	return len + 1;
}

make_helper(ja_si_b) {
	int len = decode_si_b(eip + 1);

	if (!cpu.eflags.CF && !cpu.eflags.ZF) {
		cpu.eip += op_src->simm;
	}

	print_asm("ja %x", eip + len + 1 + op_src->simm);

	return len + 1;
}

make_helper(jp_si_b) {
	int len = decode_si_b(eip + 1);

	if (cpu.eflags.PF) {
		cpu.eip += op_src->simm;
	}

	print_asm("jp %x", eip + len + 1 + op_src->simm);

	return len + 1;
}

make_helper(jnp_si_b) {
	int len = decode_si_b(eip + 1);

	if (!cpu.eflags.PF) {
		cpu.eip += op_src->simm;
	}

	print_asm("jnp %x", eip + len + 1 + op_src->simm);

	return len + 1;
}

make_helper(jecxz_si_b) {
	int len = decode_si_b(eip + 1);

	if (cpu.ecx == 0) {
		cpu.eip += op_src->simm;
	}

	print_asm("jecxz %x", eip + len + 1 + op_src->simm);

	return len + 1;
}


/* ==================== 近跳转：rel32 ==================== */

make_helper(je_si_l) {
	int len = decode_si_l(eip + 1);

	if (cpu.eflags.ZF) {
		cpu.eip += op_src->simm;
	}

	print_asm("je %x", eip + len + 1 + op_src->simm);

	return len + 1;
}

make_helper(jne_si_l) {
	int len = decode_si_l(eip + 1);

	if (!cpu.eflags.ZF) {
		cpu.eip += op_src->simm;
	}

	print_asm("jne %x", eip + len + 1 + op_src->simm);

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

make_helper(ja_si_l) {
	int len = decode_si_l(eip + 1);

	if (!cpu.eflags.CF && !cpu.eflags.ZF) {
		cpu.eip += op_src->simm;
	}

	print_asm("ja %x", eip + len + 1 + op_src->simm);

	return len + 1;
}

make_helper(js_si_l) {
	int len = decode_si_l(eip + 1);

	if (cpu.eflags.SF) {
		cpu.eip += op_src->simm;
	}

	print_asm("js %x", eip + len + 1 + op_src->simm);

	return len + 1;
}

make_helper(jl_si_l) {
	int len = decode_si_l(eip + 1);

	if (cpu.eflags.SF != cpu.eflags.OF) {
		cpu.eip += op_src->simm;
	}

	print_asm("jl %x", eip + len + 1 + op_src->simm);

	return len + 1;
}

make_helper(jge_si_l) {
	int len = decode_si_l(eip + 1);

	if (cpu.eflags.SF == cpu.eflags.OF) {
		cpu.eip += op_src->simm;
	}

	print_asm("jge %x", eip + len + 1 + op_src->simm);

	return len + 1;
}

make_helper(jle_si_l) {
	int len = decode_si_l(eip + 1);

	if (cpu.eflags.ZF ||
			cpu.eflags.SF != cpu.eflags.OF) {
		cpu.eip += op_src->simm;
	}

	print_asm("jle %x", eip + len + 1 + op_src->simm);

	return len + 1;
}

make_helper(jg_si_l) {
	int len = decode_si_l(eip + 1);

	if (!cpu.eflags.ZF &&
			cpu.eflags.SF == cpu.eflags.OF) {
		cpu.eip += op_src->simm;
	}

	print_asm("jg %x", eip + len + 1 + op_src->simm);

	return len + 1;
}