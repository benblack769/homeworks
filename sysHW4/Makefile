
CC = gcc
CFLAGS  =  -w -O2 -std=c99
LINK_FLAGS = -lm

SYSHW3 = sysHW3/
HIO = Hash-it-Out/
HIO2 = hash_it_out/c_code/
HW3_442 = 442-HW-3/
HW3_S = hw3/
SOFT_C = Software-Cache/
MATH442 = MATH442_HW3/

all: syshw3 HIO HIO2 HW3_442 HW3_S SOFT_C MATH442 clean_obj

mytest.o: #test.c create_cache_wrapper.c
	$(CC) $(CFLAGS) -c test.c

clean_obj:
	rm *.o


syshw3: sysHW3.o mytest.o syshw3_wrapper.o
	$(CC) -o exe7 hash_cache.o helper.o lru_replacement.o test.o syshw3_wrapper.o $(LINK_FLAGS)

sysHW3.o: $(SYSHW3)hash_cache.c $(SYSHW3)helper.c $(SYSHW3)lru_replacement.c
	$(CC) $(CFLAGS) -c $(SYSHW3)hash_cache.c $(SYSHW3)helper.c $(SYSHW3)lru_replacement.c

syshw3_wrapper.o: create_cache_wrapper.c
	$(CC) $(CFLAGS) -o syshw3_wrapper.o -D=SYSHW3 -c create_cache_wrapper.c


HW3_442: HW3_442.o mytest.o HW3_442_wrapper.o
	$(CC) -o exe1 cache.o test.o HW3_442_wrapper.o $(LINK_FLAGS)

HW3_442.o: $(HW3_442)cache.c
	$(CC) $(CFLAGS) -c $(HW3_442)cache.c

HW3_442_wrapper.o: create_cache_wrapper.c
	$(CC) $(CFLAGS) -o HW3_442_wrapper.o -D=HW3_442 -c create_cache_wrapper.c


HIO: HIO.o mytest.o HIO_wrapper.o
	$(CC) -o exe4 cache.o lru.o test.o HIO_wrapper.o $(LINK_FLAGS)

HIO.o: $(HIO)cache.c $(HIO)lru.c
	$(CC) $(CFLAGS) -c $(HIO)cache.c $(HIO)lru.c

HIO_wrapper.o: create_cache_wrapper.c
	$(CC) $(CFLAGS) -o HIO_wrapper.o -D=HIO -c create_cache_wrapper.c


MATH442: MATH442.o mytest.o MATH442_wrapper.o
	$(CC) -o exe6 cache.o LRU.o hash_table.o test.o MATH442_wrapper.o $(LINK_FLAGS)

MATH442.o: $(MATH442)cache.c $(MATH442)LRU.c $(MATH442)hash_table.c
	$(CC) $(CFLAGS) -c $(MATH442)cache.c $(MATH442)LRU.c  $(MATH442)hash_table.c

MATH442_wrapper.o: create_cache_wrapper.c
	$(CC) $(CFLAGS) -o MATH442_wrapper.o -D=MATH442 -c create_cache_wrapper.c


SOFT_C: SOFT_C.o mytest.o SOFT_C_wrapper.o
	$(CC) -o exe5 cache.o lru.o test.o SOFT_C_wrapper.o $(LINK_FLAGS)

SOFT_C.o: $(SOFT_C)cache.c $(SOFT_C)lru.c
	$(CC) $(CFLAGS) -c $(SOFT_C)cache.c $(SOFT_C)lru.c

SOFT_C_wrapper.o: create_cache_wrapper.c
	$(CC) $(CFLAGS) -o SOFT_C_wrapper.o -D=SOFT_C -c create_cache_wrapper.c


HW3_S: HW3_S.o mytest.o HW3_S_wrapper.o $(LINK_FLAGS)
	$(CC) -o exe3 cache.o list.o slabs.o tools.o test.o HW3_S_wrapper.o $(LINK_FLAGS)

HW3_S.o: $(HW3_S)cache.c $(HW3_S)list.c  $(HW3_S)slabs.c  $(HW3_S)tools.c
	$(CC) $(CFLAGS) -c $(HW3_S)cache.c $(HW3_S)list.c  $(HW3_S)slabs.c  $(HW3_S)tools.c

HW3_S_wrapper.o: create_cache_wrapper.c
	$(CC) $(CFLAGS) -o HW3_S_wrapper.o -D=HW3_S -c create_cache_wrapper.c



HIO2: HIO2.o mytest.o HIO2_wrapper.o
	$(CC) -o exe2 cache.o dbLL.o evict.o node.o test.o HIO2_wrapper.o $(LINK_FLAGS)

HIO2.o: $(HIO2)cache.c $(HIO2)dbLL.c $(HIO2)evict.c  $(HIO2)node.c
	$(CC) $(CFLAGS) -c $(HIO2)cache.c $(HIO2)dbLL.c $(HIO2)evict.c  $(HIO2)node.c

HIO2_wrapper.o: create_cache_wrapper.c
	$(CC) $(CFLAGS) -o HIO2_wrapper.o -D=HIO2 -c create_cache_wrapper.c
