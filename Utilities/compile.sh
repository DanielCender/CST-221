#!/bin/sh

# Save script arguments and validate we have inputs (or could use $# -eq 0)
myprog=$1
if [ -z $myprog ]; then
	echo "Input program name to compile is required for this script."
	exit 1
fi

# Compile the program
echo "Compiling my program....." $myprog
gcc $myprog.c -o$myprog.out
if [ $? -ne 0 ]; then
	echo "Compile failed"
	exit 2
else
	echo "Done"
fi

# Run the program
echo "Running my program....." $myprog
./$myprog.out
echo "Done"
