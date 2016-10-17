#include <stdlib.h>
#include <verifier-builtins.h>

#define ALLOC_NODE(N, COLOR)                                  \
    N = malloc(sizeof(TREE));                                 \
    N->left = NULL;                                           \
    N->right = NULL;                                          \
    N->data = COLOR;                                          \
    __VERIFIER_assert(N != NULL);

#define IS_LEAF(N) \
	n != NULL && n->left == NULL && n->right == NULL

#define WHITE NULL
#define BLUE null

typedef struct TTREE
{
	struct TTREE* left;
	struct TTREE* right;
	struct TTREE* data;
} TREE;

int main()
{
	// create the head
	TREE* root = NULL;
	ALLOC_NODE(root, WHITE)
	TREE* n = root;
	TREE* null = NULL;
	ALLOC_NODE(null, WHITE)

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
		if (__VERIFIER_nondet_int())
		{
			ALLOC_NODE(n->left, WHITE);
		}
		// else
		if (__VERIFIER_nondet_int())
		{
			ALLOC_NODE(n->right, WHITE);
		}
	}

	// insert a blue guy
	if (__VERIFIER_nondet_int()) { // the blue guy will be the root
		ALLOC_NODE(n, BLUE);
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
	{ // choose a parent of the blue guy
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
		ALLOC_NODE(y, BLUE);
		if (__VERIFIER_nondet_int())
		{
			y->left = n->left;
			n->left = y;
		}
		else
		{
			y->right = n->right;
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
		n = root;
		while (n->left != NULL || n->right != NULL)
		{
			if (IS_LEAF(n->left))
			{
				free(n->left);
				n->left = NULL;
			}
			if (IS_LEAF(n->right))
			{
				free(n->right);
				n->right = NULL;
			}

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
		if (n == root)
		{
			free(root);
			root = NULL;
		}
	}
	*/

	return 0;
}