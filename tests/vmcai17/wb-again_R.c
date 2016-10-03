#include <stdlib.h>
#include <verifier-builtins.h>

#define ALLOC_NODE(N, COLOR)                                  \
	N = malloc(sizeof(TREE));                                 \
    N->left = NULL;                                           \
    N->right = NULL;                                          \
    N->data = COLOR;

#define IS_LEAF(N) n != NULL && n->left == NULL && n->right == NULL

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
					if (__VERIFIER_nondet_int())
						n = n->left;
					else
					{
						ALLOC_NODE(n->right, WHITE);
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
						ALLOC_NODE(n->left, WHITE);
						break;
					}
				else
					// both undefined: choose where to add
					if (__VERIFIER_nondet_int())
					{
						ALLOC_NODE(n->left, WHITE);
						break;
					}
					else
					{
						ALLOC_NODE(n->right, WHITE);
						break;
					}

		} // end of adding one guy
	} // end of creation of the white tree

        // recolour a randomly chosen guy to a blue one
        n = root;
	while ((n->left != NULL || n->right != NULL) && __VERIFIER_nondet_int()) 
	{
		if (n->left != NULL && __VERIFIER_nondet_int())
			n = n->left;
		else if (n->right != NULL)
			n = n->right;
		else
			n = n->left;
	}

	__VERIFIER_assert(n != NULL);
	n->data = BLUE;

	// start destroying the tree, preferring left branches
        // if a blue guy is found, change to white and jump to the below loop
	n = root;
	while (1)
	{
		if (n->data == BLUE) {
			n->data = WHITE;
			break;
		}
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

	// finish destroying the list, preferring left branches
        // if a blue guy is found, report an error
	n = root;
	while (1)
	{
        __VERIFIER_assert(n->data == WHITE);
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

	return 0;
}
