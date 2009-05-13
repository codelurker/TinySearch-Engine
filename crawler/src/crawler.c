/*

  By: Ricky A. Melgares '11, April 26, 2009

  FILE: crawler.c

  Description:

  Inputs: ./crawler [SEED URL] [TARGET DIRECTORY WHERE TO PUT THE DATA] [MAX CRAWLING DEPTH]

  Outputs: For each webpage crawled the crawler program will create a file in the 
  [TARGET DIRECTORY]. The name of the file will start a 1 for the  [SEED URL] 
  and be incremented for each subsequent HTML webpage crawled. 

  Each file (e.g., 10) will include the URL associated with the saved webpage and the
  depth of search in the file. The URL will be on the first line of the file 
  and the depth on the second line. The HTML will for the webpage 
  will start on the third line.

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "crawler.h"
#include "../../shared/html.h"
#include "../../shared/hash.h"

DICTIONARY* dict = NULL; 

//variable used to keep track of the file count
int index_html_file_counter=1; 

// Input command processing logic
void checkArguments(int argc, char** argv) {
	int exit_flag=0;
	if ((argc == 0) || (argc < 4)) {
		printf("\nBad usage: please make sure the following arguments are present\n");
		printf("[SEED_URL] [TARGET_DIRECTORY WHERE TO PUT THE DATA] [CRAWLING DEPTH]\n\n");
		exit(1);
	}	
	int res; char command [100] = "wget -q "; strcat(command, argv[1]); res = system(command); system("rm -f index.html");

	if(res != 0) {
		printf("\nThe following URL is invalid: %s\nPlese enter a valid url.\n\n", argv[1]);
		exit_flag++;
	}
	int depth = atoi(argv[3]);

	if(depth > 4) {
		printf("The crawl depth cannot exceed 4. Please enter a valid crawl depth.\n\n");
		exit_flag++;
	}		
	struct stat statbuf;

	if (stat(argv[2], &statbuf) == -1) {
		printf("The following directory does not exist: %s\nPlease enter a valid directory.\n\n", argv[2]);
		exit_flag++;
	}
	if (exit_flag > 0) {
		exit(1);
	}	
}

// Initialize needed data structures
void initLists() {
	// code snippet for malloc-ing space for the dictionary  and initializing it
	dict = (DICTIONARY*)malloc(sizeof(DICTIONARY));
	MALLOC_CHECK(dict);
	BZERO(dict, sizeof(DICTIONARY));
	dict->start = dict->end = NULL;  // make explicit
	
	dict->start = dict->end = malloc(sizeof(DNODE));
}

// function to read in a file to memory
char* readInFromFileOrDie(char* fp) {
	int len=0, ch; long lsize; char * buffer; size_t result;
	
	FILE *in_file; in_file = fopen(fp, "r");
	
	if (in_file == NULL)
		printf("File error");
	
	while(1) {
		ch = fgetc(in_file);
		if (ch == EOF)
			break;
		++len;
	}	
		
	len += 1;
		
	fseek(in_file, 0, SEEK_END);
	lsize = ftell(in_file);
	rewind(in_file);

	buffer = (char*) malloc (sizeof(char)*lsize);
	BZERO(buffer, lsize);

	if (buffer == NULL) {
		printf("Out of memory!");
		exit(1);
	}	
	result = fread (buffer,1,lsize,in_file);
	
	if (result != lsize) {
		printf("Reading error occured!");
		exit(1);
	}	
	return(buffer);
	
	fclose (in_file);
	free (buffer);
}

// heavy lifting: takes url list and for each url, it determines if it is unique. URL inserted if unique.
void updateListLinkToBeVisited(char **urlist, int depth) {
	int index = 0;
	
	while (urlist[index] != NULL) {

		if(NormalizeURL(urlist[index]) == 0) {			
			free(urlist[index]);
			urlist[index] = NULL;			
			index++;
			continue;
		}
			
		int hashIndex;
		hashIndex = hash1(urlist[index])%MAX_HASH_SLOT;
		// printf("\nThe hash index of [%s] is: %d", urlist[index], hashIndex);
		
		// create DNODE(using malloc)
		DNODE* new = malloc(sizeof(DNODE));
		MALLOC_CHECK(new);
		
		// create URLDNODE(using malloc)
		URLNODE* n = malloc(sizeof(URLNODE));
		MALLOC_CHECK(n);
		
		//copy the URL to the URLNODE and initialise it
		n->depth = depth;
		n->visited = 0;
		BZERO(n->url, MAX_URL_LENGTH);
		strncpy(n->url, urlist[index], MAX_URL_LENGTH);
		
		//link it into the DNODE(and initialise it)
		new->data = n;
		new->next = new-> prev = NULL;
		BZERO(new->key, MAX_URL_LENGTH);
		strncpy(new->key, urlist[index], MAX_URL_LENGTH);
		
		// printf("\nThe key of the new DNODE is: %s\n", new->key);
		
		if(dict->hash[hashIndex] == NULL) {
			// save dict end node
			DNODE* dprev = dict->end;
			DNODE* dnext = (dict->end)->next;
			
			// re-link/re-point pointers
			dprev->next = new;
			new->next = dnext;
			new->prev = dprev;
			
			// set the hash to the new DNODE
			dict->hash[hashIndex] = new;
			
			//upate the hash end pointer
			dict->end = new;
			new->next = NULL;
		}	
		else {
			// printf("Checking for uniqueness throughout the rest of the cluster.\n");
			DNODE* orig = dict->hash[hashIndex];				
			DNODE* temp = dict->hash[hashIndex]; URLNODE* unode = new->data;
			int flag = 0;
			
			if (((temp->next) == NULL)) {
				if ((strcmp(temp->key, urlist[index])) != 0) {
					// printf("URL is not present in the cluster.\n");
					
					temp->next = new;
					new->next = NULL;
					new->prev = temp;
					
					dict->end = new;
				}
				else {
					// printf("The URL is aready present.\n");
					free(n);					
					free(new);
				}
			}
			else {
				// printf("Searching for [%s] in the cluster.\n", urlist[index]);
				if (strcmp((orig->key), unode->url) == 0) {
					flag++;
				}
				
				do {
					temp = temp->next; unode = temp->data;
					if((strcmp(temp->key, urlist[index])) == 0) {
						// printf("URL is already present in the cluster!\n");
						// if (temp->data != NULL)
						// 	free(temp->data);
						// temp->data = data;
						flag++;
						break;
					}
					// printf("Inside do statement\n");
				} while ((hash1(temp->key)%MAX_HASH_SLOT) == hashIndex);
				
				// if (flag > 0)
					// printf("Don't insert the new node.\n");
				if(!(flag > 0)) {
					// printf("You should insert the new node.\n");
					// printf("@ %s\n", temp->key);
					
					DNODE* dnext = temp;
					DNODE* dprev = temp->prev;
					
					dprev-> next = new;
					new->next = dnext;
					new->prev = dprev;	
				}
				else {
					free(n);
					free(new);				
				}
			}		
		}
		free(urlist[index]);
		urlist[index] = NULL;
		index++;
	}
}

void printVisitedFlagForURL(char* url) {
	int hashIndex = hash1(url)%MAX_HASH_SLOT;
	
	DNODE* temp = dict->hash[hashIndex]; URLNODE* unode = temp->data;
	
	// if ((!strcmp(temp->key, url)) || (temp->next) == NULL)
		// printf("\nThe current key value for [%s] is: %d\n", url, unode->visited);
	// else {
	if ((strcmp(temp->key, url)) || (temp->next) == NULL) {
		// printf("\nSearching for [%s] in the cluster.\n", url);
		do {
			temp = temp->next; unode = temp ->data;
			if(strcmp(temp->key, url)) {
				// printf("We found a match! The current key value of [%s] is: %d\n", url, unode->visited);
				break;
			}
		} while ((hash1(temp->key)%MAX_HASH_SLOT) == hashIndex);
	}
}

// setURLasVisited: Mark the URL as visited in the URLNODE structure.
void setURLasVisited(char* url) {
	NormalizeURL(url);

	int hashIndex = hash1(url)%MAX_HASH_SLOT;
	
	DNODE* temp = dict->hash[hashIndex]; 
	URLNODE* unode = temp->data;
	
	// if we find a match and reach end of the cluster
	if ((!strcmp(temp->key, url)) || (temp->next) == NULL) {
		unode->visited=1;
		// printf("\nThe visited value of [%s] has been set to: %d.\n", unode->url, unode->visited);		
	}
	else {
		// no match for first item in the cluster, step through cluster
		// printf("\nSearching for [%s] in the cluster.\n", url);
		do {
			// move to next node in the cluster
			temp = temp->next; 
			unode = temp->data;
			
			// if we find a match
			if(!(strcmp(temp->key, url))) {
				unode->visited = 1;
				// printf("\nThe visited value of [%s] has been set to: %d.\n", unode->url, unode->visited);
				break;
			}
			else {
				// printf("\n[%s] has already been visited.", unode->url);
				break;
			}
		} while ((hash1(temp->key)%MAX_HASH_SLOT) == hashIndex);
	}
}

// scans through the dictionary and finds the first URL that hasn't been visited already.
char *getAddressFromTheLinksToBeVisited(int *depth) {
	DNODE* d;
	d = dict->start;
	
	while (d != NULL) {
		if (d->next == NULL) {
			return NULL;
		}
		else {
			d = d->next;
			URLNODE* data = d->data;
			if (data->visited == 0) {
				*depth = data->depth;
				return(data->url);
			}
		}
	}
	return NULL;
}

char *url_list[MAX_URL_PER_PAGE];

// given a string of an HTML page, page is parsed and all URL's in that page are extract and returned as a url list
char **extractURLs(char* html_buffer, char* current) {
	int pos=0; int index=0;
	
	// value to store the parsed URL
	char result[1000];
	BZERO(result, 1000);

	while ((pos = GetNextURL(html_buffer, current, result, pos)) > 0) {
		if(result[0] == 0)
			continue;
	
		if (!strncmp(result, URL_PREFIX, min(strlen(result) - 1, strlen(URL_PREFIX) - 1)) && (NormalizeURL(result) == 1) 
                                                                                          && (result[strlen(result)] != ';')
																						  && (result[strlen(result) - 1] != ';')) {
			url_list[index] = malloc(MAX_URL_LENGTH);
			MALLOC_CHECK(url_list[index]);
			BZERO(url_list[index], MAX_URL_LENGTH);
			strncpy(url_list[index], result, MAX_URL_LENGTH);	

			printf("[crawler]Parser find link: %s\n", url_list[index]);
			index++;
		}
		BZERO(result, 1000);
	}
	return(url_list);
}

// appends a single character to a string
void append(char* s, char c) {
	int len = strlen(s);
	s[len] = c;
    s[len+1] = '\0';
}

// downloads the html page for the specified URL, and stores the URL and depth at the top
char* getPage(char* url, int depth, char* path) {	
	char * page_html;
	
	printf("\n[Crawler]Crawling %s\n\n", url);

	int res, counter = 0; char command[100] = "wget -O temp "; strcat(command, url); res = system(command);
	
	do {
		if (res!= 0) {
			printf("\nRetrieval failed. Trying again. Try: %d\n", counter);
			counter++;
		}
		else {
			break;
			}
		} while (counter != 4);
		
	page_html = readInFromFileOrDie("temp");
	
	// char scounter[10000];
	char* scounter;
	scounter = (char*)malloc(sizeof(char)*1000);
	BZERO(scounter, 1000);
	sprintf(scounter, "%d", index_html_file_counter);
	
	// char scommand[10000];
	char* scommand;
	scommand = (char*)malloc(sizeof(char)*1000); 
	BZERO(scommand, 1000);
	strcpy(scommand, "touch "); 
	strcat(scommand, path); append(scommand, '/');
	strcat(scommand, scounter); 
	// strcat(scommand, ".html");
	
	char* file; 
	file = (char*)malloc(sizeof(char)*1000); 
	BZERO(file, 1000);
	strcpy (file, path); append(file, '/');
	strcat (file, scounter);
	free(scounter); scounter = NULL;
	
	system(scommand); index_html_file_counter++;
	free(scommand); scommand = NULL;
		
	FILE *html_file; html_file = fopen(file, "w");
	fprintf(html_file, "%s\n%d\n%s", url, depth, page_html);
	fclose(html_file);
	free(file);
	file = NULL;
	
	return(page_html);
}

int main(int argc, char** argv) {
	char** urls_lists; char* page; 
	char* seed_url = argv[1];
	int current_depth = 0; int max_depth = atoi(argv[3]);
	
	checkArguments(argc, argv);
	
	initLists();
	
	// bootstrapping the first part of the crawler
	int seed_hash;
	seed_hash = hash1(argv[1])%MAX_HASH_SLOT;
	
	// INSERT SEED_URL INTO DICT
	// create seed DNODE
	DNODE* seed_dnode = malloc(sizeof(DNODE));
	MALLOC_CHECK(seed_dnode);
	
	// create seed URLNODE
	URLNODE* url_seed_node = malloc(sizeof(URLNODE));
	MALLOC_CHECK(url_seed_node);
	
	// copy the URL to seed URLNODE and initalize it
	url_seed_node->depth = 0;
	url_seed_node->visited = 0;
	BZERO(url_seed_node->url, MAX_URL_LENGTH);
	strncpy(url_seed_node->url, argv[1], MAX_URL_LENGTH);
	 
	// link it to the seed DNODE (and initalize it)
	seed_dnode->data = url_seed_node;
	seed_dnode->next = seed_dnode->prev = NULL;
	BZERO(seed_dnode->key, MAX_URL_LENGTH);
	strncpy(seed_dnode->key, argv[1], MAX_URL_LENGTH);
	
	// initialize and link DICTIONARY MODE
	dict->start = dict->end = seed_dnode;
	seed_dnode->next = NULL;
	seed_dnode->prev = dict->start;
	dict->hash[seed_hash] = seed_dnode;
	
	// get page for seed URL
	page = getPage(argv[1], 0, argv[2]);
	if (page == NULL) {
		LOGSTATUS("PANIC: Cannot crawl SEED_URL");
	}	

	// extract url's from seed page
	urls_lists = extractURLs(page, seed_url);
	free(page);

	// add unique urls to urls list
	updateListLinkToBeVisited(urls_lists, current_depth + 1);
	setURLasVisited(argv[1]);

	char* URLToBeVisited;
	// Main processing loop of crawler. While there are URL to visit and the depth is not 
	// exceeded keep processing the URLs.
	while((URLToBeVisited = getAddressFromTheLinksToBeVisited(&current_depth)) != NULL) {
		// for URLs over max depth, mark as visited and continue
		if (current_depth > max_depth) {
			setURLasVisited(URLToBeVisited);
			continue;
		}
		page = getPage(URLToBeVisited, current_depth, argv[2]);
		if (page == NULL) {
			LOGSTATUS("PANIC: Cannot crawl SEED_URL");
			printf("Panic! Cannot crawl the URL [%s].\n", URLToBeVisited);
			setURLasVisited(URLToBeVisited);
			continue;
		}

  		//extract URLs from next page
		urls_lists = extractURLs(page, URLToBeVisited);
  		free(page);
	
		updateListLinkToBeVisited(urls_lists, current_depth + 1);
 		setURLasVisited(URLToBeVisited);

		// sleep interval delay before crawling next page to sneak by servers
		sleep(.05);
	}

	// done
	LOGSTATUS("Nothing more to crawl!");

	/*(11) *cleanup* Clean up data structures and make sure all files are closed,
	      resources deallocated. */

	DNODE* temp = dict->start;
	while (temp != NULL) {
		DNODE* d = temp;
		if (d->data != NULL)
			free(d->data);
		temp = temp->next;
		free(d);
		d = NULL;
	}
	
	free(dict);
	dict = NULL;

	return 0;
}
