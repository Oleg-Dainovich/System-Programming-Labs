#!/bin/bash

# Размер поля
size=10 
ship_arr=(4 3 3 2 2 2 1 1 1 1)
num_arr=(0️⃣ 1️⃣ 2️⃣ 3️⃣ 4️⃣ 5️⃣ 6️⃣ 7️⃣ 8️⃣ 9️⃣)

# Инициализация полей
declare -A my_field
declare -A bot_field

set_field() {
	# Заполнение полей
	for ((i=0; i<$size; i++)); do
	  	for ((j=0; j<$size; j++)); do
	    		my_field[$i,$j]="-"
	    		bot_field[$i,$j]="-"
	  	done
	done

	ships=0
	while [[ $ships -lt $size ]]; do	
		x=$((RANDOM % size))
		y=$((RANDOM % size))
		dir=$((RANDOM % 4))
		
		x_tmp=$x
		y_tmp=$y
		
		is_possible=1
		
		for ((i=0; i<ship_arr[ships]; i++)); do
			if [[ "$x" -lt 0 || "$x" -gt 9 || "$y" -lt 0 || "$y" -gt 9 ]]; then
				is_possible=0
			fi
			
			if [[ "${my_field[$x,$y]}" == "*" ||
				"${my_field[$(($x+1)),$y]}" == "*" ||
				"${my_field[$(($x-1)),$y]}" == "*" ||
				"${my_field[$x,$(($y+1))]}" == "*" ||
				"${my_field[$(($x+1)),$(($y+1))]}" == "*" ||
				"${my_field[$(($x-1)),$(($y+1))]}" == "*" ||
				"${my_field[$x,$(($y-1))]}" == "*" ||
				"${my_field[$(($x+1)),$(($y-1))]}" == "*" ||
				"${my_field[$(($x-1)),$(($y-1))]}" == "*" ]]; then
				is_possible=0	
			fi
			
			if [[ "$dir" == 0 ]]; then
				x=$(($x+1))
			elif [[ "$dir" == 1 ]]; then
				y=$(($y+1))
			elif [[ "$dir" == 2 ]]; then
				x=$(($x-1))
			else
				y=$(($y-1))
			fi
		done
		
		x=$x_tmp
		y=$y_tmp
		
		if [[ "$is_possible" == 1 ]]; then  
			for ((i=0; i<ship_arr[ships]; i++)); do
				my_field[$x,$y]="*"
				if [[ "$dir" == 0 ]]; then
					x=$(($x+1))
				elif [[ "$dir" == 1 ]]; then
					y=$(($y+1))
				elif [[ "$dir" == 2 ]]; then
					x=$(($x-1))
				else
					y=$(($y-1))
				fi
			done
			ships=$(($ships+1))
		fi
	done
	
	bot_ships=0
	while [[ $bot_ships -lt $size ]]; do	
		x=$((RANDOM % size))
		y=$((RANDOM % size))
		dir=$((RANDOM % 4))
		
		x_tmp=$x
		y_tmp=$y
		
		is_possible=1
		
		for ((i=0; i<ship_arr[bot_ships]; i++)); do
			if [[ "$x" -lt 0 || "$x" -gt 9 || "$y" -lt 0 || "$y" -gt 9 ]]; then
				is_possible=0
			fi
			
			if [[ "${bot_field[$x,$y]}" == "*" ||
				"${bot_field[$(($x+1)),$y]}" == "*" ||
				"${bot_field[$(($x-1)),$y]}" == "*" ||
				"${bot_field[$x,$(($y+1))]}" == "*" ||
				"${bot_field[$(($x+1)),$(($y+1))]}" == "*" ||
				"${bot_field[$(($x-1)),$(($y+1))]}" == "*" ||
				"${bot_field[$x,$(($y-1))]}" == "*" ||
				"${bot_field[$(($x+1)),$(($y-1))]}" == "*" ||
				"${bot_field[$(($x-1)),$(($y-1))]}" == "*" ]]; then
				is_possible=0	
			fi
			
			if [[ "$dir" == 0 ]]; then
				x=$(($x+1))
			elif [[ "$dir" == 1 ]]; then
				y=$(($y+1))
			elif [[ "$dir" == 2 ]]; then
				x=$(($x-1))
			else
				y=$(($y-1))
			fi
		done
		
		x=$x_tmp
		y=$y_tmp
		
		if [[ "$is_possible" == 1 ]]; then  
			for ((i=0; i<ship_arr[bot_ships]; i++)); do
				bot_field[$x,$y]="*"
				if [[ "$dir" == 0 ]]; then
					x=$(($x+1))
				elif [[ "$dir" == 1 ]]; then
					y=$(($y+1))
				elif [[ "$dir" == 2 ]]; then
					x=$(($x-1))
				else
					y=$(($y-1))
				fi
			done
			bot_ships=$(($bot_ships+1))
		fi
	done
	
}

