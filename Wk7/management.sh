#!/bin/bash

#
# Desc: A script that adds users to a Linux operating system and user group
#	 using a file as input. Please execute with 'sudo'
# Author: Daniel Cender
# Date: Feb 22, 2020
#
#
# Key Resources:
# 	- Parsing text files: https://stackoverflow.com/questions/37474694/parsing-lines-in-text-in-bash-script
#		- Encrypting passcode with openssl: https://stackoverflow.com/questions/10106771/encrypt-a-string-using-openssl-command-line/38581705
#		- Hashing an SHA-512 password with openssl: https://unix.stackexchange.com/a/542835
# 	- Creating users with hashed password: https://www.techrepublic.com/article/how-to-create-users-and-groups-in-linux-from-the-command-line/
# 	- Passwords hashed with: openssl passwd -1 -salt secret password1


# Parse out three command-line arguments
userFile=$1
groupName=$2
opFlag=$3

# Measures nbr of arguments in commands passed into script
# If commnads are
if [ $# -ne 3 ]; then
    echo "Your command line contains $# arguments."
		echo "Please pass in the 3 required args: (user filename, group name, operation flag)!"
else
	# Operate on users as specified

	# Check if user group already exists
	if grep $group /etc/group
    then
         echo "User group exists"
    else
         echo "Creating user group"
				 groupadd $groupName
    fi

	# Parse out input file
	cat "${userFile}" | while IFS=" " read username password
	do
    echo "user='$username' will now be created if they don't exist already..."
    # do something
		cat /etc/passwd | grep ${username} >/dev/null 2>&1
		if [ $? -eq 0 ] ; then
    	echo "$username Already Exists!"
			usermod -a -G $groupName $username
		else
    	echo "User Not Found"
			# create user with home directory and group assignment
			useradd -m -G $groupName -p $password $username
		fi
	done
	echo "*** User management completed! ***"
fi
