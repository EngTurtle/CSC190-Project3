/* FILE index.c
 *    Generate an index of the words (and their page numbers) in a text file.
 * Author: Francois Pitt, March 2012.
 */

////////////////////////////////////////////////////////////////////////////////
//  NOTE:  This file is incomplete!  I have indicated where you need to add   //
//  code using comment blocks formatted like this one.                        //
////////////////////////////////////////////////////////////////////////////////

/******************************************************************************
 *  Constants and types.                                                      *
 ******************************************************************************/

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "file_util.h"
#include "bag.h"

/* CONSTANT MIN_WORD_LEN
 *    Default minimum word length for the index.  Can be overriden by providing
 *    another value on the command line.
 */
#define MIN_WORD_LEN  8

/* TYPE entry_t
 *    The type of one entry in the index.
 */
typedef struct entry {
    ////////////////////////////////////////////////////////////////////////////
    //  Define a type that you will use to store information about words and  //
    //  their page numbers in the index.  This is entirely up to you: think   //
    //  carefully about different ways you could do this, and the advantages  //
    //  and disadvantages of each, and make sure to include a brief comment   //
    //  here to discuss your choice and your reasons for it.                  //
    ////////////////////////////////////////////////////////////////////////////
    char *entry_word;
    bool location[2048];
} entry_t;

/******************************************************************************
 *  Function declarations -- with full documentation.                         *
 ******************************************************************************/

/* FUNCTION main
 *    Grab the name of a text file and an optional minimum word length from the
 *    command line and generate an index of all the words in the text file that
 *    are long enough, along with their page number.  The index is printed to
 *    stdout.
 * Parameters and preconditions:
 *    argc > 0: number of command line arguments
 *    argv != NULL: array of command line arguments
 * Return value:  exit status
 * Side-effects:  the main program is executed
 */
int main(int argc, char *argv[]);

/* FUNCTION generate_index
 *    Create and return an index of every word whose length is at least
 *    min_word_len in file input, along with each word's page numbers.
 * Parameters and preconditions:
 *    input != NULL: a file already opened for reading
 *    min_word_len > 0: the minimum length of words to put in the index
 * Return value:
 *    a bag that contains every word in file input whose length is at least
 *    min_word_length, along with the page numbers where the word appears;
 *    NULL in case of any error with memory allocation
 * Side-effects:
 *    memory is allocated for the bag and the file has been read to the end
 */
static
bag_t *generate_index(FILE *input, int min_word_len);

/* FUNCTION entry_create
 *    Create and return a new index entry given a word and page number.
 * Parameters and preconditions:
 *    word != NULL: pointer to a word
 *    page > 0: a page number
 * Return value:
 *    a new index entry storing a copy of word and its page number;
 *    NULL in case of any error with memory allocation
 * Side-effects:
 *    memory has been allocated for the new entry and to make a copy of word
 */
static
bag_elem_t entry_create(const char *word, unsigned page);

/* FUNCTION entry_destroy
 *    Release the memory allocated for an index entry (passed in as type
 *    bag_elem_t).
 * Parameters and preconditions:
 *    e != NULL: an index entry
 * Return value:  none
 * Side-effects:
 *    the memory allocated for e is freed
 */
static
void entry_destroy(bag_elem_t e);

////////////////////////////////////////////////////////////////////////////////
//  Complete the documentation for the next two functions to provide more     //
//  information about how entries are printed and compared -- the details     //
//  will depend on how you store entries.                                     //
////////////////////////////////////////////////////////////////////////////////

/* FUNCTION entry_print
 *    Print an index entry (passed in as type bag_elem_t) to stdout.
 * Parameters and preconditions:
 *    e != NULL: an index entry
 * Return value:  none
 * Side-effects:
 *    the entry is printed to stdout
 */
static
void entry_print(bag_elem_t e);

/* FUNCTION entry_cmp
 *    Compare two index entries (passed in as type bag_elem_t).
 * Parameters and preconditions:
 *    e1 != NULL: the first entry to compare
 *    e2 != NULL: the second entry to compare
 * Return value:
 *    < 0 if e1 < e2; > 0 if e1 > e2; == 0 if e1 == e2
 * Side-effects:  none
 */
static
int entry_cmp(bag_elem_t e1, bag_elem_t e2);

/******************************************************************************
 *  Function definitions -- see above for documentation.                      *
 ******************************************************************************/

