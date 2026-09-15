#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>
#include <stdlib.h>

enum {
	NOTYPE = 256, EQ,

	/* TODO: Add more token types */
	TK_NUM,
	TK_HEX,
	TK_REG
};

static struct rule {
	char *regex;//regular expression 正则表达式
	int token_type;
} rules[] = {//长的规则放前面

	/* TODO: Add more rules.
	 * Pay attention to the precedence level of different rules.
	 */
	{"0[xX][0-9a-fA-F]+",          TK_HEX},
	{"[0-9]+",TK_NUM},//number
	{"\\$[a-zA-Z][a-zA-Z0-9]*",    TK_REG},
	{" +",	NOTYPE},				// spaces
	{"\\+", '+'},					// plus
	{"-",       '-'},     // -
	{"\\*",     '*'},     // *
	{"/",       '/'},     // /
	{"\\(",     '('},     // (
	{"\\)",     ')'},     // )
	{"==", EQ}						// equal
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
	int i;
	char error_msg[128];
	int ret;

	for(i = 0; i < NR_REGEX; i ++) {
		ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
		if(ret != 0) {
			regerror(ret, &re[i], error_msg, 128);
			Assert(ret == 0, "regex compilation failed: %s\n%s", error_msg, rules[i].regex);
		}
	}
}

typedef struct token {
	int type;
	char str[32];
} Token;

Token tokens[32];
int nr_token;

static bool make_token(char *e) {
	int position = 0;//位置
	int i;//正则rule对应序号
	regmatch_t pmatch;//正则表达式匹配结果
	
	nr_token = 0;//数量

	while(e[position] != '\0') {//读到结尾
		/* Try all rules one by one. */
		for(i = 0; i < NR_REGEX; i ++) {
			if(regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
				//规则匹配成功  且  匹配内容从当前位置开始
				char *substr_start = e + position;
				int substr_len = pmatch.rm_eo;

				Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s", i, rules[i].regex, position, substr_len, substr_len, substr_start);
				position += substr_len;

				/* TODO: Now a new token is recognized with rules[i]. Add codes
				 * to record the token in the array `tokens'. For certain types
				 * of tokens, some extra actions should be performed.
				 */
				switch (rules[i].token_type) {
					case NOTYPE:
						break;

					case TK_NUM:
					case TK_HEX:
					case TK_REG:
						Assert(nr_token < 32, "too many tokens");
						Assert(substr_len < 32, "number is too long");

						tokens[nr_token].type = TK_NUM;
						strncpy(tokens[nr_token].str,
								substr_start, substr_len);
						tokens[nr_token].str[substr_len] = '\0';
						nr_token++;
						break;

					default:
						Assert(nr_token < 32, "too many tokens");

						tokens[nr_token].type = rules[i].token_type;
						tokens[nr_token].str[0] = '\0';
						nr_token++;
						break;
				}

				break;
			}
		}

		if(i == NR_REGEX) {
			printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
			return false;
		}
	}

	return true; 
}

static uint32_t get_register_value(char *name) {//根据名字读取寄存器
	int i;
	char *reg_name;

	/*
	 * name 是 "$eax"。
	 * name + 1 跳过开头的 '$'，得到 "eax"。
	 */
	reg_name = name + 1;

	for (i = R_EAX; i <= R_EDI; i++) {
		if (strcmp(reg_name, regsl[i]) == 0) {
			return reg_l(i);
		}
	}

	if (strcmp(reg_name, "eip") == 0) {
		return cpu.eip;
	}

	if (strcmp(reg_name, "eflags") == 0) {
		return cpu.eflags.val;
	}

	panic("unknown register: %s", name);
	return 0;
}

static int get_priority(int type) {//定义优先级
	switch (type) {
		case '+':
		case '-':
			return 1;

		case '*':
		case '/':
			return 2;

		default:
			return -1;
	}
}

static bool check_parentheses(int p, int q) {//判断整个表达式是否被括号包围
	int i;
	int depth;

	if (tokens[p].type != '(' || tokens[q].type != ')') {
		return false;
	}

	depth = 0;

	for (i = p; i <= q; i++) {
		if (tokens[i].type == '(') {
			depth++;
		} else if (tokens[i].type == ')') {
			depth--;
		}

		/*
		 * 最外层左括号在 q 之前已经闭合，
		 * 说明它没有包住整个表达式。
		 */
		if (depth == 0 && i < q) {
			return false;
		}

		if (depth < 0) {
			return false;
		}
	}

	return depth == 0;
}

static int find_main_operator(int p, int q) {
	int i;
	int op;
	int depth;
	int best_priority;
	int current_priority;

	op = -1;
	depth = 0;
	best_priority = 100;

	for (i = p; i <= q; i++) {
		if (tokens[i].type == '(') {
			depth++;
			continue;
		}

		if (tokens[i].type == ')') {
			depth--;

			if (depth < 0) {
				panic("unmatched parentheses");
			}

			continue;
		}

		/* 括号内部的运算符不能作为当前层的主运算符。 */
		if (depth != 0) {
			continue;
		}

		current_priority = get_priority(tokens[i].type);

		if (current_priority >= 0 &&
		    current_priority <= best_priority) {
			best_priority = current_priority;
			op = i;
		}
	}

	if (depth != 0) {
		panic("unmatched parentheses");
	}

	return op;
}

static uint32_t eval(int p, int q) {
	int op;
	uint32_t val1;
	uint32_t val2;

	if (p > q) {
		panic("bad expression");
	}

	if (p == q) {
		switch (tokens[p].type) {
			case TK_NUM:
				return strtoul(tokens[p].str, NULL, 10);

			case TK_HEX:
				return strtoul(tokens[p].str, NULL, 0);

			case TK_REG:
				return get_register_value(tokens[p].str);

			default:
				panic("single token has no value");
		}

		return 0;
	}

	if (check_parentheses(p, q)) {
		return eval(p + 1, q - 1);
	}

	op = find_main_operator(p, q);

	if (op == -1) {
		panic("main operator not found");
	}

	val1 = eval(p, op - 1);
	val2 = eval(op + 1, q);

	switch (tokens[op].type) {
		case '+':
			return val1 + val2;

		case '-':
			return val1 - val2;

		case '*':
			return val1 * val2;

		case '/':
			if (val2 == 0) {
				panic("division by zero");
			}
			return val1 / val2;

		default:
			panic("unknown operator");
	}

	return 0;
	return 0;
}

uint32_t expr(char *e, bool *success) {
	if(!make_token(e)) {
		*success = false;
		return 0;
	}

	/* TODO: Insert codes to evaluate the expression. */
	if (nr_token == 0) {
		*success = false;
		return 0;
	}

	*success = true;
	return eval(0, nr_token - 1);
}

