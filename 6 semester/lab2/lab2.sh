#!/bin/bash

input_file="input.txt"
output_file="output.txt"

text=$(<"$input_file")
regex="[\.|!|\?]\s*([a-z])"

text=${text^}

is_correct=0
while [[ $is_correct == 0 ]]; do
	is_correct=1
	if [[ $text =~ $regex ]]; then
		is_correct=0
		
		wrong_part="${BASH_REMATCH[0]}"
		#echo "$wrong_part"
		
		new_part="${BASH_REMATCH[0]}"
		new_part=${new_part::-1}
		new_part+="${BASH_REMATCH[1]^}"
		#echo "$new_part"
		
		text=${text/$wrong_part/$new_part}
	fi
done

echo "$text" > $output_file
echo "$text"

