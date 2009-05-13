// Filename: util.h (for util.c)
// Description: Contains prototypes for functions function in util.c

#ifndef _UTIL_
#define _UTIL_

// mallocs and BZERO's the slots in the list of defined length
void prepList(char** list, int length);

// goes through a list and for slots length it free's and sets the slot to NULL
void cleanList(char** list, int length);

void zeroList(char** list, int length);

// converts the input string into a list of words
char** formList(char** word_list, char* input);

// visits the files referenced by the items in the lists and retrieves and prints the URL for each
void printMeta(char *dirname, char** result);

// returns the length of the array
int getLength(char** list);

// returns the maximum of the two numbers
int getMax(int one, int two);

#endif