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
