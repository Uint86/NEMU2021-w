#include "cpu/exec/template-start.h"

#define instr shrd

#if DATA_BYTE == 2 || DATA_BYTE == 4
static void do_execute() {
	DATA_TYPE src = op_dest->val;
	DATA_TYPE dest = op_src2->val;
	DATA_TYPE result = dest;

	uint8_t count = op_src->val & 0x1f;

	if (count != 0) {
		uint8_t temp_count = count;

		while (temp_count != 0) {
			result >>= 1;
			result |= (src & 1) << ((DATA_BYTE << 3) - 1);
			src >>= 1;
			temp_count--;
		}

		/* The last bit shifted out of the original destination. */
		cpu.eflags.CF = (dest >> (count - 1)) & 1;

		if (count == 1) {
			cpu.eflags.OF = MSB(dest) ^ MSB(result);
		}

		OPERAND_W(op_src2, result);
		update_eflags_pf_zf_sf((DATA_TYPE_S)result);
	}

	print_asm(
		"shrd" str(SUFFIX) " %s,%s,%s",
		op_src->str,
		op_dest->str,
		op_src2->str
	);
}

make_helper(concat(shrdi_, SUFFIX)) {
	int len = concat(decode_si_rm2r_, SUFFIX) (eip + 1);  /* use decode_si_rm2r to read 1 byte immediate */
	op_dest->val = REG(op_dest->reg);
	do_execute();
	return len + 1;
}
#endif

#include "cpu/exec/template-end.h"
