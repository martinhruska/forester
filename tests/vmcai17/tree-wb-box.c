#include <stdlib.h>
#include <verifier-builtins.h>

#define ALLOC_NODE(N, PAR, COLOR)                             \
    N = malloc(sizeof(TREE));                                 \
    N->left = NULL;                                           \
    N->right = NULL;                                          \
    N->data = COLOR;                                          \
    N->parent = PAR;                                          \
    __VERIFIER_assert(N != NULL);


#define WHITE 0
#define BLUE 1

typedef struct TTREE
{
	struct TTREE* left;
	struct TTREE* right;
	struct TTREE* parent;
	int data;
} TREE;

int main()
{
	// create the head
	TREE* root = NULL;
	ALLOC_NODE(root, NULL, WHITE)
	TREE* n = root;

	// create an arbitrary white tree
	while (__VERIFIER_nondet_int())
	{
		n = root;
		__VERIFIER_assert(root != NULL);
		__VERIFIER_assert(n != NULL);

		while (n->left != NULL || n->right != NULL)
		{
			__VERIFIER_assert(n != NULL);
			if (n->left != NULL && n->right != NULL && __VERIFIER_nondet_int())
				n = n->left;
			else if (n->right != NULL)
				n = n->right;
			else if (n->left != NULL)
				n = n->left;
			else
				__VERIFIER_assert(0);
		}


		// create a node
		// if (__VERIFIER_nondet_int())
		{
			ALLOC_NODE(n->left, n, WHITE);
		}
		// if (__VERIFIER_nondet_int())
		{
			ALLOC_NODE(n->right, n, WHITE);
		}
	}

	// insert a blue guy
	if (__VERIFIER_nondet_int()) { // the blue guy will be the root
		ALLOC_NODE(n, NULL, BLUE);
		if (__VERIFIER_nondet_int())
		{
			n->left = root;
		}
		else
		{
			n->right = root;
		}
		root = n;
	}
	else 
	{ // choose a predecessor of the blue guy
		n = root;
		while ((n->left != NULL || n->right != NULL) && __VERIFIER_nondet_int())
		{
			__VERIFIER_assert(n != NULL);
			if (n->left != NULL && n->right != NULL && __VERIFIER_nondet_int())
				n = n->left;
			else if (n->right != NULL)
				n = n->right;
			else if (n->left != NULL)
				n = n->left;
			else
				__VERIFIER_assert(0);
		}

		// insert the blue guy
		TREE* y = NULL;
		ALLOC_NODE(y, n, BLUE);
		if (__VERIFIER_nondet_int())
		{
			y->left = n->left;
			if (n->left != NULL)
				n->left->parent = y;
			n->left = y;
		}
		else
		{
			y->right = n->right;
			if (n->right != NULL)
				n->right->parent = y;
			n->right = y;
		}
	}

	// check the invariant
	n = root;

	__VERIFIER_assert(n != NULL);
	while ((n->left != NULL || n->right != NULL) && n->data != BLUE)
	{
		__VERIFIER_assert(n != NULL);
		if (n->left != NULL && n->right != NULL && __VERIFIER_nondet_int())
			n = n->left;
		else if (n->right != NULL)
			n = n->right;
		else if (n->left != NULL)
			n = n->left;
		else
			__VERIFIER_assert(0);
		__VERIFIER_assert(n != NULL);
	}

	__VERIFIER_assert(n != NULL);
    // look for another blue guy
	while ((n->left != NULL || n->right != NULL))
	{
		__VERIFIER_assert(n != NULL);
		if (n->left != NULL && n->right != NULL && __VERIFIER_nondet_int())
			n = n->left;
		else if (n->right != NULL)
			n = n->right;
		else if (n->left != NULL)
			n = n->left;
		else
			__VERIFIER_assert(0);
		
		__VERIFIER_assert(n != NULL);
		__VERIFIER_assert(n->data != BLUE);
	}

	// destroy the list
	/*
	while (root)
	{
		TREE *pred = NULL;
		n = root;
		while (n->left != NULL || n->right != NULL)
		{
			pred = n;
			if (n->left != NULL && __VERIFIER_nondet_int())
			{
				n = n->left;
			}
			else if (n->right != NULL)
			{
				n = n->right;
			}
			else if (n->left != NULL)
			{
				n = n->left;
			}
		}
		if (pred) {
			if (n == pred->left)
				pred->left = NULL;
			else
				pred->right = NULL;
		} else
			root = NULL;
		free(n);
	}
	*/

	return 0;
}
