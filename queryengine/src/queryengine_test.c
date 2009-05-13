// Filename: Test cases for logic.h/.c
// Description: A unit test for logic processing for query engine
// 

//
// Test Harness Spec:
// ------------------
//
//  It uses these files but they are not unit tested in this test harness:
//
//  int findWordData(char* word, char** list, INVERTED_INDEX* index);
//  void prepList(char** list, int length);
//  void cleanList(char** list, int length);
//  char** formList(char** word_list, char* input);
//  void printMeta(char *dirname, char** result);
//
//  If any of the tests fail it prints status 
//  If all tests pass it prints status.
//
//  Test Cases:
//  -----------
//   
//  The test harness runs a number of test cases to test the code.
//  The approach is to first set up the environment for the test,
//  invoke the function to be tested, then validate the state of
//  the data structures using the SHOULD_BE macro. This is repeated
//  for each test case. 
//
//  The test harness isolates the functions under test and setting
//  up the environment the code expects when integrated in the real
//  system.
//
//
//
//
//  The following test cases (1-2) for function:
//
//  void intersection(char** list1, char** list2, char** result);
//
//  Test case1: intersection(word1, word2, result)
//  This test case calls intersection() for the condition where
//  the intersection of two lists must be performed, for which the
//  the result is stored in result. Intersection for which the result
//  should be an empty list.
//
//  Test case2: intersection(word1, word2, result)
//  This test case calls intersection() for the condition where
//  the intersection of two lists must be performed, for which the
//  the result is stored in result. Intersection for which the result
//  should not be an empty list.
//
//
//  The following test cases (1-2) for function:
//
//  void unionOf1(char** list1, char** list2, char** result);
//
//  Test case: unionOf(list1, list2, result)
//  This test case calls unionOf() for the condition where
//  the union of two lists must be performed, for which the
//  the result is stored in result.
//
//  void unionOf2(char** list1, char** list2, char** result);
//
//  Test case: unionOf(list1, list2, result)
//  This test case calls unionOf() for the condition where
//  the union of two lists must be performed, for which the
//  the result is stored in result.
//
//
//
//  The following test cases (1) for function:
//
//  int checkArgs(int length, char*dirname, char** args, INVERTED_INDEX* index);
//
//  Test case:checkArgs:
//  This test case checkArgs() check to make sure that the items were processed
//  succesfully.
//
//

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

// Useful MACROS for controlling the unit tests.

// each test should start by setting the result count to zero

#define START_TEST_CASE  int rs=0

// check a condition and if false print the test condition failed
// e.g., SHOULD_BE(dict->start == NULL)

#define SHOULD_BE(x) if (!(x))  {rs=rs+1; \
    printf("Line %d Fails\n", __LINE__); \
  }


// return the result count at the end of a test

#define END_TEST_CASE return rs

//
// general macro for running a best
// e.g., RUN_TEST(TestDAdd1, "DAdd Test case 1");
// translates to:
// if (!TestDAdd1()) {
//     printf("Test %s passed\n","DAdd Test case 1");
// } else { 
//     printf("Test %s failed\n", "DAdd Test case 1");
//     cnt = cnt +1;
// }
//

#define RUN_TEST(x, y) if (!x()) {              \
    printf("Test %s passed\n", y);              \
} else {                                        \
    printf("Test %s failed\n", y);              \
    cnt = cnt + 1;                              \
}
    
// Dumming out routines 
// We want to isolate this set of functions and test them and control
// various conditions with the hash table and function. For example,
// we want to create collisions. So in order to do this we dummy out the
// hash function and make it return whatever the current value of hash is
// In out test suite we mainpulate the value of hash so when the "real code"
// calls our dummy hash function it always returns the value we set in hash.
// Devious, hey?

// what we want the our inter/union functions to look at
INVERTED_INDEX* index = NULL;

// initializes all data structures to be used
int initList() {
  index = InitIndex();
  return (index == NULL)? 0:1; 
}

//  Test case1: intersection(word1, word2, result)
//  This test case calls intersection() for the condition where
//  the intersection of two lists must be performed, for which the
//  the result is stored in result. Intersection for which the result
//  should be an empty list.