////////////////////////////////////////////////////////////////////////////////
//  You may make changes to main() as necessary to make it work with the way  //
//  you defined type entry_t and the entry-related functions.                 //
////////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
    FILE *input;
    int min_word_len = 0;
    bag_t *index;
    clock_t ticks;

    /* First, check that there is a first command line argument and
     * that it is the name of a file that can be opened for reading. */
    if (argc <= 1 || ! (input = fopen(argv[1], "r"))) {
        fprintf(stderr,
                "ERROR: missing or incorrect argument!\n"
                "USAGE: %s <filename> [minimum_word_length]\n"
                "  . <filename> is the name of a text file (required)\n"
                "  . [minimum_word_length] is a positive integer (optional)\n",
                argv[0]);
        exit(EXIT_FAILURE);
    }
    /* If we get here, the file has been opened for reading. */

    /* Next, check if there is a second command line argument to specify
     * a minimum word length. */
    if (argc < 3 || (min_word_len = (int) strtol(argv[2], NULL, 10)) <= 0)
        min_word_len = MIN_WORD_LEN;
    /* If we get here, the minimum word length has a positive value. */

    /* Next, generate the index, close the input file (because we're done with
     * it at this point), and print timing data. */
    ticks = clock();
    index = generate_index(input, min_word_len);
    ticks = clock() - ticks;
    fclose(input);
    fprintf(stderr, "Elapsed time for generating the index: %gms\n",
                    1000.0 * ticks / CLOCKS_PER_SEC);
    /* Timing data is printed on stderr so we can isolate it from the rest of
     * the output below, if desired. */

    /* Finally, print the index on stdout and clean up: free the memory
     * allocated for each index entry, then the memory for the index itself. */
    if (index) {
        bag_traverse(index, entry_print);
        bag_traverse(index, entry_destroy);
        bag_destroy(index);
    }

    return EXIT_SUCCESS;
    ////////////////////////////////////////////////////////////////////////////
    //  This function is still missing some code to compute the running time  //
    //  of generating and printing the index.  This will be added later.      //
    ////////////////////////////////////////////////////////////////////////////
}

bag_t *generate_index(FILE *input, int min_word_len)
{
    bag_t *index = bag_create(entry_cmp);

    if (index) {
        char word[LINE_LENGTH] = "";
        unsigned page = 0;
        while (get_word(input, word, &page)) {
            ////////////////////////////////////////////////////////////////////
            //  Add code to perform the following tasks:                      //
            //      1.  Compare the length of word to min_word_len.           //
            //      2.  If word is long enough, add an entry in the index     //
            //          for the current page number -- unless the word and    //
            //          page number are already in the index together, in     //
            //          which case no new entry should be added.              //
            ////////////////////////////////////////////////////////////////////

            // if the length of the word is long enough
                // if the word is already in index
                    // add the location to the list of locations for that word
                // if the word isn't in the index
                    // create the entry
                    // add the location
        }
    }

    return index;
}

bag_elem_t entry_create(const char *word, unsigned page)
{
    entry_t *new_entry = malloc(sizeof(entry_t));
    new_entry -> entry_word = word;
    new_entry -> location[page] = true;
    return new_entry;
    ////////////////////////////////////////////////////////////////////////////
    //  Write code for this function.                                         //
    //  WARNING!  This function must make a _COPY_ of the string that it is   //
    //  given in argument 'word', i.e., it is necessary to allocate memory    //
    //  and to copy the characters from word[].                               //
    ////////////////////////////////////////////////////////////////////////////
}

void entry_destroy(bag_elem_t e)
{
    entry_t *old_entry = e;
    free(old_entry -> entry_word);
    free(old_entry);
    ////////////////////////////////////////////////////////////////////////////
    //  Write code for this function.  Don't forget to free _ALL_ of the      //
    //  memory that was allocated for e, including memory to store the word.  //
    ////////////////////////////////////////////////////////////////////////////
}

void entry_print(bag_elem_t e)
{
    ////////////////////////////////////////////////////////////////////////////
    //  Write code for this function.  How to print an individual entry will  //
    //  depend completely on how you defined type entry_t, but the final      //
    //  output of printing every entry should follow the specification given  //
    //  in the handout.                                                       //
    ////////////////////////////////////////////////////////////////////////////
}

int entry_cmp(bag_elem_t e1, bag_elem_t e2)
{
    ////////////////////////////////////////////////////////////////////////////
    //  Write code for this function.  Again, how you do this will depend on  //
    //  how you defined type entry_t.                                         //
    ////////////////////////////////////////////////////////////////////////////
}
