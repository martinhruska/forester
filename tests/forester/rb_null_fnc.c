#include <stdlib.h>

#include "verifier-builtins.h"

int __nondet();
//#define __nondet() 1

typedef enum {RED,BLACK} Colour;
struct T {
	struct T* left;
	struct T* right;
	struct T* parent;
	Colour colour;
};



//rotate(X,left,right) gives rotate LEFT
//rotate(X,right,left) gives rotate RIGHT
//
void rotateLeft(struct T* X, struct T** root)
{
    __VERIFIER_assert(X != NULL);                                          
    struct T* r = X->right;                                               
	__VERIFIER_assert(r != NULL);                                          
    X->right = r->left;                                         
	if (r->left != NULL) 
    r->left->parent = X;                                        
    __VERIFIER_assert(r != NULL);                                  
    r->parent = X->parent;                                      
    if (X->parent == NULL) {*root = r;}                         
    else {                                                      
            __VERIFIER_assert(X->parent != NULL);                                  
            if (X == X->parent->left) {X->parent->left = r;}    
            else {X->parent->right = r;}                        
    }                                                           
    __VERIFIER_assert(r != NULL);                                  
    r->left = X;                                                
    X->parent = r;                                              
    r = NULL;
}

void rotateRight(struct T* X, struct T** root)
{
    __VERIFIER_assert(X != NULL);                                          
    struct T* r = X->left;                                               
	__VERIFIER_assert(r != NULL);                                          
    X->left = r->right;                                         
	if (r->right != NULL) 
    r->right->parent = X;                                        
    __VERIFIER_assert(r != NULL);                                  
    r->parent = X->parent;                                      
    if (X->parent == NULL) {*root = r;}                         
    else {                                                      
            __VERIFIER_assert(X->parent != NULL);                                  
            if (X == X->parent->right) {X->parent->right = r;}    
            else {X->parent->left = r;}                        
    }                                                           
    __VERIFIER_assert(r != NULL);                                  
    r->right = X;                                                
    X->parent = r;                                              
    r = NULL;
}


int main() {
	struct T* r; //an auxliary var used in the ROTATE macro

	//the sentinell node used instead of NULL
	//struct T null = {.left=NULL,.right=NULL,.parent=NULL,.colour=BLACK};

	//start with an empty tree
	struct T* root = NULL;

	//insert a random number of random nodes
	while (__VERIFIER_nondet_int()) {
		if (root != NULL && root->colour != BLACK)
			*(int*)(NULL) = 0;

		//create a new node z
		struct T* z = root;
		struct T* y = NULL;
		
		//insert z as a random leaf
		int lastChoice;
		while (z != NULL) {
			y = z;
			if (__VERIFIER_nondet_int()) {
				z = z->left;
				lastChoice = 0;
				//			    if (x == NULL) {y->left = z;}
			}
			else {
				z = z->right;
				lastChoice = 1;
				//			    if (x == NULL) {y->right = z;}
			}
		}


		z = malloc(sizeof(struct T));
		z->parent = y;
		//make z root if the tree is empty
		if (y == NULL) {root = z;}
		else {
			if (lastChoice == 0) {y->left = z;}
			else {y->right = z; lastChoice = 0;}
		}
		y = NULL;
		//make z RED
		z->left = NULL;
		z->right = NULL;
		z->colour = RED;

		//rebalance the tree
		while (z->parent != NULL && z->parent->colour == RED) {
			if (z->parent == z->parent->parent->left) {
				y = z->parent->parent->right;
				if (y != NULL && y->colour == RED) {
					z->parent->colour = BLACK;
					y->colour = BLACK;
					y = NULL;
					z->parent->parent->colour = RED;
					z = z->parent->parent;
				}
				else {
					y = NULL;
					if (z == z->parent->right) {
						z = z->parent;
						rotateLeft(z, &root);
					}
					z->parent->colour = BLACK;
					z->parent->parent->colour = RED;
					y = z->parent->parent;
					//RIGHT_ROTATE(z->parent->parent)
					rotateRight(y, &root);
					y = NULL;
				}
			}
			else {
				y = z->parent->parent->left;
				if (y != NULL && y->colour == RED) {
					z->parent->colour = BLACK;
					y->colour = BLACK;
					y = NULL;
					z->parent->parent->colour = RED;
					z = z->parent->parent;
				}
				else {
					y = NULL;
					if (z == z->parent->left) {
						z = z->parent;
						rotateRight(z, &root);
					}
					z->parent->colour = BLACK;
					z->parent->parent->colour = RED;
					y = z->parent->parent;
					//LEFT_ROTATE(z->parent->parent)
					rotateLeft(y, &root);
					y = NULL;
				}
			}
		}
		z = NULL;
		root->colour = BLACK;
	}

	//Test that:
	//there are no two succesive random nodes


	//kill the tree (using the aux var r)
	/*
	while (root != NULL) {
		r = root;


		while (r->left != NULL || r->right != NULL) {
			if (r->left != NULL)
				r = r->left;
			else
				r = r->right;

		}

		if (r->parent != NULL) {
			if (r == r->parent->left)
				r->parent->left = NULL;
			else
				r->parent->right = NULL;
		} else
			root = NULL;
		free(r);
	}
	*/

	return 42;
}