draw_field() {
	clear
	# Вывод полей
	echo "  0️⃣ 1️⃣ 2️⃣ 3️⃣ 4️⃣ 5️⃣ 6️⃣ 7️⃣ 8️⃣ 9️⃣"
	for ((i=0; i<$size; i++)); do
		echo -n "${num_arr[$i]} "
	  	for ((j=0; j<$size; j++)); do
	  		if [[ "${my_field[$i,$j]}" == "*" ]]; then
	    			echo -n "🟩"
	    		elif [[ "${my_field[$i,$j]}" == "-" ]]; then
	    			echo -n "🔲"
	    		elif [[ "${my_field[$i,$j]}" == "." ]]; then
	    			echo -n "🔘"
	    		elif [[ "${my_field[$i,$j]}" == "x" ]]; then
	    			echo -n "❌"
	    		fi
	  	done
	  	echo
	done

	echo

	echo "  0️⃣ 1️⃣ 2️⃣ 3️⃣ 4️⃣ 5️⃣ 6️⃣ 7️⃣ 8️⃣ 9️⃣"
	for ((i=0; i<$size; i++)); do
		echo -n "${num_arr[$i]} "
	  	for ((j=0; j<$size; j++)); do
	    		if [[ "${bot_field[$i,$j]}" == "." ]]; then
	    			echo -n "🔘"
	    		elif [[ "${bot_field[$i,$j]}" == "x" ]]; then
	    			echo -n "❌"
	    		else
	    			echo -n "🔲"
	    		fi
	  	done
	  	echo
	done
}

write_fields_to_file() {
	echo -n "" > player.txt
	echo -n "" > bot.txt

	for ((i=0; i<$size; i++)); do
	  	for ((j=0; j<$size; j++)); do
	  		echo -n "${my_field[$i,$j]}" >> player.txt
	  		echo -n "${bot_field[$i,$j]}" >> bot.txt
	  	done
	  	echo "" >> player.txt
	  	echo "" >> bot.txt
	done	  	
}

is_end_game() {
	is_win=1
	is_defeat=1

	for ((i=0; i<$size; i++)); do
	  	for ((j=0; j<$size; j++)); do
	  		if [[ "${bot_field[$i,$j]}" == "*" ]]; then
	  			is_win=0
	  		fi
	  		
	  		if [[ "${my_field[$i,$j]}" == "*" ]]; then
	  			is_defeat=0
	  		fi
	  	done
	done
	
	if [[ "$is_win" == 1 ]]; then
		draw_field
		echo "YOU WON!"
		exit
	fi
	if [[ "is_defeat" == 1 ]]; then
		draw_field
		echo "YOU LOSE!"
		exit
	fi
}

bot_move() {
	is_end=0
	
	while [[ "$is_end" == 0 ]]; do
		draw_field
		
		echo "Bot is making move!"
		sleep 1
		
		x_move=$((RANDOM % size))
		y_move=$((RANDOM % size))
	
		if [[ "${my_field[$x_move,$y_move]}" == "-" ]]; then
			is_end=1
			my_field[$x_move,$y_move]="."
		elif [[ "${my_field[$x_move,$y_move]}" == "*" ]]; then
			my_field[$x_move,$y_move]="x"
		fi
		
		is_end_game
	done
}

my_move() {
	is_continue=1
	
	while [[ "$is_continue" == 1 ]]; do
		is_continue=0
		
		draw_field
		
		read -p "Your turn! Enter coordinates: " y_move x_move
		
		if [[ "${bot_field[$x_move,$y_move]}" == "-" ]]; then
			bot_field[$x_move,$y_move]="."
		elif [[ "${bot_field[$x_move,$y_move]}" == "*" ]]; then
			bot_field[$x_move,$y_move]="x"
			is_continue=1
		fi
		
		is_end_game
	done
}

set_field
write_fields_to_file

while [[ 1 ]]; do
	draw_field
	my_move
	bot_move
done

