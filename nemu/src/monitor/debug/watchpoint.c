#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

WP* new_wp() {
	WP *p;
	p = free_;
	if (p == NULL) {
		printf("No free room!");
		assert(0);
	}
	free_ = free_->next;
	p->next = head;
	head = p;
	return head;
}

void free_wp(WP *wp) {
	WP *pre, *p;
	/*No watchpoint*/
	if (head == NULL) {
		printf("NO watchpoint at all!");
		assert(0);
	}

	pre = head;
	p = free_;
	/*Have free watchpoint*/
	if (free_ != NULL) {
		while (p->next != NULL){
			p = p->next;
		}

		/*Head node is just the wp node*/
		if (head == wp) {
			head = head->next;
			p->next = wp;
		}	
		/*find the pre node of wp*/
		else{
			while (pre->next != NULL && pre->next != wp){
				pre = pre->next;
			}
			if (pre->next == NULL) printf("No such wp!");
			else if(pre->next == wp) {
				p->next = wp;
				pre->next = pre->next->next;
			}
			else {
				printf("Unknown Error!");
				assert(0);
			}
		}
	}
	/*No free point*/
	else {
		/*Head node is just the wp node*/
		if (head == wp) {
			head = head->next;
			free_ = wp;
		}
		/*find the pre node of wp*/
		else{
			while (pre->next != NULL && pre->next != wp){
				pre = pre->next;
			}
			if (pre->next == NULL) printf("No such wp!");
			else if(pre->next == wp) {
				free_ = wp;
				pre->next = pre->next->next;
			}
			else {
				printf("Unknown Error!");
				assert(0);
			}
		}
	}
	wp->next =NULL;
	wp->new_val = 0;
	wp->old_val = 0;
	wp->expr[0] = '\0';
}

int set_watchpoint(char *e) {
	WP *p;
	bool success = false;
	p = new_wp();
	strcpy(p->expr, e);
	p->old_val = expr(e, &success);
	if (!success) {
		printf("Wrong Expression\n");
		assert(0);
	}
	return p->NO;
}

bool delete_watchpoint(int NO) {
	if (NO >= NR_WP) {
		printf("The NO is no more than 32!\n");
		return false;
	}
	else {
		WP *p;
		p = &wp_pool[NO];
		free_wp(p);
		printf("Delete watchpoint %d successfully!\n", NO);
		return true;
	}
}

void list_watchpoint() {
	WP *p;
	p = head;
	while(p != NULL) {
		printf("%d\t%s\t0x%08x\n", p->NO, p->expr, p->old_val);
		p = p->next;
	}
}
