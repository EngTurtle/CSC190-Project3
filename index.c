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
 *    The type of one word in the word index.
 */
typedef struct entry
{
    char  *entry_word;
    bag_t *page_index;
} entry_t;

/* Type page entry
 *    The type of one page in the page index.
 */
typedef int page_entry;

// A number to keep track of the number of pages printed in a line so
// the formating is correct.
size_t i = 0;

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

/* FUNCTION page_destroy
 *    Release the memory allocated for an page index entry (passed in as type
 *    bag_elem_t).
 * Parameters and preconditions:
 *    e != NULL: an index entry
 *    e is a pointer to the type page_entry
 * Return value:  none
 * Side-effects:
 *    the memory allocated for e is freed
 */
static
void page_destroy(bag_elem_t e);

/* FUNCTION entry_destroy
 *    Release the memory allocated for an word index entry (passed in as type
 *    bag_elem_t).
 * Parameters and preconditions:
 *    e != NULL: an index entry
 *    e is a pointer to the type entry_t
 * Return value:  none
 * Side-effects:
 *    the memory allocated for e is freed
 */
static
void entry_destroy(bag_elem_t e);

/* Function page_print
 *    print an page index entry (passed in as type bag_elem_t) to stdout.
 * Parameters and preconditions:
 *    e != NULL: an word index entry
 *    e is a pointer to the page_entry type.
 * Return value:  none
 * Side-effects:
 *    the entry is printed to stdout
 */
static
void page_print(bag_elem_t e);

/* FUNCTION entry_print
 *    Print an word index entry (passed in as type bag_elem_t) to stdout.
 * Parameters and preconditions:
 *    e != NULL: an word index entry
 *    e is a pointer to the entry_t type.
 * Return value:  none
 * Side-effects:
 *    the entry is printed to stdout
 */
static
void entry_print(bag_elem_t e);

/* FUNCTION entry_cmp
 *    Compare two word index entries (passed in as type bag_elem_t).
 * Parameters and preconditions:
 *    e1 != NULL: the first entry to compare
 *    e2 != NULL: the second entry to compare
 * Return value:
 *    < 0 if e1 < e2; > 0 if e1 > e2; == 0 if e1 == e2
 * Side-effects:  none
 */
static
int entry_cmp(bag_elem_t e1, bag_elem_t e2);

/* Function entry_add
 *      add the page number to the entry
 * Parameters and preconditions:
 *      entry != NULL: a point the the word entry to be modified
 *      page > 0: a page number to be added to the entry
 * Side-effects: none
 */
static
void entry_add(bag_elem_t *element, unsigned page);

/* Function page_cmp
 *    Compare two page entries (passed in as type bag_elem_t).
 * Parameters and preconditions:
 *    e1 != NULL: the first page to compare
 *    e2 != NULL: the second page to compare
 *    e1 and e2 pointers to the page_entry type
 * Return value:
 *    < 0 if e1 < e2; > 0 if e1 > e2; == 0 if e1 == e2
 * Side-effects:  none
 */
static
int page_cmp(bag_elem_t e1, bag_elem_t e2);

/******************************************************************************
 *  Function definitions -- see above for documentation.                      *
 ******************************************************************************/

int main(int argc, char *argv[])
{
    FILE *input, *log;
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

    //creat or append to a runtime log file
    log = fopen("runtime_log.txt", "a");
    fprintf(log, "For %s and word %d characters and larger:\n", argv[1], min_word_len);

    /* Next, generate the index, close the input file (because we're done with
     * it at this point), and print timing data. */
    ticks = clock();
    index = generate_index(input, min_word_len);
    ticks = clock() - ticks;
    fclose(input);
    fprintf(log, "Elapsed time for generating the index: %gms\n",
                    1000.0 * ticks / CLOCKS_PER_SEC);
    /* Timing data is printed on stderr so we can isolate it from the rest of
     * the output below, if desired. */

    /* Finally, print the index on stdout and clean up: free the memory
     * allocated for each index entry, then the memory for the index itself. */
    if (index) {

        // timing how long it takes to print the index
        ticks = clock();
        bag_traverse(index, entry_print);
        ticks = clock() - ticks;
        fprintf(log, "Elapsed time for printing the index: %gms\n",
                        1000.0 * ticks / CLOCKS_PER_SEC);

        // timing how long it takes to destroy the index
        ticks = clock();
        bag_traverse(index, entry_destroy);
        bag_destroy(index);
        ticks = clock() - ticks;
        fprintf(log, "Elapsed time for destroy the index: %gms\n\n",
                        1000.0 * ticks / CLOCKS_PER_SEC);
    }

    fclose(log);

    return EXIT_SUCCESS;
}

