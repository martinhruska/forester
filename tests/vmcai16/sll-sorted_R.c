#include <stdlib.h>
#include <verifier-builtins.h>

typedef struct TSLL
{
	struct TSLL* next;
	int data;
} SLL;

int main()
{
	// create the head
	SLL* head = malloc(sizeof(SLL));
	head->next = NULL;
	head->data = 0;

	SLL* x = head;
	int marked = 0;

	// create an arbitrarily long tail
	while (__VERIFIER_nondet_int() || !marked)
	{
		// create a node
		x->next = malloc(sizeof(SLL));
		x = x->next;
		x->next = NULL;

		if (__VERIFIER_nondet_int())
		{
			marked = 1;
		}

		if (marked)
			x->data = 1;
		else
			x->data = 0;
	}

	x = head;
	// check the invariant
	__VERIFIER_assert(NULL != x);
	marked = 0;

	while (x->next != NULL && x->next->data == 0)
	{
		x = x->next;
	}

	while (x->next != NULL && __VERIFIER_nondet_int())
	{
		x = x->next;
		__VERIFIER_assert(x->data == 1);
	}

	SLL* tmp = malloc(sizeof(SLL));
	tmp->data = 1;
	tmp->next = NULL;
	if (x->next == NULL)
		x->next = tmp;
	else
	{
		tmp->next = x->next;
		x->next = tmp;
	}

	marked = 0;
	x = head;
	// destroy the list
	while (x != NULL && x->data != 1)
	{
		marked = x->data;
		__VERIFIER_assert(x->data == 0);
		__VERIFIER_assert(marked == 0);
		x = x->next;
	}
	while (x != NULL)
	{
		marked = x->data;
		__VERIFIER_assert(x->data != 1);
		__VERIFIER_assert(marked == 1);
		x = x->next;
	}

	/*
	marked = 0;
	x = head;
	while (x != NULL)
	{
		marked = x->data;
		__VERIFIER_assert(x->data == 1 || marked == 0);
		x = x->next;
	}
	*/

	x = head;
	// destroy the list
	while (x != NULL)
	{
		head = x;
		x = x->next;
		free(head);
	}

	return 0;
}
