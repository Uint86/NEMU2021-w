#include "cpu/exec/template-start.h"

#define instr adc

// static void do_execute () {
// 	DATA_TYPE result = op_dest->val + op_src->val + cpu.eflags.CF;
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

	uint64_t wide_result =
		(uint64_t)dest + (uint64_t)src + old_cf;

	DATA_TYPE result = (DATA_TYPE)wide_result;

	OPERAND_W(op_dest, result);

	update_eflags_pf_zf_sf((DATA_TYPE_S)result);

	cpu.eflags.CF =
		(wide_result >> (DATA_BYTE * 8)) & 1;

	cpu.eflags.OF =
		MSB(~(dest ^ src) & (dest ^ result));

	print_asm_template2();
}

make_instr_helper(r2rm)

#include "cpu/exec/template-end.h"
