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

#define WHITE 0
#define BLUE 1

typedef struct TTREE
{
	struct TTREE* left;
	struct TTREE* right;
	int data;
} TREE;

int main()
{
	// create the head
	TREE* root = NULL;
	ALLOC_NODE(root, WHITE)
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

	n = root;
	// Choose one node to mark it blue
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
	__VERIFIER_assert(n != NULL);
	n->data = BLUE;

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

	/*
	// destroy the list
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
