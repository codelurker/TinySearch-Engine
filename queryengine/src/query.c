/*
	By: Ricky A. Melgares '11, April 30, 2009
	
	FILE: indexer.c
	
	Description:
	
	Inputs: ./indexer [TARGET\_DIRECTORY WHERE TO PUT THE DATA] [RESULTS FILE NAME]
	
	Outputs: For all the files created by the crawler program, indexer will gennerate an
	inverted index (called index.dat) containing the occurences of words found in the
	files, along with the frequency and identifiers representing the names of the files
	in which the word was found.
*/

/*** IMPLEMENTATION SPEC *** - PSEUDOCODE

1. Load the previously generated index from the file system.
2. Prompt for and receive user queries from input.
3. Convert all capital lettes of words of query to lower case.
4. For each query, check the index by retrieving results matching
   the query, returning a non-ranked list of results.
5. Rank the list of resutls according to a simple algorithm based on the
   frequency of occurences of each word and the use of AND and OR operators.
6. Display the results to the user.

*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/dir.h>
#include <sys/param.h>
#include <ctype.h>
#include <curses.h>

#include "../util/input.h"
#include "../util/util.h"
#include "../../shared/hash.h"
#include "dictionary.h"
#include "logic.h"

INVERTED_INDEX* index = NULL;

// initializes all data structures to be used
int initList() {
  index = InitIndex();
  return (index == NULL)? 0:1; 
}

// cleans up the index
void cleanup() {
	CleanIndex(index);
	index = NULL;
}

// list to store temporary elements int
char *doc_list[1000];

int main(int argc, char** argv) {
	
	if (checkArguments(argc, argv) != 1) {
	exit(1);
	}

	if (initList() != 1) {
	printf("Initialization wrong!");
	exit(1);
	}
	
	// load index
	index = reloadIndex(argv[2], argv[1], index);
	LOGSTATUS("Index has been recreated and stored.");

	// main loop, ask for input and process it
	while(1) {
		printf("\nInput to tinysearch - type keywords:\n\n");
		char input[1000];
	
		fgets(input, 999, stdin);
		NormalizeWord(input);
		removeNewLines(input);
		
		char* args[1000];
		formList(args, input);
		
		int length = getLength(args);
		
		checkArgs(length, argv[2], args, index);
		cleanList(args, length);
	}
	
	return 0;
}