#include "cpu/exec/template-start.h"

#define instr shr

static void do_execute() {
	DATA_TYPE dest = op_dest->val;
	uint8_t count = op_src->val & 0x1f;

	if (count != 0) {
		DATA_TYPE result = dest >> count;

		/* The last bit shifted out enters CF. */
		cpu.eflags.CF = (dest >> (count - 1)) & 0x1;

		/* For SHR by one bit, OF receives the original MSB. */
		if (count == 1) {
			cpu.eflags.OF = MSB(dest);
		}

		OPERAND_W(op_dest, result);
		update_eflags_pf_zf_sf((DATA_TYPE_S)result);
	}

	print_asm_template2();
}

make_instr_helper(rm_1)
make_instr_helper(rm_cl)
make_instr_helper(rm_imm)

#include "cpu/exec/template-end.h"
