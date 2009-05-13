#!/bin/bash
#
# by: Ricky A. Melgares '11, May 3, 2009
#
# File: crawler_test.sh
# Description: This simple script file which does:
# Step 1,2,3:
#   Pass in invalid arguments and see what happens
# Step 4:
#   Test for wrong specified directory.
# Step 5:
#   Let crawler do a full crawl of depth 3 on www.cs.dartmouth.edu.

# Step 1
echo "Step 1"
echo "Wrong arguments number"
crawler www.cs.dartmouth.edu
if [ $? -ne 1 ]; then
	echo "Return value wrong!"
fi
echo "Pass Step 1"

# Step 2
echo "Step 2"
echo "Invalid URL"
crawler www.cs.dartmouth.eduuu ../../data 2
if [ $? -ne 1 ]; then
	echo "Return value wrong!"
fi
echo "Pass Step 2"

# Step 3
echo "Step 3"
echo "Invalid depth"
crawler www.cs.dartmouth.edu ../../data 5
if [ $? -ne 1 ]; then
	echo "Return value wrong!"
fi
echo "Pass Step 3"

# Step 4
echo "Step 4"
echo "Wrong directory"
crawler www.cs.dartmouth.edu ./daterrr 2
if [ $? -ne 1 ]; then
	echo "Return value wrong!"
fi
echo "Pass Step 4"

# Step 5
echo "Step 5"
echo "Real crawling job............."
crawler http://www.cs.dartmouth.edu/ ../../data 3
if [ $? -ne 1 ]; then
	echo "Return value wrong!"
fi
echo "Pass Step 5"

exit 0