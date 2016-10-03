/* 
 * A slightly obfuscated implementation of skip lists without using ordering and height counters.
 * For a better implementation, see, e.g., http://eternallyconfuzzled.com/tuts/datastructures/jsw_tut_skip.aspx
 * or http://ck.kolivas.org/patches/bfs/test/bfs406-skiplists.patch
 *
 * We assume the height to be fixed to 2 and we always have the maximum height at the head and tail
 * nodes---in other words, we do not let the height grow/shrink. Also, we do not consider a dynamic
 * number of next pointers in the nodes.
 *
 */

#include <stdlib.h>
#include <verifier-builtins.h>

// a skip list node with three next pointers
struct sl_item {
	struct sl_item *n1, *n2;
};

// a skip list
struct sl {
	struct sl_item *head, *tail;
};

struct sl_item* alloc_or_die(void)
{
	struct sl_item *pi = malloc(sizeof(struct sl_item));

	return pi;
}

struct sl* create_sl_with_head_and_tail(void)
{
	struct sl *sl = malloc(sizeof(*sl));

	sl->head = malloc(sizeof(struct sl_item));
	sl->tail = malloc(sizeof(struct sl_item));

	sl->head->n2 = sl->head->n1 = sl->tail;
	sl->tail->n2 = sl->tail->n1 = NULL;

	return sl;
}

// The function inserts one node of a random height to a randomly chosen position in between of 
// the head and tail.
void sl_random_insert(struct sl *sl)
{
	// a1, a2 remember the nodes before the inserted one at the particular levels
	struct sl_item *a1, *a2;
	struct sl_item *new;

	a2 = sl->head;
	while (a2->n2 != sl->tail && __VERIFIER_nondet_int())
		a2 = a2->n2;

	// moving randomly on the 1st level, not going behind a2->n2
	a1 = a2;
	while (a1->n1 != a2->n2 && __VERIFIER_nondet_int())
		a1 = a1->n1;

	// allocation and insertion of a new node
	new = malloc(sizeof(struct sl_item));
	// always insert at level 1
	new->n1 = a1->n1;
	a1->n1 = new;

	// choose whether to insert at level 2
	if (__VERIFIER_nondet_int()) {
		new->n2 = a2->n2;
		a2->n2 = new;
	}
}

void destroy_sl(struct sl *sl)
{
	struct sl_item *tmp;

	while (sl->head) {
		tmp = sl->head;
		sl->head = sl->head->n1;
		free(tmp);
	}
	free(sl);
}

int main()
{
	// struct sl *sl = create_sl_with_head_and_tail();
	struct sl *sl = malloc(sizeof(*sl));

	sl->head = malloc(sizeof(struct sl_item));
	sl->tail = malloc(sizeof(struct sl_item));

	sl->head->n2 = sl->head->n1 = sl->tail;
	sl->tail->n2 = sl->tail->n1 = NULL;

	while (__VERIFIER_nondet_int())
	{
		// a1, a2 remember the nodes before the inserted one at the particular levels
		struct sl_item *a1, *a2;
		struct sl_item *new;

		a2 = sl->head;
		while (a2->n2 != sl->tail && __VERIFIER_nondet_int())
			a2 = a2->n2->n2;

		// moving randomly on the 1st level, not going behind a2->n2
		a1 = a2;
		while (a1->n1 != a2->n2 && __VERIFIER_nondet_int())
			a1 = a1->n1->n1;

		// allocation and insertion of a new node
		new = malloc(sizeof(struct sl_item));
		// always insert at level 1
		new->n1 = a1->n1->n1;
		a1->n1 = new;

		// choose whether to insert at level 2
		if (__VERIFIER_nondet_int()) {
			new->n2 = a2->n2;
			a2->n2 = new;
		}
		// sl_random_insert(sl);
	}

	// destroy_sl(sl);

	struct sl_item *tmp;

	while (sl->head) {
		tmp = sl->head;
		sl->head = sl->head->n1;
		free(tmp);
	}
	free(sl);

	return 0;
}
