#include <stdlib.h>
#include <verifier-builtins.h>

#define WHITE 0
#define BLUE 1

#define ALLOC_NODE(X, C) \
	X->next = malloc(sizeof(SLL)); \
	X = X->next; \
	X->next = NULL; \
	X->data = C;

#define ALLOC_NONDET(X)          \
	while (__VERIFIER_nondet_int()) \
	{ \
		ALLOC_NODE(X, WHITE) \
	}

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
	head->data = WHITE;

	SLL* x = head;
	ALLOC_NONDET(x);
	ALLOC_NODE(x, BLUE);
	ALLOC_NONDET(x);

	// check the invariant
	x = head;

    // look for the first blue guy
    while (x->data != BLUE)
	{
		x = x->next; // fails if there is no blue guy
    }

    // look for another blue guy
    x = x->next;
    while (x)
	{
		if (x->data == BLUE)
			__VERIFIER_assert(0);
        x = x->next;
    }

	// destroy the list
	x = head;
	while (x != NULL)
	{
		head = x;
		x = x->next;
		free(head);
	}

	return 0;
}
