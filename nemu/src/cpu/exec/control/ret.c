#include "cpu/exec/helper.h"

make_helper(ret) {
	swaddr_t target = swaddr_read(cpu.esp, 4);

	cpu.esp += 4;
	cpu.eip = target - 1;

	print_asm("ret");

	return 1;
}