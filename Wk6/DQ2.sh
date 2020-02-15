#!/bin/bash
#
# Description: This program goes to the root directory and recursively gets all files
# 	with 2 or more hard links, displaying their linked nodes to the console.
# Author: Daniel Cender
# Class: CST-221-O500 - Operating Systems Concepts
#

# Assigns output of pwd to WORKING_DIR, so we can get back to where we started
WORKING_DIR=`pwd`

echo "Current Directory (will return here after process...): "
echo $WORKING_DIR
# Go to the root directory
~

# TODO Recursively get
# grep

# `stat -f '%i' DQ2.sh` gets inode number of file

#
# find / -type f -links +2 -printf format "%i %n %p\n"

for i in $(find / -type f -links +2 -printf '%i'):
do
	echo "File contains >=2 hard links: $i"
	INODE=$(ls -i $i | awk '{print $1}')
	CONNECTED_FILES=$(find / -inum $INODE)
	echo $CONNECTED_FILES
done


cd $WORKING_DIR