int TestInter1() {
  START_TEST_CASE;
	if (initList() != 1) {
	printf("Initialization wrong!");
	exit(1);
	}
	
	index = reloadIndex("../../data", "index.dat", index);
	LOGSTATUS("Index has been recreated and stored.");
	
	char* word1[1000]; char* word2[1000]; char* result[1000];
	prepList(word1, 1000); 
	prepList(word2, 1000); 
	prepList(result, 1000);
	
	findWordData("zoser", word1, index);
	findWordData("zooming", word2, index);

	intersection(word1, word2, result);
	cleanList(word1, 1000);
	cleanList(word2, 1000);
	SHOULD_BE(strcmp(result[0], "") == 0);
	cleanList(result, 1000);
	END_TEST_CASE;
}

//  Test case2: intersection(word1, word2, result)
//  This test case calls intersection() for the condition where
//  the intersection of two lists must be performed, for which the
//  the result is stored in result. Intersection for which the result
//  should not be an empty list.

int TestInter2() {
  START_TEST_CASE;
	if (initList() != 1) {
	printf("Initialization wrong!");
	exit(1);
	}
	
	index = reloadIndex("../../data", "index.dat", index);
	LOGSTATUS("Index has been recreated and stored.");
	
	char* word1[1000]; char* word2[1000]; char* result[1000];
	prepList(word1, 1000); 
	prepList(word2, 1000); 
	prepList(result, 1000);
	
	findWordData("andrew", word1, index);
	findWordData("campbell", word2, index);

	intersection(word1, word2, result);
	cleanList(word1, 1000);
	cleanList(word2, 1000);
	SHOULD_BE(strcmp(result[0], "") != 0);
	cleanList(result, 1000);
	END_TEST_CASE;
}

//  Test case: unionOf(list1, list2, result)
//  This test case calls unionOf() for the condition where
//  the union of two lists must be performed, for which the
//  the result is stored in result.

int TestUnion() {
  START_TEST_CASE;
	if (initList() != 1) {
	printf("Initialization wrong!");
	exit(1);
	}
	
	index = reloadIndex("../../data", "index.dat", index);
	LOGSTATUS("Index has been recreated and stored.");
	
	char* word1[1000]; char* word2[1000]; char* result[1000];
	prepList(word1, 1000); 
	prepList(word2, 1000); 
	prepList(result, 1000);
	
	findWordData("andrew", word1, index);
	findWordData("campbell", word2, index);

	unionOf(word1, word2, result);
	cleanList(word1, 1000);
	cleanList(word2, 1000);
	SHOULD_BE(strcmp(result[0], "1 2") == 0);
	SHOULD_BE(strcmp(result[1], "2 4") == 0);
	cleanList(result, 1000);
	END_TEST_CASE;
}

//  Test case:checkArgs:
//  This test case checkArgs() check to make sure that the items were processed
//  succesfully.

int TestCheckArgs() {
  START_TEST_CASE;
	if (initList() != 1) {
	printf("Initialization wrong!");
	exit(1);
	}
	
	index = reloadIndex("../../data", "index.dat", index);
	LOGSTATUS("Index has been recreated and stored.");
	
	char input[1000] = "andrew campbell dartmouth";
	NormalizeWord(input);
	removeNewLines(input);
	
	char* args[1000];
	formList(args, input);
	
	int length = getLength(args);
	int status;
	
	status = checkArgs(length, "../../data", args, index);
	SHOULD_BE(status == 1);
	cleanList(args, length);
	
	END_TEST_CASE;
}

// This is the main test harness for the set of logic functions.
// 
//  It test the following functions:
//
//  void intersection(char** list1, char** list2, char** result);
//  void unionOf1(char** list1, char** list2, char** result);
//  int checkArgs(int length, char*dirname, char** args, INVERTED_INDEX* index);
//
//  If any of the tests fail it prints status 
//  If all tests pass it prints status.


int main(int argc, char** argv) {
	int cnt = 0;
	RUN_TEST(TestInter1, "Intersection Test case 1");
	RUN_TEST(TestInter2, "Intersection Test case 2");
	RUN_TEST(TestUnion, "Union Test Case");
	RUN_TEST(TestCheckArgs, "Check Args Test Case");
	if (!cnt) {
	  printf("All passed!\n"); return 0;
	} else {
	  printf("Some fails!\n"); return 1;
	}
}