/*
	By: Ricky A. Melgares '11
	
	Filename: dictionary.c
	Description: contains various functions that relate to the dictionary 
	structure such as adding elements or retrieving information from 
	elements in the structure.

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

#include "dictionary.h"
#include "../util/input.h"
#include "../../shared/hash.h"

// initializes the index
INVERTED_INDEX* InitIndex() {
	INVERTED_INDEX* index = (INVERTED_INDEX*)malloc(sizeof(INVERTED_INDEX));
	MALLOC_CHECK(index);
	index->start = index->end = NULL;
	
	for (int i = 0; i< MAX_HASH_SLOT; i++) {
		index->hash[i] = NULL;
	}
	return index;
	free(index);
}

// insert a page object into the index
void IndexAdd(INVERTED_INDEX* index, void* page, char* word) {
	
	MYASSERT(strlen(word) > 0);
	int h = hash1(word)%MAX_HASH_SLOT;
	
	if (index->start == NULL) {
		// first element of the index
     	MYASSERT(index->end == NULL);
     	index->start = index->end = (WordNode*)malloc(sizeof(WordNode));
     	MALLOC_CHECK(index->start);
     	index->start->prev = index->start->next = NULL; 
     	index->hash[h] = index->start;
     	index->start->page = page;
     	BZERO(index->start->word, WORD_LENGTH);
     	strncpy(index->start->word, word, WORD_LENGTH);
	} 
	else {
		WordNode* d;
     	// not the first element of the index
     	if (index->hash[h]) {
       		// same hash
       		int flag = 0;
       		// check to see if word is already present
       		for (d = index->hash[h]; (d!=NULL) && ((hash1(d->word)%MAX_HASH_SLOT) == h); d = d->next) {
         		if (!strncmp(d->word, word, WORD_LENGTH)) {
           			// find same word
           			if (d->page != NULL)
             			free(d->page);
           			d->page = page;
           			flag = 1;
           			break;
         		}        
       		}
       		// not present
       		if (flag == 0) {
         		// did not find same word
         		if (d == NULL)
           			d = index->end;
         		else
           			d = d->prev;
		         WordNode* dd = (WordNode*)malloc(sizeof(WordNode));
		         MALLOC_CHECK(dd);
		         dd->next = d->next;
		         dd->prev = d;
		         if (dd->next == NULL)
		           index->end = dd;
		         d->next = dd;
		         BZERO(dd->word, WORD_LENGTH);
		         strncpy(dd->word, word, WORD_LENGTH);
		         dd->page = page;
       		}
      	} 
		else {
	       //! No same hash, create at the end of the list.
	       WordNode* d = (WordNode*)malloc(sizeof(WordNode));
	       d->next = NULL;
	       d->prev = index->end;
	       index->end->next = d;
	       //! add to our cash slot
	       index->hash[h] = d;
	       d->page = page;
	       BZERO(d->word, WORD_LENGTH);
	       strncpy(d->word, word, WORD_LENGTH);
	       //! change the end of our list
	       index->end = d;
  	    }
	}
}

// tweaked version of update index to insert an element into the index along with with the frequency
int insertElementIntoIndex(char* word, int documentId, int frequency, INVERTED_INDEX* index) {
	int hashIndex = hash1(word)%MAX_HASH_SLOT;
	// something is in there already
	if ((index->hash[hashIndex]) != NULL) {
		DocumentNode* init;
		init = (index->hash[hashIndex])->page;
		
		if(init->document_id == documentId) {
			init->page_word_frequency = frequency;
			(index->hash[hashIndex])->page = init;
		}
		else {
			DocumentNode* d;
			
			if(index->hash[hashIndex]) {
				int flag = 0;
				
				d = (index->hash[hashIndex])->page;
				while ((d->next) != NULL) {
					d = d->next;
					if(d->document_id == documentId) {
						//we found the same documentId
						d->page_word_frequency = frequency;
						flag = 1;
						break;
					}
				}
				
				if (flag == 0) {
					// documentId not present
					DocumentNode* dd = (DocumentNode*)malloc(sizeof(DocumentNode));
					MALLOC_CHECK(dd);
					dd->next = NULL;
					dd->document_id = documentId;
					dd->page_word_frequency = frequency;
					
					if (d->next == NULL) {
						d->next = dd;
					}
				}
			}
		}
		
		return 1;
	}
	// nothing is in there already
	else {		
		DocumentNode* doc = malloc(sizeof(DocumentNode));
		MALLOC_CHECK(doc);
		doc->document_id = documentId;
		doc->page_word_frequency = frequency;
		doc->next = NULL;
		
		// add element into the index
		IndexAdd(index, (void*)doc, word);
		return 1;	
	}
}

// inserts a page (DocumentNode) into the index
int findWordData(char* word, char** list, INVERTED_INDEX* index) {
	int hashIndex = hash1(word)%MAX_HASH_SLOT;
	int pos = 0;
	
	// something is in there already
	if (((index->hash[hashIndex]) != NULL) && (!strcmp(word,(index->hash[hashIndex])->word))) {
		WordNode* node = index->hash[hashIndex];
		// printf("\nThe following word is present: %s\n", word);
		DocumentNode* d;
		d = node->page;
		
		while (d != NULL) {
			list[pos] = (char*)malloc(sizeof(char)*1000);
			BZERO(list[pos], 1000);
			
			int id = d->document_id;
			int freq = d->page_word_frequency;
			
			sprintf(list[pos], "%d %d ", id, freq);
			// printf("%d: %s\n", pos, list[pos]);
			d = d->next;
			pos++;
		}
		return(1);
	}
	// nothing is in there already
	else {		
		printf("\nNo results found for the following word: %s\n", word);
		return(0);
	}
}

// function to reload an index from a file, re-create the index structure for it, and output it to a new file
INVERTED_INDEX* reloadIndex(char* dirname, char* filename, INVERTED_INDEX* index) {
	index = InitIndex();
	
	// strings to build up the full path
	char* path;
	path = (char*)malloc(sizeof(char)*1000);
	BZERO(path, 1000);
	strcpy(path, dirname); strcat(path, "/"); strcat(path, filename);
		
	FILE* indexFile; indexFile = fopen(path, "r");
	
	if (indexFile == NULL) {
		printf("Error opening [%s] to RELOAD.\n", path);
	}
	char* buffer;
	buffer = (char*)malloc(sizeof(char)*20000);
	BZERO(buffer, 20000);
	
	// get each line from the index file and process it
	while(1) {
		if(fgets(buffer, 20000, indexFile) == NULL) {
			break;
		}
		else {
			removeNewLines(buffer);
						
			// variables to store parsed elements: word, # of docs, pairs of docID's and frequency
			char temp[20000]; 
			strcpy(temp, buffer);
			// free(buffer);
			
			char* word; word = strtok(temp, " ");
			char* docs; docs = strtok(NULL, "  ");
			
			char* doc;
			
			// while the end of the line hasn't been reached, keep str'toking			
			while((doc = strtok(NULL, " ")) != NULL) {
				char* freq;
				// parse the frequency
				freq = strtok(NULL, " ");
				// insert word, along with docID and frequency into the index
				insertElementIntoIndex(word, atoi(doc), atoi(freq), index);
			}
		}
	}
	fclose(indexFile);
	return(index);
}

// cleans the index by freeing up all the WordNode's
void CleanIndex(INVERTED_INDEX* index) {
	WordNode* d = index->start;
	while (d != NULL) {
		WordNode* wd = d;
		if (wd->page != NULL)
			free(wd->page);
		d = d->next;
		free(wd);
	}
}