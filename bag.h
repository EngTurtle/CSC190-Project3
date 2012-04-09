/* FILE bag.h
 *    Declarations of types and functions to work with "bags" -- collections of
 *    elements with duplicate values allowed.
 * Author: Francois Pitt, March 2012.
 */
#ifndef BAG_H
#define BAG_H

/******************************************************************************
 *  Types and Constants.                                                      *
 ******************************************************************************/

#include <stdlib.h>  /* for type size_t */

/* TYPE bag_elem_t -- The type of elements in bags. */
typedef const void *bag_elem_t;

/* TYPE bag_t -- The type of a bag. */
typedef struct bag bag_t;

/******************************************************************************
 *  Functions, with full documentation.                                       *
 ******************************************************************************/

/* FUNCTION bag_create
 *    Create a new empty bag.
 * Parameters and preconditions:
 *    cmp != NULL: pointer to a function for comparing elements -- cmp(e1, e2)
 *          < 0 if e1 < e2; > 0 if e1 > e2; == 0 if e1 == e2
 * Return value:
 *    pointer to a newly-created empty bag;
 *    NULL in case of error with memory allocation
 * Side-effects:
 *    memory has been allocated for the new bag
 */
bag_t *bag_create(int (*cmp)(bag_elem_t, bag_elem_t));

/* FUNCTION bag_destroy
 *    Free all the memory allocated for a bag.
 * Parameters and preconditions:
 *    b != NULL: a bag
 * Return value:  none
 * Side-effects:
 *    all memory allocated for b has been freed
 */
void bag_destroy(bag_t *b);

/* FUNCTION bag_size
 *    Return the size of a bag.
 * Parameters and preconditions:
 *    b != NULL: a bag
 * Return value:
 *    the number of elements in b
 * Side-effects:  none
 */
size_t bag_size(const bag_t *b);

/* FUNCTION bag_traverse
 *    Call a function on every element in a bag, in order.
 * Parameters and preconditions:
 *    b != NULL: a bag
 *    f != NULL: a pointer to a function to apply to each element in the bag
 * Return value:  none
 * Side-effect:
 *    function f has been called on each element in the bag, in order
 */
void bag_traverse(const bag_t *b, void (*f)(bag_elem_t));

/* FUNCTION bag_contains
 *    Return whether or not a bag contains a certain element.
 * Parameters and preconditions:
 *    b != NULL: a bag
 *    e != NULL: an element
 * Return value:
 *    e, if it belongs to b; NULL otherwise
 * Side-effects:  none
 */
bag_elem_t bag_contains(bag_t *b, bag_elem_t e);

/* FUNCTION bag_insert
 *    Add an element to a bag.
 * Parameters and preconditions:
 *    b != NULL: a bag
 *    e != NULL: an element
 * Return value:
 *    e, if it was added to b; NULL in case of error during the insertion
 * Side-effects:
 *    e has been added to b, except possibly in case of error
 */
bag_elem_t bag_insert(bag_t *b, bag_elem_t e);

/* FUNCTION bag_remove
 *    Remove an element from a bag.
 * Parameters and preconditions:
 *    b != NULL: a bag
 *    e != NULL: an element
 * Return value:
 *    e, if it was removed from b; NULL in case of error during the removal
 * Side-effects:
 *    e has been removed from b, except possibly in case of error
 */
bag_elem_t bag_remove(bag_t *b, bag_elem_t e);

#endif/*_BAG_H*/
