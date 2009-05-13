// Filename: util.c (for util.h)
// Description: Contains helper function for processing lists among other things.

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

#include "../util/input.h"
#include "../src/dictionary.h"
#include "util.h"

// mallocs and BZERO's the slots in the list of defined length
void prepList(char** list, int length) {
	for (int i=0; i <length; i++) {
		list[i] = (char*)malloc(sizeof(char)*(length));
		BZERO(list[i], (length));
	}
}

// goes through a list and for slots length it free's and sets the slot to NULL
void cleanList(char** list, int length) {
	for (int i=0; i < length; i++) {
		free(list[i]);
		list[i] = NULL;
	}
}

void zeroList(char** list, int length) {
	for (int i=0; i<length; i++) {
		BZERO(list[i], 999);
	}
}

// converts the input string into a list of words
char** formList(char** word_list, char* input) {
	int n = 0;
	
	char temp[1000];
	strcpy(temp, input);
	
	char* word; 
	word = (char*)malloc(sizeof(char)*1000); 
	BZERO(word, 1000);
	
	word = strtok(temp, " ");
	
	word_list[n] = (char*)malloc(sizeof(char)*1000); 
	BZERO(word_list[n], 1000);
	 
	strcpy(word_list[n], word);
	// printf("\nWord %d: %s\n", n, word);
	
	while((word = strtok(NULL, " ")) != NULL) {
		n++;
		word_list[n] = (char*)malloc(sizeof(char)*1000);
		BZERO(word_list[n], 1000);
		strcpy(word_list[n], word);
		// printf("Word %d: %s\n", n, word);
	}
	return(word_list);
}

// visits the files referenced by the items in the lists and retrieves and prints the URL for each
void printMeta(char *dirname, char** result) {
	// int length = getLength(result);
	int i = 0;
	
	FILE* data; data = fopen("data", "w");
	
	while(result[i]!= NULL) {
		char* temp; 
		temp = (char*)malloc(sizeof(char)*1000);
		BZERO(temp, 1000);
		strcpy(temp, result[i]);
		
		char* id; 
		id = (char*)malloc(sizeof(char)*2000);
		BZERO(id, 2000);
		id = strtok(temp, " ");
		
		char* freq; 
		freq = (char*)malloc(sizeof(char)*2000);
		BZERO(freq, 2000);
		freq = strtok(NULL, " ");
		
		if(id == NULL) {
			i++;
			break;
		}
		// printf("Parsed ID: %s\n", id);
		char* path = getFilePath(dirname, atoi(id));	
		
		FILE* fp; fp = fopen(path, "r");
		if(fp == NULL) {
			i++;
			break;
		}
		char* buffer;
		buffer = (char*)malloc(sizeof(char)*2000);
		BZERO(buffer, 2000);
		
		if(fgets(buffer, 2000, fp) == NULL) {
			i++;
			break;
		}
		else {
			// printf("Document ID: %s Frequency: %s URL:%s", id, freq, buffer);
			fprintf(data, "%s DocID: %s URL:%s", freq, id, buffer);
		}
		fclose(fp);
		i++;
	}
	fclose(data);
	
	char* command;
	command = (char*)malloc(sizeof(char)*1000);
	BZERO(command, 1000);
	strcpy(command, "sort -rn < data > data.sort");
	system(command);
	
	FILE* sorted;
	sorted = fopen("data.sort", "r");
	char word[1000];
	fgets(word, 1000, sorted);
	int c = 0;
	while(!feof(sorted)) {
		printf("%s", word);
		fgets(word, 1000, sorted);
		c++;
	}
	
	// for(int j = 0; j < i; j++) {
	// 	printf("%s\n", list[j]);
	// }
	
	printf("Total results returned: %d\n", (i-1));
}

// returns the length of the array
int getLength(char** list) {
	int len = 0;
	for(int i=0; list[i] != NULL; i++)
		len++;
	return(len);
}

// returns the maximum of the two numbers
int getMax(int one, int two) {
	if(two > one)
		return(two);
	else
		return(one);
}