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
#include "../../shared/hash.h"
#include "dictionary.h"

INVERTED_INDEX* index = NULL;

// initializes all data structures to be used
int initList() {
  index = InitIndex();
  return (index == NULL)? 0:1; 
}

// gloval variables: n (keeps trak of total number of crawled files in specified directory), alphasort
// is used for sorting the directory listing, dirent is used for storing files found in directory
int n; extern int alphasort();
struct dirent **namelist;

// filters any directories out
int file_select(struct direct *entry) {
	if ((strcmp(entry->d_name, ".")== 0) || (strcmp(entry->d_name, "..")== 0) || (strcmp(entry->d_name, "index.dat")== 0) || (strcmp(entry->d_name, "index_R.dat")== 0))
		return (FALSE);
	else
		return (TRUE);
}

// reads in a file and returns a string
char* ReadInFromFileOrDie(FILE* fp) {
	int len=0, ch; long lsize; 
	char * buffer; size_t result;
	
	if (fp == NULL)
		printf("File error");
	
	while(1) {
		ch = fgetc(fp);
		if (ch == EOF)
			break;
		++len;
	}	
	len += 1;
		
	fseek(fp, 0, SEEK_END);
	lsize = ftell(fp);
	rewind(fp);

	buffer = (char*) malloc (sizeof(char)*lsize);
	BZERO(buffer, lsize);
	
	if (buffer == NULL) {
		printf("Out of memory!");
		exit(1);
	}	
	result = fread (buffer,1,lsize,fp);
	
	if (result != lsize) {
		printf("Reading error occured!");
		exit(1);
	}
	return(buffer);
	fclose (fp);
	free (buffer);
}

// loads a document from a filename
char* loadDocument(char* fileName) {
	char * page_html;
	
	FILE* fp; fp = fopen(fileName, "r");
	
	if (fp == NULL) {
		printf("Error reading file [%s].\n", fileName);
		exit(1);
	}

	page_html = ReadInFromFileOrDie(fp);

	fclose(fp);
	return(page_html);
}

// function to visit all files in a directory
void visitFiles(char *dirname, INVERTED_INDEX* index) {
	int current=1;
	// chile the max number of files hasn't been visited
	while (current < (n+1)) {
		char* file; file = getFilePath(dirname, current);
		char path[strlen(file)+1]; strcpy(path, file);
		
		// load document and remove new lines from it				
		char * loadedDocument; loadedDocument = loadDocument(path);
		removeNewLines(loadedDocument);
		
		printf("\nProcessing HTML document %d.\n\n", current);
		
		char* word; int currentPosition = 0; int documentId = current;
				
		while (currentPosition < strlen(loadedDocument)){
			currentPosition = getNextWordFromHTMLDocument(loadedDocument, word, currentPosition, index, documentId);
		}			
		current++;	
	}
}

// function to scan a directory
void scanDirectory (char *dirname, INVERTED_INDEX* index, char* datFile) {
	int file_select();
	n = scandir(dirname, &namelist, file_select, alphasort);
	
	// if the directory contains to files to process, then exit
	if (n == 0) {
		LOGSTATUS("The directory is empty. Please specify another directory.");
		exit(1);
	}	
	visitFiles(dirname, index);
}

// cleans up the index
void cleanup() {
	CleanIndex(index);
	index = NULL;
}

// flag used to distinguish the first time through an HTML document
int flag=0;

int main (int argc, char** argv) {

	if (checkArguments(argc, argv) != 1) {
		return(0); exit(1);
	}

	if (initList() != 1) {
	  printf("Initialization wrong!");
	  exit(1);
	}
			
	LOGSTATUS("Start building index.");
	scanDirectory(argv[1], index, argv[2]);
	
	LOGSTATUS("Building the index completed!");
	printf("\nSaving the index to file.\n");
	saveIndexToFile(argv[1], argv[2], index);
	cleanup();
	
	reloadIndex(argv[1], argv[2], index);
	LOGSTATUS("Index has been recreated and stored.");
	
	return 0;
}