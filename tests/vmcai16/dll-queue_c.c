#include <stdlib.h>
#include <verifier-builtins.h>

typedef struct TSLL
{
	struct TSLL* next;
	struct TSLL* prev;
	int data;
} SLL;

int main()
{
	// create the head
	SLL* head = malloc(sizeof(SLL));
	head->next = NULL;
	head->prev = NULL;
	head->data = 0;
	SLL* item = head;
	int status = 0;
	
	while (__VERIFIER_nondet_int())
	{
		item->next = malloc(sizeof(SLL));
		item->next->prev = item;
		item = item->next;
		item->next = NULL;

		if (!status)
		{
			item->data = 1;
			status = 1;
		}
		else if (status == 1)
		{
			item->data = 2;
			status = 2;
		}
		else if (status >= 2)
		{
			item->data = 3;
			status = 3;
		}

		__VERIFIER_assert(head != NULL);
		__VERIFIER_assert(item != NULL);
	}

	__VERIFIER_assert(head != NULL);
	if (status == 1)
	{
		__VERIFIER_assert(head != NULL);
		__VERIFIER_assert(head->next != NULL);
	}
	if (status == 2)
	{
		__VERIFIER_assert(head != NULL);
		__VERIFIER_assert(head->next != NULL);
		__VERIFIER_assert(head->next->next != NULL);
	}
	if (status == 3)
	{
		__VERIFIER_assert(head != NULL);
		__VERIFIER_assert(head->next != NULL);
		__VERIFIER_assert(head->next->next != NULL);
		__VERIFIER_assert(head->next->next->next != NULL);
		__VERIFIER_assert(head->next->next->next->data == 3);
	}

	item = head->next;
	while(item && __VERIFIER_nondet_int())
	{
		__VERIFIER_assert(!status || item->data > 0);
		item = item->next;
	}


	while(head)
	{
		item = head;
		head = head->next;
		free(item);
	}

	return 0;
}
