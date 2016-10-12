#include <stdlib.h>
#include <verifier-builtins.h>

#define ALLOC_NODE(N, COLOR)                                  \
	N = malloc(sizeof(TREE));                                 \
N->left = NULL;                                           \
N->right = NULL;                                          \
N->data = COLOR;


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
					if (__VERIFIER_nondet_int()) goto go_left;
					else goto go_right;
				else
					// left defined, right not:
					// choose to go on left or add to the right
					if (__VERIFIER_nondet_int()) goto go_left;
					else goto add_right;
			else
				if (n->right) 
					// right defined, left not:
					// choose to go on right or add to the left
					if (__VERIFIER_nondet_int()) goto go_right;
					else goto add_left;
				else
					// both undefined: choose where to add
					if (__VERIFIER_nondet_int())  goto add_left;
					else goto add_right;

go_left: n = n->left; continue;
go_right: n = n->right; continue;

add_left: ALLOC_NODE(n->left, WHITE); break;
add_right: ALLOC_NODE(n->right, WHITE); break;

		} // end of adding one guy
	} // end of creation of the white tree

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
		while (1) 
		{
			if (n->left != NULL && __VERIFIER_nondet_int())
				n = n->left;
			else if (n->right != NULL && __VERIFIER_nondet_int())
				n = n->right;
			else
				break;
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

	// look for the first blue guy
	while (n != NULL && n->data != BLUE)
	{
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

	// if not missed the blue guy, look for another
	while (n != NULL)
	{
		if (n->left)
			if (n->right)
				// choose which way to go
				if (__VERIFIER_nondet_int())
					n = n->left;
				else
					n = n->right;
			else n = n->left; // must go left
		else n = n->right; // must go right
		if (n != NULL) 
		{
			__VERIFIER_assert(n->data != BLUE);
		}
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

	return 0;
}
