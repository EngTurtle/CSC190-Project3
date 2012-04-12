/* FILE psb_bag.c
 *    Implementation of the bag ADT using an psb tree.
 * Author: Tommy Pearson and Oliver Liang, March 2012.
 */

/******************************************************************************
 *  Types and Constants.                                                      *
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "bag.h"


/* TYPE psb_node_t -- A node in an psb tree. */
typedef struct psb_node {
    bag_elem_t elem;        /* the element stored in this node       */
    struct psb_node *left;  /* pointer to this node's left child     */
    struct psb_node *right; /* pointer to this node's right child    */
} psb_node_t;

/* TYPE struct bag -- Definition of struct bag from the header. */
struct bag {
    size_t size; /* number of elements in this bag */
    psb_node_t *root; /* root of the psb tree storing the elements */
    int (*cmp)(bag_elem_t, bag_elem_t); /* function to compare elements */
};

/******************************************************************************
 *  Declarations of helper functions -- including full documentation.         *
 ******************************************************************************/

/* FUNCTION psb_destroy
 *    Free the memory allocated for the binary tree rooted at a given node.
 * Parameters and preconditions:
 *    root: the root of the tree to free
 * Return value:  none
 * Side-effects:
 *    all the memory allocated for nodes in the subtree rooted at root has been
 *    freed
 */
static
void psb_destroy(psb_node_t *root);

/* FUNCTION psb_traverse
 *    Call a function on every element in a BST, given its root.
 * Parameters and preconditions:
 *    root: the root of the BST to traverse
 *    fun != NULL: a pointer to a function to apply to each element in the tree
 * Return value:  none
 * Side-effect:
 *    function fun has been called on each element in the tree rooted at root,
 *    in order
 */
static
void psb_traverse(const psb_node_t *root, void (*fun)(bag_elem_t));

/* FUNCTION psb_contains
 *    Return whether or not a BST contains a certain element, given the root.
 * Parameters and preconditions:
 *    root: the root of the BST to search
 *    elem != NULL: the element to search for
 *    cmp != NULL: the comparison function to use for the search
 * Return value:
 *    elem, if the BST rooted at 'root' contains it; NULL otherwise
 * Side-effects:  If the element is found, a rotation is done about its parent
 *    so that the the element moves closer to the root
 */
static
bag_elem_t psb_contains(const psb_node_t *root, bag_elem_t elem,
                        int (*cmp)(bag_elem_t, bag_elem_t),
                        int is_right, psb_node_t *parent,
                        int is_parent_right,
                        psb_node_t *grandparent);

/* FUNCTION psb_insert
 *    Add an element to a BST, given a pointer to its root.
 * Parameters and preconditions:
 *    root: a pointer to the root of the BST into which to insert
 *    elem != NULL: the element to insert
 *    cmp != NULL: the comparison function to use to find the insertion point
 * Return value:
 *    elem, if it was inserted; NULL in case of error
 * Side-effects:
 *    memory has been allocated for the new element, and the element has been
 *    added at the bottom
 */
static
bag_elem_t psb_insert(psb_node_t **root, bag_elem_t elem,
                      int (*cmp)(bag_elem_t, bag_elem_t));

/* FUNCTION psb_remove
 *    Remove an element from a BST, given a pointer to its root.
 * Parameters and preconditions:
 *    root: a pointer to the root of the BST into which to remove
 *    elem != NULL: the element to remove
 *    cmp != NULL: the comparison function to use to find the removal point
 * Return value:
 *    elem, if it was removed; NULL if the element was not there
 * Side-effects:
 *    memory has been freed for the element removed, and the tree structure has
 *    been adjusted accordingly
 */
static
bag_elem_t psb_remove(psb_node_t **root, bag_elem_t elem,
                      int (*cmp)(bag_elem_t, bag_elem_t));

/* FUNCTION psb_remove_min
 *    Remove and return the smallest element in a BST, given a pointer to its
 *    root.
 * Parameters and preconditions:
 *    root: a pointer to the root of the BST
 * Return value:
 *    the smallest element in the BST rooted at 'root'
 * Side-effects:
 *    memory has been freed for the node containing the smallest element
 */
static
bag_elem_t psb_remove_min(psb_node_t **root);

/* FUNCTION psb_remove_max
 *    Remove and return the largest element in a BST, given a pointer to its
 *    root.
 * Parameters and preconditions:
 *    root: a pointer to the root of the BST
 * Return value:
 *    the largest element in the BST rooted at 'root'
 * Side-effects:
 *    memory has been freed for the node containing the largest element
 */
static
bag_elem_t psb_remove_max(psb_node_t **root);

