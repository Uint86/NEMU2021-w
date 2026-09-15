#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
	int NO;
	struct watchpoint *next;

	/* TODO: Add more members if necessary */

	char expression[128];
	uint32_t old_value;

} WP;

WP *new_wp(void);
void free_wp(WP *wp);
WP *get_wp_head(void);
bool check_watchpoints(swaddr_t instr_eip);

#endif

// WP *new_wp(void);
// void free_wp(WP *wp);
// WP *get_wp_head(void);

// bool check_watchpoints(swaddr_t instr_eip);