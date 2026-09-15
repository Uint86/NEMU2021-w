#include "cpu/exec/template-start.h"

#define instr sbb

// static void do_execute () {
// 	DATA_TYPE result = op_dest->val - (op_src->val + cpu.eflags.CF);
// 	OPERAND_W(op_dest, result);

// 	/* TODO: Update EFLAGS. */
// 	update_eflags_pf_zf_sf((DATA_TYPE_S)result);
// 	cpu.eflags.CF = result < op_dest->val;
// 	cpu.eflags.OF = MSB(~(op_dest->val ^ op_src->val) & (op_dest->val ^ result));

// 	print_asm_template2();
// }

static void do_execute() {
	DATA_TYPE dest = op_dest->val;
	DATA_TYPE src = op_src->val;
	uint8_t old_cf = cpu.eflags.CF;

	uint64_t subtrahend = (uint64_t)src + old_cf;
	DATA_TYPE result =
		(DATA_TYPE)((uint64_t)dest - subtrahend);

	OPERAND_W(op_dest, result);

	update_eflags_pf_zf_sf((DATA_TYPE_S)result);

	/* A borrow occurs when dest is smaller than src + old CF. */
	cpu.eflags.CF = (uint64_t)dest < subtrahend;

	cpu.eflags.OF =
		MSB((dest ^ src) & (dest ^ result));

	print_asm_template2();
}

make_instr_helper(r2rm)

#include "cpu/exec/template-end.h"
