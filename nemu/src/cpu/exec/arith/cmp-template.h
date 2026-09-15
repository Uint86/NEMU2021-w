#include "cpu/exec/template-start.h"

#define instr cmp

static void do_execute() {
	DATA_TYPE dest = op_dest->val;
	DATA_TYPE src = op_src->val;
	DATA_TYPE result = dest - src;

	update_eflags_pf_zf_sf((DATA_TYPE_S)result);

	cpu.eflags.CF = dest < src;
	cpu.eflags.OF =
		MSB((dest ^ src) & (dest ^ result));

	print_asm_template2();
}

#if DATA_BYTE == 2 || DATA_BYTE == 4
make_instr_helper(si2rm)
#endif

make_instr_helper(r2rm)

#include "cpu/exec/template-end.h"