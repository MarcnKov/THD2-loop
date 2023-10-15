#!/bin/bash

passed=true
for line_test in `cat tests_list.txt`
do
	c_array=$(python3 image_to_datastream.py "$line_test")
	cp main_for_test.c temp.c
	sed -i 's/carraytoreplacehere/'"$c_array"'/g' temp.c
	result=$(gcc temp.c ../../Common/process_image.c -o tests && ./tests)
	sleep 0.2 # to be sure expected.txt is created
	expected=$(cat expected.txt)
	
	if [ "$result" == "$expected" ]
	then
		echo -e $line_test': \033[01;92mOK\033[0m'
	else
		passed=false
		echo -e $line_test': \033[01;91mFAIL, image results: '$result'\033[0m'
	fi
done
rm temp.c

if [ $passed = true ]
then
	echo -e '\033[01;92mTests passed successfully.\033[0m'
else
	echo -e '\033[01;91mAt least one test failed.\033[0m'
fi
