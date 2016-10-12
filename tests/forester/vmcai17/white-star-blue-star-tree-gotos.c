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

	// add blue guys
	while (__VERIFIER_nondet_int())
	{
		n = root;
		while (1)
		{
			if (n->left)
                                if (n->right) 
                                        // both defined: choose where to go
                                        if (__VERIFIER_nondet_int()) goto go_snd_left;
                                        else goto go_snd_right;
                                else
                                        // left defined, right not:
                                        // choose to go on left or add to the right
                                        if (__VERIFIER_nondet_int()) goto go_snd_left;
                                        else goto add_snd_right;
                        else
                                if (n->right) 
                                        // right defined, left not:
                                        // choose to go on right or add to the left
                                        if (__VERIFIER_nondet_int()) goto go_snd_right;
                                        else goto add_snd_left;
                                else
                                        // both undefined: choose where to add
		                        if (__VERIFIER_nondet_int())  goto add_snd_left;
                        		else goto add_snd_right;

                        go_snd_left: n = n->left; continue;
                        go_snd_right: n = n->right; continue;

                        add_snd_left: ALLOC_NODE(n->left, BLUE); break;
                        add_snd_right: ALLOC_NODE(n->right, BLUE); break;

                } // end of adding one guy
	} // end of creation of the blue branches

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

        // check that now there are blue guys only 
	while (n != NULL)
        {
                __VERIFIER_assert(n->data == BLUE);
                if (n->left)
                        if (n->right)
                                // choose which way to go
                                if (__VERIFIER_nondet_int())
			                n = n->left;
                                else
                                        n = n->right;
                        else n = n->left; // must go left
                else n = n->right; // must go right
        }

	// destroy the tree, preferring left branches
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

	return 0;
}
