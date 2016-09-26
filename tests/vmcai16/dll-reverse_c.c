#include <stdlib.h>
#include <verifier-builtins.h>

#define WHITE 0
#define GREEN 1
#define RED 2

#define ALLOC_NODE(X, C) \
	X->next = malloc(sizeof(DLL)); \
	X->next->prev = x; \
	X = X->next; \
	X->next = NULL; \
	X->data = C;

#define ALLOC_NONDET(X)          \
	while (__VERIFIER_nondet_int()) \
	{ \
		ALLOC_NODE(X, WHITE) \
	}

typedef struct TDLL
{
	struct TDLL* next;
	struct TDLL* prev;
	int data;
} DLL;

int main()
{
	// create the head
	DLL* head = malloc(sizeof(DLL));
	head->next = NULL;
	head->prev = NULL;
	head->data = WHITE;

	DLL* x = head;

	// allocate list
	ALLOC_NONDET(x)

	// choose a node which will be colored
	x = head;
	while (x->next != NULL && __VERIFIER_nondet_int())
	{
		__VERIFIER_assert(x != NULL);
		x = x->next;
	}
	__VERIFIER_assert(x != NULL);

	x->data = GREEN;
	
	if (x->next == NULL)
	{
		ALLOC_NODE(x, RED)
	}
	else
		x->next->data = RED;

	// reverse
	DLL* prev = NULL;
	x = head;
	while(x != NULL)
	{
		DLL* next = x->next;
		// | PREV == X->prev | X | NEXT == X->NEXT
		x->next = prev; // Previous before reversion is now the next one
		x->prev = next;
		// | X->NEXT == PREV | X | NEXT == X->PREV
		prev = x; // Remember this one
		// | X->NEXT | PREV == X | NEXT == X->PREV
		x = next; // Go to the next
		// | ... | PREV | X == NEXT
	}
	head = prev;

	// check the invariat
	x = head;
	while (x != NULL)
	{
		int t1 = x->data;
		int t2 = 0;
		if (t1 == RED)
			t2 = x->next->data;
		__VERIFIER_assert(t1 != RED || t2 == GREEN);
		// if (x->data == RED)
		// 	__VERIFIER_assert(x->next->data == GREEN);
		if (x->data == RED)
			x = x->next;

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
