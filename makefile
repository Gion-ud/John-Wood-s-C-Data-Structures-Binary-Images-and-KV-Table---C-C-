all: kv.o kv_ser.o vector.o mem_arena.o mkkvdat rdkvdat

kv.o: kv/kvtable/kv.c
	cc -c $^ -o $@ -Iinclude -Ikv -Wall -Wextra -Werror # -Wpedantic
kv_ser.o: kv/kvtable/kv_ser.c
	cc -c $^ -o $@ -Iinclude -Ikv -Iwin_include -Wall -Wextra -Werror # -Wpedantic
vector.o: vector/c_std_vector/vector.c
	cc -c $< -o $@ -Iinclude -Ivector -Wall -Wextra # -Werror # -Wpedantic
mem_arena.o: include/mem/mem_arena.c
	cc -c $^ -o $@ -Iinclude -Wall -Wextra -Werror # -Wpedantic


main: main.c mem_arena.o kv.o vector.o kv_ser.o
	cc $^ -o $@ -Iinclude -Iwin_include -Ikv -Ivector -Wall -Wextra -Llib -lz -lmman #-Werror# -Wpedantic

mkkvdat: mkkvdat.cpp mem_arena.o kv.o kv_ser.o
	g++ $^ -o $@ -Iinclude -Iwin_include -Ikv -Wall -Wextra -Llib -lz -lmman -fno-exceptions -fno-rtti #-Werror# -Wpedantic

rdkvdat: rdkvdat.cpp mem_arena.o kv.o kv_ser.o
	g++ $^ -o $@ -Iinclude -Iwin_include -Ikv -Wall -Wextra -Llib -lz -lmman -fno-exceptions -fno-rtti #-Werror# -Wpedantic