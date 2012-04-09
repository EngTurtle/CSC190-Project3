/* FILE file_util.h
 *    Declarations of types and functions to process text files word-by-word.
 * Author: Francois Pitt, March 2012.
 */
#ifndef FILE_UTIL_H
#define FILE_UTIL_H

/******************************************************************************
 *  Types and Constants.                                                      *
 ******************************************************************************/

#include <stdbool.h> /* for type bool   */
#include <stdio.h>   /* for type FILE   */

/* Constants for pagination. */
#define LINE_LENGTH 80U /* maximum number of characters on one line */
#define PAGE_LENGTH 66U /* maximum number of lines on one page      */

/******************************************************************************
 *  Function declarations -- with full documentation.                         *
 ******************************************************************************/

/* FUNCTION get_word
 *    Read the next word from file f and store it in w; store the page number in
 *    *p.  w must point to a location in memory large enough for at least
 *    LINE_LENGTH characters.  The function keeps track of the file position and
 *    page number internally.
 * Parameters and preconditions:
 *    f: a file already opened for reading, or NULL
 *    w: where to store the next word (must be large enough for at least
 *       LINE_LENGTH many characters)
 *    p: where to store the page number
 * Return value:
 *    true if a new word and its page number were stored in *w and *p;
 *    false otherwise (such as at the end of file)
 * Side-effects:
 *    if f != NULL, the contents of *w and *p are changed and the file pointer
 *    for f is advanced; otherwise, the page number is reset to 0
 */
bool get_word(FILE *f, char *w, unsigned *p);

#endif/*FILE_UTIL_H*/
