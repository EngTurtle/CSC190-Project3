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
 * Side-effects:  none
 */
static
bag_elem_t psb_contains(const psb_node_t *root, bag_elem_t elem,
                        int (*cmp)(bag_elem_t, bag_elem_t));

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





//DELETEDELETEDELETEDELETEDELETEDELETEDELETEDELETEDELETEDELETEDELETEDELETEDELETEDELETEDELETEDELETEDELETEDELETEDELETEDELETEDELETEDELETE
/* FUNCTION psb_rebalance_to_the_left
 *    Rebalance the subtree rooted at *root, given that its right subtree is too
 *    tall -- this involves performing either a single or a double rotation.
 * Parameters and preconditions:
 *    root != NULL: a pointer to the root of the tree to rebalance
 *                  (*root != NULL and (*root)->right != NULL)
 * Return value:  none
 * Side-effects:
 *    the subtree rooted at *root has been rebalanced, and the heights of each
 *    node involved have been updated appropriately
 */
static
void psb_rebalance_to_the_left(psb_node_t **root);

/* FUNCTION psb_rebalance_to_the_right
 *    Rebalance the subtree rooted at *root, given that its left subtree is too
 *    tall -- this involves performing either a single or a double rotation.
 * Parameters and preconditions:
 *    root != NULL: a pointer to the root of the tree to rebalance
 *                  (*root != NULL and (*root)->left != NULL)
 * Return value:  none
 * Side-effects:
 *    the subtree rooted at *root has been rebalanced, and the heights of each
 *    node involved have been updated appropriately
 */
static
void psb_rebalance_to_the_right(psb_node_t **root);
//DELETEDELETEDELETEDELETEDELETEDELETEDELETEDELETEDELETEDELETEDELETEDELETEDELETEDELETEDELETEDELETEDELETEDELETEDELETEDELETEDELETEDELETE





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







//DELETEDELETEDELETEDELETEDELETEDELETEDELETEDELETEDELETEDELETEDELETEDELETEDELETEDELETEDELETEDELETEDELETEDELETEDELETEDELETEDELETEDELETE
/* FUNCTION psb_update_height
 *    Update the height of a node (based on the heights of its children).
 * Parameters and preconditions:
 *    node != NULL: the node to update
 * Return value:  none
 * Side-effects:
 *    the height of node is updated
 */
