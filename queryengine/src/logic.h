/*

	By: Ricky A. Melgares '11
	
	Filename: logic.h
	Description: Contains prototypes for the logic.c file.

*/


#ifndef _LOGIC__
#define _LOGIC__

// parses the string entered by the user in the query engine
void parseInput(int length, int pos, char* dirname, char** args, INVERTED_INDEX* index);

// processes the input string and performs logic based on the input and reserved words
int checkArgs(int length, char*dirname, char** args, INVERTED_INDEX* index);

// gets the intersection of the two lists
void intersection(char** list1, char** list2, char** result);

// returns the union of the two lists
void unionOf(char** list1, char** list2, char** result);

#endif
