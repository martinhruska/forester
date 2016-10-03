/*
 * Singly linked list bubble-sort
 *
 * boxes:
 */

#include <stdlib.h>
#include <stdbool.h>
#include <verifier-builtins.h>

int main() {

	struct T {
		struct T* next;
		int data;
	};

	struct T* x = NULL;
	struct T* y = NULL;

	while (__VERIFIER_nondet_int()) {
		y = malloc(sizeof(*y));
		if (__VERIFIER_nondet_int())
		{
			y->data = 0;
		}
		else
		{
			y->data = 1;
		}
		y->next = x;
		x = y;
	}

	if (!x)
		return 0;

	struct T* pred, * succ;

	bool sorted = false;

	while (!sorted) {
		sorted = true;
		y = x;
		pred = NULL;
		while (y && y->next) {
			if (y->data > y->next->data) {
				succ = y->next;
				if (pred) pred->next = succ; // is it head?
				else x = succ;
				y->next = succ->next;
				succ->next = y;
				sorted = false;
			}
			pred = y;
			y = y->next;
		}
	}

	y = x;

	while (y != NULL)
	{
		if (y->next != NULL)
			__VERIFIER_assert(y->data <= y->next->data);

		y = y->next;
	}

	while (x != NULL) {
		y = x;
		x = x->next;
		free(y);
	}

	return 0;

}
