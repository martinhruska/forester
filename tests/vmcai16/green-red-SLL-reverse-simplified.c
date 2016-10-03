#include <stdlib.h>
#include <verifier-builtins.h>

#define WHITE 0
#define GREEN 1
#define RED 2

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

	ALLOC_NODE(x, GREEN)
	ALLOC_NODE(x, RED)
	ALLOC_NONDET(x)

	// reverse
	x = NULL;
	while(head != NULL)
	{
		SLL* next = head->next;
		head->next = x; 
		x = head; 
		head = next; 
	}
        head = x;

	// check the invariat
	x = head;
	while (x != NULL)
	{
		if (x->data == RED)
		{
			x = x->next;
			if (x->data != GREEN)
				__VERIFIER_assert(0);
		}

		x = x->next;
	}

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
