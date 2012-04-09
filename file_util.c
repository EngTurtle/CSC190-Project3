/* FILE file_util.c
 *    Implementation of the file_util functions.
 * Author: Francois Pitt, March 2012.
 */

/******************************************************************************
 *  Types and Constants.                                                      *
 ******************************************************************************/

#include <string.h>
#include <ctype.h>

#include "file_util.h"

/******************************************************************************
 *  Function definitions -- see header file for documentation.                *
 ******************************************************************************/

bool get_word(FILE *file, char *word, unsigned *page)
{
    static char line[LINE_LENGTH + 2] = ""; /* last line read from file */
    static char *next_word = line; /* first character of the next word */
    static unsigned line_no = 0; /* number of the last line read */
    
    /* Reset the line and character information if file == NULL. */
    if (! file) {
        line[0] = '\0';
        next_word = line;
        line_no = 0;
        return false;
    }
    
    /* Check if we need to read another line from file.  Based on the fact that
     * line[] contains a null character to mark the end of the line. */
    if (! *next_word) {
        if (! fgets(line, LINE_LENGTH + 2, file))  return false;
        /* Reset the next_word pointer and increment the line number. */
        next_word = line;
        ++line_no;
    }
    
    /* Set the next word and mark its end in the input array. */
    while (isalnum(*next_word))  *word++ = *next_word++;
    *word++ = *next_word++ = '\0';
    
    /* Set the page number. */
    *page = 1U + line_no / PAGE_LENGTH;
    
    return true;
}