/* FUNCTION psb_rotate_to_the_left
 *    Perform a single rotation of *parent to the left -- the tree structure
 *    goes from    *parent         to          child
 *                  /   \                      /   \
 *                 A     child           *parent    C
 *                       /   \            /   \
 *                      B     C          A     B
 * Parameters and precondition:
 *    parent != NULL: a pointer to the root of the tree to rotate
 *                    (*parent != NULL and (*parent)->right != NULL)
 * Return value:  none
 * Side-effects:
 *    the subtree rooted at *parent has been modified by rotating *parent with
 *    its right child
 */
static
void psb_rotate_to_the_left(psb_node_t **parent);

/* psb_rotate_to_the_right
 *    Perform a single rotation of *parent to the right -- the tree structure
 *    goes from         *parent    to     child
 *                       /   \            /   \
 *                  child     C          A    *parent
 *                  /   \                      /   \
 *                 A     B                    B     C
 * Parameters and precondition:
 *    parent != NULL: a pointer to the root of the tree to rotate
 *                    (*parent != NULL and (*parent)->left != NULL)
 * Return value:  none
 * Side-effects:
 *    the subtree rooted at *parent has been modified by rotating *parent with
 *    its left child
 */
static
void psb_rotate_to_the_right(psb_node_t **parent);

/* FUNCTION psb_node_create
 *    Create a new psb_node.
 * Parameters and preconditions:
 *    elem: the element to store in the new node
 * Return value:
 *    pointer to a new node that stores elem and whose children are both NULL;
 *    NULL in case of error with memory allocation
 * Side-effects:
 *    memory has been allocated for the new node
 */
static
psb_node_t *psb_node_create(bag_elem_t elem);

/******************************************************************************
 *  Definitions of "public" functions -- see header file for documentation.   *
 ******************************************************************************/

bag_t *bag_create(int (*cmp)(bag_elem_t, bag_elem_t))
{
    bag_t *bag = malloc(sizeof(bag_t));
    if (bag) {
        bag->size = 0;
        bag->root = NULL;
        bag->cmp = cmp;
    }
    return bag;
}

void bag_destroy(bag_t *bag)
{
    psb_destroy(bag->root);
    free(bag);
}

size_t bag_size(const bag_t *bag)
{
    return bag->size;
}

void bag_traverse(const bag_t *bag, void (*fun)(bag_elem_t))
{
    psb_traverse(bag->root, fun);
}

bag_elem_t bag_contains(bag_t *bag, bag_elem_t elem)
{
    return psb_contains(bag->root, elem, bag->cmp, 3, NULL,
                        3, NULL);
}

bag_elem_t bag_insert(bag_t *bag, bag_elem_t elem)
{
    bag_elem_t e = psb_insert(&bag->root, elem, bag->cmp);
    if (e)  bag->size++;
    return e;
}

bag_elem_t bag_remove(bag_t *bag, bag_elem_t elem)
{
    bag_elem_t e = psb_remove(&bag->root, elem, bag->cmp);
    if (e)  bag->size--;
    return e;
}

/******************************************************************************
 *  Definitions of helper functions -- see above for documentation.           *
 ******************************************************************************/

void psb_destroy(psb_node_t *root)
{
    if (root) {
        psb_destroy(root->left);
        psb_destroy(root->right);
        free(root);
    }
}

void psb_traverse(const psb_node_t *root, void (*fun)(bag_elem_t))
{
    if (root) {
        psb_traverse(root->left, fun);
        (*fun)(root->elem);
        psb_traverse(root->right, fun);
    }
}

bag_elem_t psb_contains(const psb_node_t *root, bag_elem_t elem,
                        int (*cmp)(bag_elem_t, bag_elem_t),
                        int is_right, psb_node_t *parent,
                        int is_parent_right,
                        psb_node_t *grandparent)
{
    if (! root)
        return NULL;
    else if ((*cmp)(elem, root->elem) < 0)
        return psb_contains(root->left, elem, cmp, 0, root,
                             is_right, parent);
    else if ((*cmp)(elem, root->elem) > 0)
        return psb_contains(root->right, elem, cmp, 1, root,
                             is_right, parent);
    else /* ((*cmp)(elem, root->elem) == 0) */
        if (is_right == 1) {
            psb_rotate_to_the_left(&parent);
            if (is_parent_right) grandparent -> right = root;
            else grandparent -> left = root;
        } else if (! is_right) {
            psb_rotate_to_the_right(&parent);
            if (is_parent_right) grandparent -> right = root;
            else grandparent -> left = root;
        }
        return root->elem;
}

bag_elem_t psb_insert(psb_node_t **root, bag_elem_t elem,
                      int (*cmp)(bag_elem_t, bag_elem_t))
{
    bag_elem_t inserted;

    if (! *root) {
        if ((*root = psb_node_create(elem)))
            inserted = (*root)->elem;
        else
            inserted = NULL;
    } else if ((*cmp)(elem, (*root)->elem) < 0) {
        (inserted = psb_insert(&(*root)->left, elem, cmp));
            /* The tree does not get rebalanced at this point */
    } else if ((*cmp)(elem, (*root)->elem) > 0) {
        (inserted = psb_insert(&(*root)->right, elem, cmp));
    } else { /* ((*cmp)(elem, (*root)->elem) == 0) */
        /* Insert into the left subtree */
        inserted = psb_insert(&(*root)->left, elem, cmp);
    }
    return inserted;
}

