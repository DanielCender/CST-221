#!/bin/bash

#
# Desc: A script that checks the password inputted for security vulnerabilities.
# Author: Daniel Cender
# Date: Feb 21, 2020
#

rxContainsDigit="(\d)."
# Even though \W means Non-alphanumeric,
#	our quest is to check for one of these specific non-alphanumeric chars
rxContainsNonAlphaNumeric="[@#\$%|&\*\+-=]."

pass=$1

# Measures nbr of arguments in commands passed into script
# If commnads are
if [ $# -ne 1 ]; then
    echo "Your command line contains $# arguments."
		echo "Please pass in single password argument as a string!"
else
	# Evaluate password
	echo "Passed in: $pass"
	size=${#pass}
	if [ $size -lt 8 ]; then
		echo "Your password is not long enough! It's only $size characters long!"
		exit
	fi
	# Check for digits in password
	if ! [[ $pass =~ $rxContainsDigit ]]; then
	 echo "Your password needs to contain at least 1 digit (0-9) character!"
	 exit
	fi
	# Check for Non-alphanumeric chars
	if ! [[ $pass =~ $rxContainsNonAlphaNumeric ]]; then
	 echo "Your password needs to contain at least 1 Non-alphanumeric character!"
	 echo "Acceptable characters are any of: (@, #, $, %, &, *, +, -, =)"
	 exit
	fi

	echo "*** Congratulations! Your password is secure! ***"
fi
