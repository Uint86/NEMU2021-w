#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "monitor/elf.h"
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
	int i;
	WP *wp;

	if (args == NULL) {
		printf("Usage: info r|w\n");
		return 0;
	}

	if (strcmp(args, "r") == 0) {
		//bug
		// if (args == NULL) {
		// 	printf("Usage: info r\n");
		// 	return 0;
		// }

        for (i = R_EAX; i <= R_EDI; i++) {
            printf("%-6s 0x%08x\n", regsl[i], reg_l(i));
        }

        printf("%-6s 0x%08x\n", "eip", cpu.eip);
        printf("%-6s 0x%08x\n", "eflags", cpu.eflags.val);
    }

	else if (strcmp(args, "w") == 0) {
		wp = get_wp_head();

		if (wp == NULL) {
			printf("No watchpoints\n");
			return 0;
		}

		printf("%-4s %-20s %s\n", "NO", "Expression", "Value");

		while (wp != NULL) {
			printf("%-4d %-20s 0x%08x\n",
					wp->NO,
					wp->expression,
					wp->old_value);

			wp = wp->next;
		}
	}

	//修复bug
	else {
		printf("Unknown info argument '%s'\n", args);
	}

	return 0;
}

static int cmd_x(char *args) {
	char *n_str;
	char *expr_str;
	char *separator;
	char *endptr;
	int n;
	int i;
	bool success;
	swaddr_t addr;
	swaddr_t current_addr;
	uint32_t data;

	if (args == NULL) {
		printf("Usage: x N EXPR\n");
		return 0;
	}

	/* 跳过参数开头可能存在的空格。 */
	while (*args == ' ') {
		args++;
	}

	n_str = args;
	separator = strchr(args, ' ');

	if (separator == NULL) {
		printf("Usage: x N EXPR\n");
		return 0;
	}

	/*
	 * 把 "4 $esp + 4" 分成：
	 * n_str    = "4"
	 * expr_str = "$esp + 4"
	 */
	*separator = '\0';
	expr_str = separator + 1;

	/* 跳过 N 和表达式之间多余的空格。 */
	while (*expr_str == ' ') {
		expr_str++;
	}

	if (*expr_str == '\0') {
		printf("Usage: x N EXPR\n");
		return 0;
	}

	n = strtol(n_str, &endptr, 10);

	if (*endptr != '\0' || n <= 0) {
		printf("Invalid count '%s'\n", n_str);
		return 0;
	}

	addr = expr(expr_str, &success);

	if (!success) {
		printf("Invalid expression '%s'\n", expr_str);
		return 0;
	}

	for (i = 0; i < n; i++) {
		current_addr = addr + i * 4;
		data = swaddr_read(current_addr, 4);

		printf("0x%08x: 0x%08x\n",
				current_addr, data);
	}

	return 0;
}

static int cmd_p(char *args) {
	bool success;
	uint32_t value;

	if (args == NULL) {
		printf("Usage: p EXPR\n");
		return 0;
	}

	value = expr(args, &success);

	if (success) {
		printf("%u (0x%08x)\n", value, value);
	}

	return 0;
}

static int cmd_w(char *args) {
	bool success;
	uint32_t value;
	WP *wp;

	if (args == NULL) {
		printf("Usage: w EXPR\n");
		return 0;
	}

	/*
	 * 创建监视点之前先检查表达式。
	 * 表达式错误时，不应该消耗监视点节点。
	 */
	value = expr(args, &success);

	if (!success) {
		printf("Invalid expression '%s'\n", args);
		return 0;
	}

	wp = new_wp();

	if (wp == NULL) {
		return 0;
	}

	if (strlen(args) >= sizeof(wp->expression)) {
		printf("Expression is too long\n");
		free_wp(wp);
		return 0;
	}

	strcpy(wp->expression, args);
	wp->old_value = value;

	printf("Watchpoint %d: %s\n", wp->NO, wp->expression);
	printf("Initial value: 0x%08x\n", wp->old_value);

	return 0;
}

static int cmd_d(char *args) {
	char *endptr;
	int no;
	WP *wp;

	if (args == NULL) {
		printf("Usage: d N\n");
		return 0;
	}

	no = strtol(args, &endptr, 10);

	if (*endptr != '\0' || no < 0) {
		printf("Invalid watchpoint number '%s'\n", args);
		return 0;
	}

	wp = get_wp_head();

	while (wp != NULL && wp->NO != no) {
		wp = wp->next;
	}

	if (wp == NULL) {
		printf("Watchpoint %d does not exist\n", no);
		return 0;
	}

	free_wp(wp);
	printf("Watchpoint %d deleted\n", no);

	return 0;
}

static int cmd_bt(char *args) {
	uint32_t frame_eip;
	swaddr_t frame_ebp;
	int frame_no;
	int i;

	if (args != NULL) {
		printf("Usage: bt\n");
		return 0;
	}

	frame_eip = cpu.eip;
	frame_ebp = cpu.ebp;
	frame_no = 0;

	if (frame_ebp == 0) {
		printf("No stack frame\n");
		return 0;
	}

	while (frame_ebp != 0) {
		const char *function_name;

		function_name = find_function_name(frame_eip);

		printf("#%d  0x%08x in %s(",
				frame_no,
				frame_eip,
				function_name == NULL ? "??" : function_name);

		for (i = 0; i < 4; i++) {
			uint32_t arg;

			arg = swaddr_read(frame_ebp + 8 + i * 4, 4);
			printf("%s0x%08x", i == 0 ? "" : ", ", arg);
		}

		printf(")\n");

		frame_eip = swaddr_read(frame_ebp + 4, 4);
		frame_ebp = swaddr_read(frame_ebp, 4);
		frame_no++;
	}

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
	{"p", "Evaluate expression", cmd_p},
	{"w", "Set a watchpoint", cmd_w},
	{"d", "Delete a watchpoint", cmd_d},
	{"bt", "Print the stack frame chain", cmd_bt},

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
