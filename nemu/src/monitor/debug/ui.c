#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint32_t);

/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
	static char *line_read = NULL;

	if (line_read) {
		free(line_read);
		line_read = NULL;
	}

	line_read = readline("(nemu) ");

	if (line_read && *line_read) {
		add_history(line_read);
	}

	return line_read;
}

static int cmd_c(char *args) {
	cpu_exec(-1);
	return 0;
}

static int cmd_q(char *args) {
	return -1;
}

static int cmd_help(char *args);

static int cmd_si(char *args) {
	int steps;
	if(args == NULL) {
		steps = 1;
	}
	else {
		steps = atoi(args);
	}
	cpu_exec(steps);
	return 0;
}

static int cmd_info(char *args){
	if (strcmp(args, "r") == 0) {
        int i;

		if (args == NULL) {
			printf("Usage: info r\n");
			return 0;
		}

        for (i = R_EAX; i <= R_EDI; i++) {
            printf("%-6s 0x%08x\n", regsl[i], reg_l(i));
        }

        printf("%-6s 0x%08x\n", "eip", cpu.eip);
        printf("%-6s 0x%08x\n", "eflags", cpu.eflags.val);
    }

	return 0;
}

static int cmd_x(char *args) {
	char *n_str;
	char *addr_str;
	int n;
	swaddr_t addr;

	if (args == NULL) {
		printf("Usage: x N ADDR\n");
		return 0;
	}

	n_str = strtok(args, " ");
	addr_str = strtok(NULL, " ");

	if (n_str == NULL || addr_str == NULL) {
		printf("Usage: x N ADDR\n");
		return 0;
	}

	n = atoi(n_str);
	addr = strtoul(addr_str, NULL, 0);

	printf("n = %d, addr = 0x%08x\n", n, addr);

	return 0;
}

static struct {
	char *name;
	char *description;
	int (*handler) (char *);//函数指针，handler存放下面的名字
} cmd_table [] = {
	{ "help", "Display informations about all supported commands", cmd_help },
	{ "c", "Continue the execution of the program", cmd_c },
	{ "q", "Exit NEMU", cmd_q },

	/* TODO: Add more commands*/
	{"si","single step",cmd_si},
	{"info","print",cmd_info},
	{"x","examine",cmd_x},

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
	/* extract the first argument */
	char *arg = strtok(NULL, " ");
	int i;

	if(arg == NULL) {
		/* no argument given */
		for(i = 0; i < NR_CMD; i ++) {
			printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
		}
	}
	else {
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(arg, cmd_table[i].name) == 0) {
				printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
				return 0;
			}
		}
		printf("Unknown command '%s'\n", arg);
	}
	return 0;
}

void ui_mainloop() {
	while(1) {
		char *str = rl_gets();
		char *str_end = str + strlen(str);

		/* extract the first token as the command */
		char *cmd = strtok(str, " ");    //在这里识别命令，cmd指向前面的
		if(cmd == NULL) { continue; }

		/* treat the remaining string as the arguments,
		 * which may need further parsing
		 */
		char *args = cmd + strlen(cmd) + 1;
		if(args >= str_end) {
			args = NULL;
		}

#ifdef HAS_DEVICE
		extern void sdl_clear_event_queue(void);
		sdl_clear_event_queue();
#endif

		int i;
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(cmd, cmd_table[i].name) == 0) {
				if(cmd_table[i].handler(args) < 0) { return; }
				break;
			}
		}

		if(i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
	}
}
