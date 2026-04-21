all: kv.o kv_ser.o vector.o mem_arena.o mkkvdat rdkvdat

CFLAGS = -Wall -Wextra -Wno-unused -Werror -fno-exceptions -fno-strict-aliasing
CXXFLAGS = -fno-rtti
# -Wpedantic

kv.o: kv/kvtable/kv.c
	cc -c $^ -o $@ -Iinclude -Ikv $(CFLAGS)
kv_ser.o: kv/kvtable/kv_ser.c
	cc -c $^ -o $@ -Iinclude -Ikv -Iwin_include $(CFLAGS)
vector.o: vector/c_std_vector/vector.c
	cc -c $< -o $@ -Iinclude -Ivector $(CFLAGS)
mem_arena.o: include/mem/mem_arena.c
	cc -c $^ -o $@ -Iinclude $(CFLAGS)


main: main.c mem_arena.o kv.o vector.o kv_ser.o
	cc $^ -o $@ -Iinclude -Iwin_include -Ikv -Ivector -Wall -Wextra -Llib -lz -lmman $(CFLAGS)

mkkvdat: mkkvdat.cpp mem_arena.o kv.o kv_ser.o
	g++ $^ -o $@ -Iinclude -Iwin_include -Ikv -Llib -lz -lmman $(CFLAGS) $(CXXFLAGS)

rdkvdat: rdkvdat.cpp mem_arena.o kv.o kv_ser.o
	g++ $^ -o $@ -Iinclude -Iwin_include -Ikv -Llib -lz -lmman $(CFLAGS) $(CXXFLAGS)

kitt: kitt.c include/mem/mem_arena.c kv/key_index_table/kidxtbl.c
	cc $^ -o $@ -Iinclude -Iwin_include -Ikv -I. $(CFLAGS)