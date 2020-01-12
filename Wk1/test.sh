#!/bin/bash

# A simple bash script that creates a tmp/ directory with
# as many files as lines in the SAMPLE variable.
# Each file contains one line of text, capitalized.


SAMPLE='lorem ipsum
anakin skywalker
obi-wan
windu
United States
Friendship
Leroy Jenkins
Solomon Mines
Wine
friends forever
college days
summer sweat
workout time'

DEST=/temp

COUNT=1
# if file exists
if [ -r temp.txt ]
then
	echo "Temp file found, re-creating now!"
	rm temp.txt
else
	echo "No temp file found, creating now!"
fi

touch temp.txt
mkdir tmp

echo "$SAMPLE" > temp.txt

cat temp.txt | while read line
do
	FILE_NAME="tmp/tmp${COUNT}.txt"
	echo $FILE_NAME
	touch $FILE_NAME
	echo "$line" | tr '[:lower:]' '[:upper:]' > $FILE_NAME
	((COUNT++))
done

# cleanup temporary holding file
rm temp.txt
