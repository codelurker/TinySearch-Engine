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

// inserts a page (DocumentNode) into the index
int updateIndex(char* word, int documentId, INVERTED_INDEX* index) {
	int hashIndex = hash1(word)%MAX_HASH_SLOT;
	
	// something is in there already
	if ((index->hash[hashIndex]) != NULL) {
		DocumentNode* init;
		init = (index->hash[hashIndex])->page;
		// if the document ID's match
		if(init->document_id == documentId) {
			init->page_word_frequency += 1;
			(index->hash[hashIndex])->page = init;
		}
		// if the document ID's don't match
		else {
			DocumentNode* d;
			
			if(index->hash[hashIndex]) {
				int flag = 0;
				
				d = (index->hash[hashIndex])->page;
				while ((d->next) != NULL) {
					d = d->next;
					if(d->document_id == documentId) {
						//we found the same documentId
						d->page_word_frequency += 1;
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
					dd->page_word_frequency = 1;
					
					if (d->next == NULL) {
						d->next = dd;
					}
					else {
						free(dd);
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
		doc->page_word_frequency = 1;
		doc->next = NULL;
		// add documentNode into the index
		IndexAdd(index, (void*)doc, word);
		return 1;	
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
					dd->next = NULL;
					dd->document_id = documentId;
					dd->page_word_frequency = frequency;
					
					if (d->next == NULL) {
						d->next = dd;
					}
					else {
						free(dd);
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

// function to reload an index from a file, re-create the index structure for it, and output it to a new file
void reloadIndex(char* dirname, char* filename, INVERTED_INDEX* index) {
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
	// save the re-created index into a new file
	saveIndexToFile(dirname, "index_R.dat", index);
	
	// clean the index and set it to null
	CleanIndex(index);
	free(index);
	index = NULL;
	
	fclose(indexFile);
	free(buffer);
}

// saves/writes the index to the specified file at the prompt
void saveIndexToFile(char* dirname, char* filename, INVERTED_INDEX* index) {
	// computer full path from the directory name and file name
	// char path[strlen(dirname)+strlen(filename)+1];
	char* path = (char*)malloc(sizeof(char)*1000);
	BZERO(path, 1000);
	strcpy(path, dirname); strcat(path, "/"); strcat(path, filename);
		
	FILE* fp; fp = fopen(path, "w");
	
	if (fp == NULL) {
		printf("Error creating file [%s].\n", path);
		free(path);
		exit(1);
	}
		
	WordNode* n = index->start;
	while (n != NULL) {
	  int len = 0;
	  DocumentNode* p = (DocumentNode*)n->page;
	  while (p != NULL) {
	    len++; p = p->next;
	  }
	  fprintf(fp, "%s %d ", n->word, len);  
	  for (p = (DocumentNode*)n->page; p != NULL; p = p->next)
	    fprintf(fp, "%d %d ", p->document_id, p->page_word_frequency);
	  fprintf(fp, "\n");
	  n = n->next;
	}
	// string to build up our sort command
	char command[1000] = "sort -o "; strcat(command, path); append(command, ' '); strcat(command, path);
	fclose(fp); 
	system(command);
	free(path);
	path = NULL;
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

// gets the next word from html document
int getNextWordFromHTMLDocument (char* html, char* word, int pos, INVERTED_INDEX* index, int documentId) {
		
	int len=0; int count=0;
	char c; char* p1; char* p2;
	
	char copy[strlen(html)]; strcpy(copy, html);
	// while end of file hasn't been reached
	while((c = html[pos]) != 0) {
		// if open tag is found, skip position, and keep track of open characters
		if((c=='<')) {
			count++;
			pos++;
		}
		// if close tag is found, keep track of closed characters
		else if((c=='>')) {
			count--;
			// if count is zero, we can be certain that we are not inside a nested set of tags
			if (count == 0) {
				int last = pos;
				
				// position to keep track of last closed tag
				p1 = &(html[pos+1]);
				
				// while we don't encounter an open tag
				while((html[pos+1] != '<') && (html[pos+1] != '\0')) {
					pos++;
					len++;
				}			
				if (len < 1) {
					return(last+1);
				}
				else {
					// record the position
					p2 = &html[pos+1];
					
					// copy things in between tags
					char temp[(p2 - p1)+1];
					BZERO(temp, (p2 - p1)+1);
					strncpy(temp, p1, (p2 - p1));
					
					// makes all uppers lowers
					NormalizeString(temp);
					
					// strotk on the string to get the first word					
					word = strtok(temp, " ");
					
					// if strtok gets a word
					if (word != NULL) {
						NormalizeWord(word);
						updateIndex(word, documentId, index);				
					}
					// keep strtoking
					while((word = strtok(NULL, " "))) {
						NormalizeWord(word);
						// printf("\nThe NEW word is: %s", word);
						updateIndex(word, documentId, index);
					}
					return(pos+1);
				}
				
				break;
			}
			// if not outside of nested pair of tags, then keep going
			else {
				count--;
				pos++;
				continue;
			}
		}
		// if not tag found, keep skipping
		else {
			pos++;
		}
	}
	return(pos);
}
