// Filename: input.h (for input.c)
// Description: Contains prototypes for functions for checking any input such as the arguments, 
//              as well as the input to to other functions.

#ifndef _INPUT_
#define _INPUT_

// input logic processing
int checkArguments(int argc, char** argv);

// parses the filepath given a directory name and file name
char* getFilePath(char* dirname, int file);

// prunes the html document from any new line characters
void removeNewLines(char* html);

// converts any non alpha character to a space
void NormalizeString(char* string);

// changes all words to lower case
void NormalizeWord(char* word);

// appends a single character to a string
void append(char* s, char c);

#endif