static
void psb_update_height(psb_node_t *node);
//DELETEDELETEDELETEDELETEDELETEDELETEDELETEDELETEDELETEDELETEDELETEDELETEDELETEDELETEDELETEDELETEDELETEDELETEDELETEDELETEDELETEDELETE







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
    return psb_contains(bag->root, elem, bag->cmp);
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
                        int (*cmp)(bag_elem_t, bag_elem_t))
{
    if (! root)
        return NULL;
    else if ((*cmp)(elem, root->elem) < 0)
        return psb_contains(root->left, elem, cmp);
    else if ((*cmp)(elem, root->elem) > 0)
        return psb_contains(root->right, elem, cmp);
    else /* ((*cmp)(elem, root->elem) == 0) */
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
        if ((inserted = psb_insert(&(*root)->left, elem, cmp))) {
            /* Check if the subtree needs rebalancing; update its height. */
            if (HEIGHT((*root)->left) > HEIGHT((*root)->right) + 1)
                psb_rebalance_to_the_right(root);
            else
                psb_update_height(*root);
        }
    } else if ((*cmp)(elem, (*root)->elem) > 0) {
        if ((inserted = psb_insert(&(*root)->right, elem, cmp))) {
            /* Check if the subtree needs rebalancing; update its height. */
            if (HEIGHT((*root)->right) > HEIGHT((*root)->left) + 1)
                psb_rebalance_to_the_left(root);
            else
                psb_update_height(*root);
        }
    } else { /* ((*cmp)(elem, (*root)->elem) == 0) */
        /* Insert into the subtree with smaller height. */
        if (HEIGHT((*root)->left) < HEIGHT((*root)->right))
            inserted = psb_insert(&(*root)->left, elem, cmp);
        else
            inserted = psb_insert(&(*root)->right, elem, cmp);
        /* No rebalancing necessary, but update height. */
        if (inserted)  psb_update_height(*root);
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
        if ((removed = psb_remove(&(*root)->left, elem, cmp))) {
            /* Check if the subtree needs rebalancing; update its height. */
            if (HEIGHT((*root)->left) + 1 < HEIGHT((*root)->right))
                psb_rebalance_to_the_left(root);
            else
                psb_update_height(*root);
        }
    } else if ((*cmp)(elem, (*root)->elem) > 0) {
        if ((removed = psb_remove(&(*root)->right, elem, cmp))) {
            /* Check if the subtree needs rebalancing; update its height. */
            if (HEIGHT((*root)->right) + 1 < HEIGHT((*root)->left))
                psb_rebalance_to_the_right(root);
            else
                psb_update_height(*root);
        }
    } else { /* ((*cmp)(elem, (*root)->elem) == 0) */
        removed = (*root)->elem;
        if ((*root)->left && (*root)->right) {
            /* Remove from the subtree with larger height. */
            if (HEIGHT((*root)->left) > HEIGHT((*root)->right))
                (*root)->elem = psb_remove_max(&(*root)->left);
            else
                (*root)->elem = psb_remove_min(&(*root)->right);
            /* No rebalancing necessary, but update height. */
            psb_update_height(*root);
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
        /* *root is not the minimum, keep going and rebalance if necessary. */
        min = psb_remove_min(&(*root)->left);
        if (HEIGHT((*root)->left) + 1 < HEIGHT((*root)->right))
            psb_rebalance_to_the_left(root);
        else
            psb_update_height(*root);
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
        if (HEIGHT((*root)->right) + 1 < HEIGHT((*root)->left))
            psb_rebalance_to_the_right(root);
        else
            psb_update_height(*root);
    } else {
        /* Remove *root. */
        psb_node_t *old = *root;
        max = (*root)->elem;
        *root = (*root)->left;
        free(old);
    }

    return max;
}

void psb_rebalance_to_the_left(psb_node_t **root)
{
    if (HEIGHT((*root)->right->left) > HEIGHT((*root)->right->right))
        psb_rotate_to_the_right(&(*root)->right);
    psb_rotate_to_the_left(root);
}

void psb_rebalance_to_the_right(psb_node_t **root)
{
    if (HEIGHT((*root)->left->right) > HEIGHT((*root)->left->left))
        psb_rotate_to_the_left(&(*root)->left);
    psb_rotate_to_the_right(root);
}

void psb_rotate_to_the_left(psb_node_t **parent)
{
    /* Rearrange pointers. */
    psb_node_t *child = (*parent)->right;
    (*parent)->right = child->left;
    child->left = *parent;
    *parent = child;

    /* Update heights. */
    psb_update_height(child->left);
    psb_update_height(child);
}

void psb_rotate_to_the_right(psb_node_t **parent)
{
    /* Rearrange pointers. */
    psb_node_t *child = (*parent)->left;
    (*parent)->left = child->right;
    child->right = *parent;
    *parent = child;

    /* Update heights. */
    psb_update_height(child->right);
    psb_update_height(child);
}

void psb_update_height(psb_node_t *node)
{
    node->height = 1 + ( HEIGHT(node->left) > HEIGHT(node->right) ?
                         HEIGHT(node->left) : HEIGHT(node->right) );
}

psb_node_t *psb_node_create(bag_elem_t elem)
{
    psb_node_t *node = malloc(sizeof(psb_node_t));
    if (node) {
        node->elem = elem;
        node->height = 1;
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
 *    tree" layout with the root at the given depth.  Print each node's element
 *    and height.
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
        printf(" [%u]\n", root->height);

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

