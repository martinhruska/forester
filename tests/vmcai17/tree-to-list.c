#include <stdlib.h>
#include <verifier-builtins.h>

#define WHITE 0
#define BLUE 1

#define ALLOC_NODE(N, COLOR)                                  \
	N = malloc(sizeof(TREE));                                 \
    N->left = NULL;                                           \
    N->right = NULL;                                          \
    if (COLOR == BLUE) \
		N->data = BLUE; \
	else if (__VERIFIER_nondet_int()) \
		N->data = BLUE; \
	else \
		N->data = WHITE;

#define IS_LEAF(N) n != NULL && n->left == NULL && n->right == NULL

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
		while (1)
		{
			if (n->left)
				if (n->right) 
					// both defined: choose where to go
					if (__VERIFIER_nondet_int())
						n = n->left;
					else
						n = n->right;
				else
					// left defined, right not:
					// choose to go on left or add to the right
					if (__VERIFIER_nondet_int() || n->data == BLUE)
						n = n->left;
					else
					{
						ALLOC_NODE(n->right, n->data);
						break;
					}
			else
				if (n->right) 
					// right defined, left not:
					// choose to go on right or add to the left
					if (__VERIFIER_nondet_int())
						n = n->right;
					else
					{
						ALLOC_NODE(n->left, n->data);
						break;
					}
				else
					// both undefined: choose where to add
					if (__VERIFIER_nondet_int() || n->data == BLUE)
					{
						ALLOC_NODE(n->left, n->data);
						break;
					}
					else
					{
						ALLOC_NODE(n->right, n->data);
						break;
					}

		} // end of adding one guy
	} // end of creation of the white tree

	// check the invariant
	n = root;

	// look for the first blue guy
	while (n != NULL)
	{
		__VERIFIER_assert(n->data == WHITE || n->right == NULL);
		if (n->left)
			if (n->right)
				// choose which way to go
				if (__VERIFIER_nondet_int())
					n = n->left;
				else
					n = n->right;
			else n = n->left; // must go left
		else n = n->right; // must go right, may be to null
	}

	// destroy the list, preferring left branches
	/*
	n = root;
	while (1)
	{
		if (n->left != NULL)
			if (n->left->left == NULL && n->left->right == NULL) {
				free(n->left);
				n->left = NULL;
				n = root; // back to root
			}
			else n = n->left;
		else if (n->right != NULL)
			if (n->right->left == NULL && n->right->right == NULL) {
				free(n->right);
				n->right = NULL;
				n = root; // back to root
			}
			else n = n->right;
		else {
			free(n); // freeing the root
			break;
		}
	}
	*/

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


	return 0;
}
