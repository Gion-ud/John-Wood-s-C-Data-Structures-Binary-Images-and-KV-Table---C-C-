all: main

main: main.c c_std_vector/vector.c
	gcc $^ -o $@ -I../include -O2 -Wextra -Wall
# -Werror
# -g -fsanitize=address -std=c23