bag_t *generate_index(FILE *input, int min_word_len)
{
    bag_t *index = bag_create(entry_cmp);

    if (index) {
        char word[LINE_LENGTH] = "";
        entry_t new_word, *existing_entry;
        unsigned page = 0;
        while (get_word(input, word, &page))
        {
            new_word.entry_word = word;
            // check if the length of the word is long enough
            if(strlen(word) >= min_word_len)
            {
                existing_entry = bag_contains(index, &new_word);
                // if the word is already in index
                if(existing_entry != NULL)
                {
                    entry_add(existing_entry, page); // add the location to the list of locations for that word
                }
                // if the word isn't in the index
                else
                {
                    bag_elem_t new_entry = entry_create(word, page); // create the entry
                    bag_insert(index, new_entry); // add the location
                }
            }
        }
    }

    return index;
}

bag_elem_t entry_create(const char *word, unsigned page)
{
    entry_t *new_entry = malloc(sizeof(entry_t));

    new_entry -> entry_word = malloc((strlen(word) + 1) * sizeof(char));
    strcpy(new_entry -> entry_word, word);

    new_entry->page_index = bag_create(page_cmp);

    page_entry *new_page = malloc(sizeof(page_entry));
    *new_page = page;

    bag_insert(new_entry->page_index, new_page);
    return new_entry;
    ////////////////////////////////////////////////////////////////////////////
    //  Write code for this function.                                         //
    //  WARNING!  This function must make a _COPY_ of the string that it is   //
    //  given in argument 'word', i.e., it is necessary to allocate memory    //
    //  and to copy the characters from word[].                               //
    ////////////////////////////////////////////////////////////////////////////
}

void page_destroy(bag_elem_t e)
{
    free(e);
}

void entry_destroy(bag_elem_t e)
{
    entry_t *old_entry = e;
    free(old_entry -> entry_word);

    // empty and free the page index
    bag_traverse(old_entry->page_index, page_destroy);
    bag_destroy(old_entry->page_index);

    free(old_entry);
}

void page_print(bag_elem_t e)
{
    page_entry *page = e;
    fprintf(stdout, "%d", *page);

    // only print the comma and space if it's not the last
    // page in the list.
    if(i != 1)
    {
        fprintf(stdout, ", ");
        i--;
    }
}

void entry_print(bag_elem_t e)
{
    // Print the word
    entry_t *this_entry = e;
    fprintf(stdout, "%s: ", this_entry -> entry_word);

    // Print the page index for that word
    i = bag_size(this_entry->page_index);
    bag_traverse(this_entry->page_index, page_print);

    fprintf(stdout,"\n");
}

int entry_cmp(bag_elem_t e1, bag_elem_t e2)
{
    entry_t *entry1 = e1, *entry2 = e2;
    return strcmp(entry1->entry_word, entry2->entry_word);
}

void entry_add(bag_elem_t *element, unsigned page)
{
    entry_t *mod = element;

    page_entry *new_page = malloc(sizeof(page_entry));
    *new_page = page;

    // check if page is already in the index
    if(!bag_contains(mod->page_index, new_page))
    bag_insert(mod->page_index, new_page);
}

int page_cmp(bag_elem_t e1, bag_elem_t e2)
{
    return *(page_entry*)e1 - *(page_entry*)e2;
}
