#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;

void init_wp_pool() {
	int i;
	for(i = 0; i < NR_WP; i ++) {
		wp_pool[i].NO = i;
		wp_pool[i].next = &wp_pool[i + 1];
	}
	wp_pool[NR_WP - 1].next = NULL;

	head = NULL;
	free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

WP *new_wp(void) {//分配监视点
	WP *wp;

	if (free_ == NULL) {
		printf("No free watchpoint\n");
		return NULL;
	}

	/* 从空闲链表取下第一个节点。 */
	wp = free_;
	free_ = free_->next;

	/* 把它放到使用中链表的开头。 */
	wp->next = head;
	head = wp;

	return wp;
}

void free_wp(WP *wp) {//释放监视点
	WP *prev;

	if (wp == NULL) {
		return;
	}

	/* wp 正好是使用中链表的第一个节点。 */
	if (head == wp) {
		head = wp->next;
	} else {
		prev = head;

		while (prev != NULL && prev->next != wp) {
			prev = prev->next;
		}

		if (prev == NULL) {
			printf("Watchpoint is not active\n");
			return;
		}

		prev->next = wp->next;
	}

	/* 放回空闲链表开头。 */
	wp->next = free_;
	free_ = wp;
}

WP *get_wp_head(void) {
	return head;
}

bool check_watchpoints(swaddr_t instr_eip) {//检查
	WP *wp;
	bool success;
	bool triggered;
	uint32_t new_value;

	wp = head;
	triggered = false;

	while (wp != NULL) {
		new_value = expr(wp->expression, &success);

		if (!success) {
			printf("Failed to evaluate watchpoint %d: %s\n",
					wp->NO, wp->expression);
			wp = wp->next;
			continue;
		}

		if (new_value != wp->old_value) {
			printf("Hint watchpoint %d at address 0x%08x\n",
					wp->NO, instr_eip);

			printf("Expression: %s\n", wp->expression);
			printf("Old value:  0x%08x\n", wp->old_value);
			printf("New value:  0x%08x\n", new_value);

			wp->old_value = new_value;
			triggered = true;
		}

		wp = wp->next;
	}

	return triggered;
}
