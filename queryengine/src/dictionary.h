// File: dictionary.h
// This file contains useful information for implementing the search engine:

// DEFINES

// #define max WORD_LENGTH 1000
#define WORD_LENGTH 1000

// #define MAX_HASH_SLOT 10000
#define MAX_HASH_SLOT 10000

//! \brief Print \a s together with the source file name and the current line number.
#define LOGSTATUS(s)  printf("[%s:%d]%s\n", __FILE__, __LINE__, s)


//! \brief Check whether \a s is NULL or not. Quit this program if it is NULL.
#define MYASSERT(s)  if (!(s))   {                                      \
    printf("General Assert Error at %s:line%d\n", __FILE__, __LINE__);  \
    exit(-1); \
  }


//! \brief Check whether \a s is NULL or not on a memory allocation. Quit this program if it is NULL.
#define MALLOC_CHECK(s)  if ((s) == NULL)   {                     \
    printf("No enough memory at %s:line%d ", __FILE__, __LINE__); \
    perror(":");                                                  \
    exit(-1); \
  }

//! \brief Set memory space starts at pointer \a n of size \a m to zero. 
#define BZERO(n,m)  memset(n, 0, m)


// DATA STRUCTURES. All these structures should be malloc 'd
typedef struct _DocumentNode {
  struct _DocumentNode *next;        // pointer to the next member of the list.
  int document_id;                   // document identifier
  int page_word_frequency;           // number of occurrences of the word
} __DocumentNode;

typedef struct _DocumentNode DocumentNode;

typedef struct _WordNode {
  struct _WordNode *prev;           // pointer to the previous word
  struct _WordNode *next;           // pointer to the next word
  char word[WORD_LENGTH];           // the word
  DocumentNode  *page;              // pointer to the first element of the page list.
} __WordNode;
typedef struct _WordNode WordNode;

typedef struct _INVERTED_INDEX {
                                      // Start and end pointer of the dynamic links.
WordNode *start;                      // start of the list
WordNode *end;                        // end of the list
WordNode *hash[MAX_HASH_SLOT];  // hash slot
} __INVERTED_INDEX;

typedef struct _INVERTED_INDEX INVERTED_INDEX;

//PROTOTYPES used by various components of the search engine.

// initIndex: creates a new index inverted index object.
INVERTED_INDEX* InitIndex();

// insert a page object into the index
void IndexAdd(INVERTED_INDEX* index, void* page, char* word);

// updateIndex: Updates the data structure containing the index. It receives as input a
// string containing the word and the identifier of the document. It returns 1 (true) if
// successful, 0 (false) otherwise.
int updateIndex(char* word, int documentId, INVERTED_INDEX* index);

// tweaked version of update index to insert an element into the index along with with the frequency
int insertElementIntoIndex(char* word, int documentId, int frequency, INVERTED_INDEX* index);

// retrieves the doc and frequency data for the given word
int findWordData(char* word, char** list, INVERTED_INDEX* index);

// function to reload an index from a file, re-create the index structure for it, and output it to a new file
INVERTED_INDEX* reloadIndex(char* dirname, char* filename, INVERTED_INDEX* index);

// saves/writes the index to the specified file at the prompt
void saveIndexToFile(char* dirname, char* filename, INVERTED_INDEX* index);

// cleans the index by freeing up all the WordNode's
void CleanIndex(INVERTED_INDEX* index);

// gets the next word from html document
int getNextWordFromHTMLDocument (char* html, char* word, int pos, INVERTED_INDEX* index, int documentId);