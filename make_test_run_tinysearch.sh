#!/bin/bash
#
# by: Ricky A. Melgares '11, May 3, 2009
#
# File: make_test_run_tinysearch.sh
# Description: This simple script file which does:
# Step 1:
#   Makes each component of the system.
# Step 2:
#   Unit test query engine. If it passed then proceed.
# Step 3:
#   Run crawler.
# Step 4:
#   Run indexer.
# Step 5:
#	Run query.

# Step 1
echo "Step 1"
echo "Building the system........."

# delete any data that might be in the data foler
rm -f data/*

# build the crawler
make clean --directory=crawler/src
make --directory=crawler/src

# build the indexer
make clean --directory=indexer/src
make --directory=indexer/src

# build the test query harness
echo "Building test harness"
make clean -f Makefile_test --directory=queryengine/src
make -f Makefile_test --directory=queryengine/src
echo "Pass step 1 - Building complete."

# build the query engine
echo "Building the query engine"
make clean --directory=queryengine/src
make --directory=queryengine/src

# Step 2
echo "Step 2"
echo "Unit testing the query engine."
cd queryengine/src
./test
if [ $? -ne 0 ]; then
	echo "Unit testing for query failed!"
	exit
fi
cd ../../
echo "Pass Step 2 - Unit testing successful"

# Step 3
echo "Step 3"
echo "Real crawling job........."
cd crawler/src
./crawler http://www.cs.dartmouth.edu/ ../../data 3
if [ $? -ne 0 ]; then
	echo "Crawler panic."
	exit
fi
cd ../../
echo "Pass Step 3 - Crawling Completed."

# Step 4
echo "Step 4"
echo "Indexing.................."
cd indexer/src
./indexer ../../data index.dat
if [ $? -ne 0 ]; then
	echo "Indexing failed."
	exit
fi
cd ../../
echo "Pass Step 4 - Indexing completed."

# Step 5
echo "Step 5"
echo "Query Engine"
cd queryengine/src
./query index.dat ../../data
