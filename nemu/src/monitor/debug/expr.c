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
	TK_NUM
};

static struct rule {
	char *regex;//regular expression 正则表达式
	int token_type;
} rules[] = {//长的规则放前面

	/* TODO: Add more rules.
	 * Pay attention to the precedence level of different rules.
	 */
	{"[0-9]+",TK_NUM},//number
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

static int find_main_operator(int p, int q) {
	int i;
	int op;
	int best_priority;
	int current_priority;

	op = -1;
	best_priority = 100;

	for (i = p; i <= q; i++) {
		current_priority = get_priority(tokens[i].type);

		if (current_priority >= 0 &&
		    current_priority <= best_priority) {
			best_priority = current_priority;
			op = i;
		}
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
		if (tokens[p].type != TK_NUM) {
			panic("single token is not a number");
		}

		return strtoul(tokens[p].str, NULL, 10);
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

