/*

	By: Ricky A. Melgares '11
	
	Filename: logic.c
	Description: contains functions that related to the logic of the query
	engine such as functions to get the intersection or union of two lists,
	as well as functions for parsing the inputted string in the query engine
	and some helper funtions for copying the contents of a list.

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

#include "../util/util.h"
#include "dictionary.h"
#include "logic.h"

// variables for keeping track of different intersections and unions
char* l1[1000]; char* l2[1000]; char* final[1000];

// makes a copy of an a list to a new one
void listcpy(char** new, char** original, int len) {
	for(int i = 0; i < len; i++) {
		strcpy(new[i], original[i]);
	}
}

// parses the string entered by the user in the query engine
void parseInput(int length, int pos, char* dirname, char** args, INVERTED_INDEX* index) {
	
	// if more than two words are present and first time through
	if ((pos == 0)) {
		prepList(l1, 1000); prepList(l2, 1000); prepList(final, 1000);
		
		char* list1[1000]; char* list2[1000];
		prepList(list1, 1000);prepList(list2, 1000);
		
		findWordData(args[0], list1, index);
		
		// if the second string is not a RESERVED word
		if((strcmp(args[1], "and") != 0) && (strcmp(args[1], "or") != 0)) {
			char* word[1000]; prepList(word, 1000);
			
			findWordData(args[1], word, index);
			
			listcpy(final, word, 1000);
			
			parseInput(length-1, 2, dirname, args, index);
		}
		// if the second string is a RESERVED word
		else{
			pos += 2;
			findWordData(args[pos], list2, index);
			
			// if string 2 -> reserved word AND
			if((strcmp(args[pos-1], "and") == 0)) {
				listcpy(final, list1, 1000);
				parseInput(length-2, pos, dirname, args, index);
				
				cleanList(list1, 1000);
			}
			// if string 2 -> reserved word OR
			else if((strcmp(args[pos-1], "or") == 0)) {
				
				char* temp[1000]; prepList(temp, 1000);
				
				unionOf(list1, list2, temp);
				listcpy(final, temp, 1000);
				
				cleanList(temp, 1000);
				// printMeta(dirname, temp);
				parseInput(length-4, pos+2, dirname, args, index);
			}
			// if string 2 -> no reserved word present (check string 3)
			else {
				char* temp[1000]; prepList(temp, 1000);
				
				intersection(list1, list2, temp);
				listcpy(final, temp, 1000);
				
				cleanList(temp, 1000);
				// printMeta(dirname, temp);
				parseInput(length-3, pos+1, dirname, args, index);
			}
		}
	}
	// if this is not our first time through
	else if ((pos > 0)) {
		// if we have more than two words
		if (length > 2) {			
			// if previous OR and next OR
			if((strcmp(args[pos-1], "or") == 0) && (args[pos+1] == NULL)) {
				char* temp[1000]; prepList(temp, 1000);
				char* word[1000]; prepList(word, 1000);
				
				findWordData(args[pos], word, index);
				
				unionOf(final, word, temp);
				listcpy(final, temp, 1000);
				printMeta(dirname, final);
				
				cleanList(temp, 1000); cleanList(word, 1000);
			}
			// if previous word was or and next word is or
			else if((strcmp(args[pos-1], "or") == 0) && (strcmp(args[pos+1], "or") == 0)) {
				char* temp[1000]; prepList(temp, 1000);
				char* word[1000]; prepList(word, 1000);
				
				findWordData(args[pos], word, index);
				
				unionOf(final, word, temp);
				listcpy(final, temp, 1000);
				
				cleanList(temp, 1000); cleanList(word, 1000);
				
				parseInput(length-2, pos+2, dirname, args, index);
			}
			// if previous word was AND and next word is OR
			else if(((strcmp(args[pos-1], "and") == 0)) && (strcmp(args[pos+1], "or") == 0)) {
				char* word1[1000]; char* word2[1000]; char* temp[1000]; char* temp2[1000];
				prepList(word1, 1000); prepList(word2, 1000); prepList(temp, 1000); prepList(temp2, 1000);
				
				findWordData(args[pos], word1, index);
				findWordData(args[pos+2], word2, index);
				
				unionOf(word1, word2, temp);
				intersection(final, temp, temp2);
				listcpy(final, temp2, 1000);
				
				cleanList(word1, 1000); cleanList(word2, 1000); cleanList(temp, 1000); cleanList(temp2, 1000); 
				
				parseInput(length-3, pos+3, dirname, args, index);
			}
			// if prevous was OR and next is AND
			else if((strcmp(args[pos-1], "or") == 0) &&  (strcmp(args[pos-3], "and") == 0) && (strcmp(args[pos-2], "and") != 0) && (strcmp(args[pos-2], "and") != 0)) {
				char* word[1000]; char* word2[1000]; char* temp[1000]; char* temp2[1000];
				prepList(word, 1000); prepList(word, 1000); prepList(temp, 1000); prepList(temp2, 1000);
				
				findWordData(args[pos-2], word, index);
				findWordData(args[pos], word2, index);
				unionOf(word, word2, temp);
				
				listcpy(final, temp, 1000);
				
				cleanList(word, 1000); cleanList(temp, 1000);
				
				parseInput(length-2, pos+2, dirname, args, index);
			}
			// else if previous word is no RESERVED word, then assume AND
			else if((strcmp(args[pos-1], "and") == 0) && (strcmp(args[pos+1], "and") == 0)) {
				char* word1[1000]; char* word2[1000]; char* temp[1000];
				prepList(word1, 1000); prepList(word2, 1000); prepList(temp, 1000);
				
				findWordData(args[pos-2], word1, index);
				findWordData(args[pos], word2, index);
				
				intersection(word1, word2, temp);
				listcpy(final, temp, 1000);
				
				cleanList(word1, 1000); cleanList(word2, 1000); cleanList(temp, 1000); 
				
				parseInput(length-4, pos+2, dirname, args, index);
			}
			// else if previous word is not a RESERVED word
			else if((strcmp(args[pos-1], "and") != 0) && (strcmp(args[pos-1], "and") != 0)) {
				// if next word is OR
				if (strcmp(args[pos], "or") == 0) {
					char* word[1000]; char* word1[1000]; char* word2[1000]; 
					char* temp[1000]; char* temp2[1000];
					
					prepList(word, 1000); prepList(word1, 1000); prepList(word2, 1000); 
					prepList(temp, 1000); prepList(temp2, 1000);
				
					findWordData(args[pos-2], word, index);
					findWordData(args[pos-1], word1, index);
					findWordData(args[pos+1], word2, index);
				
					unionOf(word1, word2, temp);
					intersection(word, temp, temp2);
					listcpy(final, temp2, 1000);
				
					cleanList(word, 1000); cleanList(word1, 1000); cleanList(word2, 1000);
					cleanList(temp, 1000); cleanList(temp2, 1000); 
				
					parseInput(length-3, pos+2, dirname, args, index);
				}
				// // if next word is AND
				else if (strcmp(args[pos], "and") == 0) {
					char* word[1000]; char* word1[1000]; char* word2[1000]; 
					char* temp[1000]; char* temp2[1000];
					
					prepList(word, 1000); prepList(word1, 1000); prepList(word2, 1000); 
					prepList(temp, 1000); prepList(temp2, 1000);
				
					findWordData(args[pos-2], word, index);
					findWordData(args[pos-1], word1, index);
					findWordData(args[pos+1], word2, index);
				
					intersection(word1, word2, temp);
					intersection(word, temp, temp2);
					listcpy(final, temp2, 1000);
				
					cleanList(word, 1000); cleanList(word1, 1000); cleanList(word2, 1000);
					cleanList(temp, 1000); cleanList(temp2, 1000);
				
					// printf("\nNEW LENGTH IS: %d\n", (length-3));
					parseInput(length-3, pos+1, dirname, args, index);
				}
				// if no next reserved word
				else {					
					char* word[1000]; char* word1[1000]; char* word2[1000]; 
					char* temp[1000]; char* temp2[1000];
					
					prepList(word, 1000); prepList(word1, 1000); prepList(word2, 1000); 
					prepList(temp, 1000); prepList(temp2, 1000);
				
					findWordData(args[pos-2], word, index);
					findWordData(args[pos-1], word1, index);
					findWordData(args[pos], word2, index);
				
					intersection(word1, word2, temp);
					intersection(word, temp, temp2);
					listcpy(final, temp2, 1000);
				
					cleanList(word, 1000); cleanList(word1, 1000); cleanList(word2, 1000);
					cleanList(temp, 1000); cleanList(temp2, 1000);
				
					parseInput(length-3, pos+1, dirname, args, index);
				}
			}
		}
		// if only two words remain
		else if (length == 2) {
			if((strcmp(args[pos], "and") != 0) && (strcmp(args[pos], "or") != 0) && (strcmp(args[pos+1], "and") != 0) &&(strcmp(args[pos+1], "or") != 0)) {
				if(strcmp(args[pos-1], "and") == 0) {
					char* temp[1000]; prepList(temp, 1000);
					char* word[1000]; prepList(word, 1000);
					
					findWordData(args[pos], word, index);
					intersection(final, word, temp);
					
					char* temp2[1000]; prepList(temp2, 1000);
					char* word2[1000]; prepList(word2, 1000);
					
					findWordData(args[pos+1], word, index);
					intersection(temp, word, temp2);
					printMeta(dirname, temp2);
					
					cleanList(temp, 1000); cleanList(word, 1000);
					cleanList(temp2, 1000); cleanList(word2, 1000);
				}
				else {
					char* temp[1000]; prepList(temp, 1000);
					char* word[1000]; prepList(word, 1000);
					
					findWordData(args[pos], word, index);
					intersection(final, word, temp);
					
					char* temp2[1000]; prepList(temp2, 1000);
					char* word2[1000]; prepList(word2, 1000);
					
					findWordData(args[pos+1], word, index);
					intersection(temp, word, temp2);
					printMeta(dirname, temp2);
					
					cleanList(temp, 1000); cleanList(word, 1000);
					cleanList(temp2, 1000); cleanList(word2, 1000);
				}
			}
			// if previous word was AND
			else {
				if (strcmp(args[pos], "and") == 0) {
					char* tempor [1000]; prepList(tempor, 1000);
					char* tword[1000]; prepList(tword, 1000);
					
					findWordData(args[pos+1], tword, index);
					intersection(final, tword, tempor);
					printMeta(dirname, tempor);
					
					cleanList(tempor, 1000); cleanList(tword, 1000);
					
				}
				else {
					printMeta(dirname, final);
					
				}
			}
		}
		// if only one word is left
		else if (length == 1) {
				// if remaining string is valid
				if ((strcmp(args[pos], "and") != 0) &&(strcmp(args[pos], "or") != 0)) {
					// if previous word was AND
						if(strcmp(args[pos-1], "and") == 0) {
							char* temp[1000]; prepList(temp, 1000);
							char* word[1000]; prepList(word, 1000);
				
							findWordData(args[pos], word, index);
				
							intersection(final, word, temp);
							printMeta(dirname, temp);
				
							cleanList(temp, 1000); cleanList(word, 1000);
						}
						// if previous word was OR
						else if(strcmp(args[pos-1], "or") == 0) {
							char* temp[1000]; prepList(temp, 1000);
							char* word[1000]; prepList(word, 1000);
				
							findWordData(args[pos], word, index);
				
							unionOf(final, word, temp);
							printMeta(dirname, temp);
				
							cleanList(temp, 1000); cleanList(word, 1000);
						}
						// no previous reserved word, assume AND
						else {
							char* temp[1000]; prepList(temp, 1000);
							char* word[1000]; prepList(word, 1000);
					
							findWordData(args[pos], word, index);
					
							intersection(final, word, temp);
							printMeta(dirname, temp);
					
							cleanList(temp, 1000); cleanList(word, 1000);
						}
				}
		}
		// no more words to parse, print final result
		else {
			printMeta(dirname, final);
		}
	}
}


// processes the input string and performs logic based on the input and reserved words
int checkArgs(int length, char*dirname, char** args, INVERTED_INDEX* index) {
	// only one word present
	if (length < 2) {
		if((strcmp(args[0], "and") != 0) && (strcmp(args[0], "or") != 0)) {
			char* result[1000]; prepList(result, 1000);

			if (findWordData(args[0], result, index) != 1) {
				return(0);
			}
			printMeta(dirname, result);
			cleanList(result, 1000);
		}
		else {
			printf("\nPlease enter a valid string.\n");
			return(0);
		}
		return(1);
	}
	// two words present 
	else if (length == 2) {
		// check for reserved words - AND case (space)
		if((strcmp(args[0], "and") != 0) && (strcmp(args[1], "and") != 0) && (strcmp(args[0], "or") != 0) && (strcmp(args[1], "or") != 0)) {
			// word1 list
			char* result1[1000]; prepList(result1, 1000);
			
			// word1 exists
			if (findWordData(args[0], result1, index) != 1) {
				return(0);
			}
			// word2 list
			char* result2[1000]; prepList(result2, 1000);
			
			//word2 exists
			if (findWordData(args[1], result2, index) != 1) {
				return(0);
			}
			// variable to store the intersected list
			char* inter[1000]; prepList(inter, 1000);
			
			// get the intersection of the two lists
			intersection(result1, result2, inter);
			printMeta(dirname, inter);
			// cleanup the lists
			cleanList(result1, 1000); cleanList(result2, 1000); cleanList(inter, 1000);
			
			return(1);
		}
		else {
			printf("\nPlease enter a valid string.\n");
			return(0);
		}			
	}
	else if (length == 3) {
		if((strcmp(args[0], "and") != 0) && (strcmp(args[0], "or")) && (strcmp(args[2], "and") != 0) && (strcmp(args[2], "or") != 0)) {
			if (strcmp(args[1], "and") == 0){
				// printf("\nAND present in the middle of the two words.\n");
				char* result1[1000]; prepList(result1, 1000);
				
				if(findWordData(args[0], result1, index) != 1) {
					return(0);
				}
				
				char* result2[1000]; prepList(result2, 1000);
				
				if(findWordData(args[2], result2, index) != 1) {
					return(0);
				}
				char* inter[1000]; prepList(inter, 1000);
				
				intersection(result1, result2, inter);
				printMeta(dirname, inter);
				
				cleanList(result1, 1000); cleanList(result2, 1000); cleanList(inter, 1000);
				
				return(1);
			}
			else if (strcmp(args[1], "or") == 0){
				// printf("\nOR present in the middle of the two words.\n");
				char* result1[1000]; prepList(result1, 1000);
				
				if(findWordData(args[0], result1, index) != 1) {
					return(0);
				}
				
				char* result2[1000]; prepList(result2, 1000);
				
				if(findWordData(args[2], result2, index) != 1) {
					return(0);
				}
				char* oring[1000]; prepList(oring, 1000);
				
				unionOf(result1, result2, oring); 
				printMeta(dirname, oring);
				
				cleanList(result1, 1000); cleanList(result2, 1000); cleanList(oring, 1000);
			}
			else if((strcmp(args[0], "and") != 0) && (strcmp(args[0], "or")) && (strcmp(args[1], "and") != 0) && (strcmp(args[1], "or") != 0) && (strcmp(args[2], "and") != 0) && (strcmp(args[2], "or") != 0)) {
				char* temp1[1000]; char* temp2[1000]; char* temp3[1000];
				prepList(temp1, 1000); prepList(temp2, 1000); prepList(temp3, 1000);
				
				char* r1[1000]; char* r2[1000];
				prepList(r1, 1000); prepList(r2, 1000);
				
				findWordData(args[0], temp1, index); 
				findWordData(args[1], temp2, index);
				findWordData(args[2], temp3, index);
				
				intersection(temp1, temp2, r1); intersection(r1, temp3, r2);
				printMeta(dirname, r2);
				
				cleanList(temp1, 1000); cleanList(temp2, 1000); cleanList(temp3, 1000);
				cleanList(r1, 1000); cleanList(r2, 1000);
			}
		}
		else {
			printf("\nPlease enter a valid string.\n");
			return(0);
		}
	}
	else if (length > 3) {
		parseInput(length, 0, dirname, args, index);
	}
	return(1);
}

// gets the intersection of the two lists
void intersection(char** list1, char** list2, char** result) {
	int n1 = 0; int n2 = 0; int r = 0; int val;
	
	// while the visited element in each list is not NULL
	while((list1[n1] != NULL) || (list2[n2] != NULL)) {
		if(list1[n1] == NULL || list2[n2] == NULL)
			break;	
		val = strcmp(list1[n1], list2[n2]);
		
		char temp1[10000]; strcpy(temp1, list1[n1]);
		char temp2[10000]; strcpy(temp2, list2[n2]);
		
		char* word1; word1 = strtok(temp1, " ");
		char* freq1; freq1 = strtok(NULL, " ");
		
		char* word2; word2 = strtok(temp2, " ");
		char* freq2; freq2 = strtok(NULL, " ");
		
		char string[1000];
		
		if((word1 == NULL) || (word2 == NULL)) {
			break;
		}
		
		// if we find a match, copy it over
		if (strcmp(word1, word2) == 0) {
			sprintf(string, "%s %d", word1, (atoi(freq1)+atoi(freq2)));
			result[r] = (char*)malloc(sizeof(char)*1000);
			BZERO(result[r], 1000);
			strcpy(result[r], string);
			r++; n1++; n2++;
		}
		else {
			if(val == -1)
				n1++;
			else
				n2++;		
		}
	}
}

// gets the intersection of the two lists
void unionOf(char** list1, char** list2, char** result) {
	int n1 = 0; int n2 = 0; int r = 0; int val;
	
	while((list1[n1] != NULL) || (list2[n2] != NULL)) {
		if(list1[n1] == NULL || list2[n2] == NULL)
			break;	
		val = strcmp(list1[n1], list2[n2]);
		
		char temp1[10000]; strcpy(temp1, list1[n1]);
		char temp2[10000]; strcpy(temp2, list2[n2]);
		
		char* word1; word1 = strtok(temp1, " ");
		char* freq1; freq1 = strtok(NULL, " ");
		
		char* word2; word2 = strtok(temp2, " ");
		char* freq2; freq2 = strtok(NULL, " ");
		
		char string[1000];
		
		if((word1 == NULL) && (word2 == NULL)) {
			break;
		}
		
		// only add one instance of the word
		if ((word1 != NULL) && (word2 != NULL) && (strcmp(word1, word2) == 0)) {
			sprintf(string, "%s %d", word1, (atoi(freq1)+atoi(freq2)));
			result[r] = (char*)malloc(sizeof(char)*1000);
			BZERO(result[r], 1000);
			strcpy(result[r], string);
			r++; n1++; n2++;
		}
		// if the words aren't equal, add the words appropiately
		else if((word1 != NULL) && (word2 != NULL)) {
			if(val == -1) {
				sprintf(string, "%s %d", word1, atoi(freq1));
				result[r] = (char*)malloc(sizeof(char)*1000);
				BZERO(result[r], 1000);
				strcpy(result[r], string);
				r++;
				n1++;
			}
			else {
				sprintf(string, "%s %d", word2, atoi(freq2));
				result[r] = (char*)malloc(sizeof(char)*1000);
				BZERO(result[r], 1000);
				strcpy(result[r], string);
				r++;
				n2++;
			}		
		}
		else if ((word1 != NULL) && (word2 == NULL)) {
			sprintf(string, "%s %d", word1, atoi(freq1));
			result[r] = (char*)malloc(sizeof(char)*1000);
			BZERO(result[r], 1000);
			strcpy(result[r], string);
			// printf("\nThe copied result is: %s\n", result[r]);
			r++;
			n1++;
		}
		// if elements are not equal
		else if((word1 == NULL) && (word2 != NULL)) {
			sprintf(string, "%s %d", word2, atoi(freq2));
			result[r] = (char*)malloc(sizeof(char)*1000);
			BZERO(result[r], 1000);
			strcpy(result[r], string);
			// printf("\nThe copied result is: %s\n", result[r]);
			r++;
			n2++;
		}
	}
}