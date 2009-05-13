// Filename: input.c (for input.h)
// Description: Contains functions for checking any input such as the arguments, as well as the input to
//              to other functions, and parsing certain input or variables, and dealing with output.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/dir.h>
#include <sys/param.h>
#include <ctype.h>
#include <curses.h>

#include "../src/dictionary.h"
#include "input.h"

// input logic processing
int checkArguments(int argc, char** argv) {
	printf("\nRunning the query engine (called tinysearch)\n");
	
	if ((argc == 0) || (argc < 3)) {
		printf("\nBad usage: please make sure the following arguments are present\n");
		printf("[INDEX FILE NAME] [TARGET_DIRECTORY]\n");
		printf("\nExample command input:");
		printf("\n./query index.dat ../../data\n\n");
		return(0);
	}	
	struct stat statbuf;
	
	if (stat(argv[2], &statbuf) == -1) {
		printf("\nThe following directory does not exist: %s\nPlease enter a valid directory.\n\n", argv[2]);
		return(0);
	}
	return(1);
}

// parses the filepath given a directory name and file name
char* getFilePath(char* dirname, int file) {
	char* sfile;
	sfile = malloc(sizeof(char) * 1000);
	BZERO(sfile, 1000);
	
	char* path;
	path = malloc(sizeof(char) * 1000);
	BZERO(path, 1000);
	
	sprintf(sfile, "%d", file);	
	strcpy(path, dirname); strcat (path, "/"); strcat(path, sfile);
	
	free(sfile);
	sfile = NULL;
		
	return(path);
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

// prunes the html document from any new line characters
void removeNewLines(char* html) {
  int i;
  char* buffer;
  buffer = malloc(sizeof(char) * strlen(html) + 1);
  BZERO(buffer, (strlen(html) +1));

  char* p = malloc (sizeof(char) * 1);
  BZERO(p, 1);

  for (i=0;html[i];i++) {
    if(html[i]>31){
      sprintf(p,"%c",html[i]);
      strcat(buffer,p);
    }
  }
  strcpy(html,buffer);
  free(buffer); free(p);
}

// changes all words to lower case
void NormalizeWord(char* word) {
  int i = 0;
  while (word[i]) {
    if (word[i] < 91 && word[i] > 64)
      word[i] += 32;
    i++;
  }
}

// appends a single character to a string
void append(char* s, char c) {
	int len = strlen(s);
	s[len] = c;
    s[len+1] = '\0';
}