bag_elem_t psb_remove(psb_node_t **root, bag_elem_t elem,
                      int (*cmp)(bag_elem_t, bag_elem_t))
{
    bag_elem_t removed;

    if (! *root) {
        removed = NULL;
    } else if ((*cmp)(elem, (*root)->elem) < 0) {
        (removed = psb_remove(&(*root)->left, elem, cmp));
            /* The subtree does not get rebalanced */
    } else if ((*cmp)(elem, (*root)->elem) > 0) {
        (removed = psb_remove(&(*root)->right, elem, cmp));
            /* The subtree does not get rebalanced */
    } else { /* ((*cmp)(elem, (*root)->elem) == 0) */
        removed = (*root)->elem;
        if ((*root)->left && (*root)->right) {
            /* Remove from the left subtree */
            (*root)->elem = psb_remove_max(&(*root)->left);
        } else {
            /* Remove *root. */
            psb_node_t *old = *root;
            *root = (*root)->left ? (*root)->left : (*root)->right;
            free(old);
        }
    }

    return removed;
}

bag_elem_t psb_remove_min(psb_node_t **root)
{
    bag_elem_t min;

    if ((*root)->left) {
        /* *root is not the minimum, keep going */
        min = psb_remove_min(&(*root)->left);
    } else {
        /* Remove *root. */
        psb_node_t *old = *root;
        min = (*root)->elem;
        *root = (*root)->right;
        free(old);
    }

    return min;
}

bag_elem_t psb_remove_max(psb_node_t **root)
{
    bag_elem_t max;

    if ((*root)->right) {
        /* *root is not the maximum, keep going and rebalance if necessary. */
        max = psb_remove_max(&(*root)->right);

    } else {
        /* Remove *root. */
        psb_node_t *old = *root;
        max = (*root)->elem;
        *root = (*root)->left;
        free(old);
    }

    return max;
}

void psb_rotate_to_the_left(psb_node_t **parent)
{
    /* Rearrange pointers. */
    psb_node_t *child = (*parent)->right;
    (*parent)->right = child->left;
    child->left = *parent;
    *parent = child;
}

void psb_rotate_to_the_right(psb_node_t **parent)
{
    /* Rearrange pointers. */
    psb_node_t *child = (*parent)->left;
    (*parent)->left = child->right;
    child->right = *parent;
    *parent = child;
}


psb_node_t *psb_node_create(bag_elem_t elem)
{
    psb_node_t *node = malloc(sizeof(psb_node_t));
    if (node) {
        node->elem = elem;
        node->left = NULL;
        node->right = NULL;
    }
    return node;
}

/******************************************************************************
 *  Additional "hidden" functions, for debugging purposes.                    *
 ******************************************************************************/

/* FUNCTION psb_print
 *    Print every value in the subtree rooted at root to stdout, in a "sideways
 *    tree" layout with the root at the given depth.  Print each node's element.
 * Parameters and preconditions:
 *    root != NULL: the root of the subtree to print
 *    depth >= 0: the depth at which to print the root's value
 *    indent > 0: number of spaces to print for each level of depth
 *    print != NULL: the function to use to print each node's value
 * Return value:  none
 * Side-effects:
 *    every value in the subtree rooted at root is printed to stdout, using a
 *    "sideways tree" layout (with right subtrees above and left subtrees below,
 *    and indentation to indicate each value's depth in the tree)
 */
static
void psb_print(const psb_node_t *root, int depth, int indent,
               void (*print)(bag_elem_t))
{
    if (root) {
        psb_print(root->right, depth + 1, indent, print);

        /* Print each value followed by its depth, with INDENT spaces of
         * indentation for each level of depth in the tree. */
        printf("%*s", depth * indent, "");
        (*print)(root->elem);

        psb_print(root->left, depth + 1, indent, print);
    }
}

/* FUNCTION bag_print
 *    Print every value in a bag to stdout, in a "sideways tree" layout.
 * Parameters and preconditions:
 *    bag != NULL: the bag
 *    print != NULL: the function to use to print each value in the bag
 * Return value:  none
 * Side-effects:
 *    every value in the bag is printed to stdout, using a "sideways tree"
 *    layout (with right subtrees above and left subtrees below, and indentation
 *    to indicate each value's depth in the tree)
 */
void bag_print(const bag_t *bag, int indent, void (*print)(bag_elem_t))
{
    psb_print(bag->root, 1, indent, print);
}

