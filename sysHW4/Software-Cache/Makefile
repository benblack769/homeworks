CC=gcc

CFLAGS= -c -w

VERBOSEFLAGS = -c -Wall

DEBUG = -g

SOURCES= cache.c tests.c lru.c

TSOURCES = cache_threaded.c lru_threaded.c tests_threaded.c

TOBJECTS = $(TSOURCES:.c=.o)

OBJECTS=$(SOURCES:.c=.o)

DEBUGOBJ = $(SOURCES:.c=.o)

VERBOSEOBJ = $(SOURCES:.c=.v)

cache: $(OBJECTS)
	$(CC) $^ -o $@

$(OBJECTS):
	$(CC) $(CFLAGS) $(SOURCES)

debug: $(DEBUGOBJ)
	$(CC) $^ -o $@

$(DEBUGOBJ):
	$(CC) $(CFLAGS) $(DEBUG) $(SOURCES)

verbose: $(VERBOSEOBJ)
	$(CC) $^ -o $@

$(VERBOSEOBJ):
	$(CC) $(VERBOSEFLAGS) $(SOURCES)

thread: $(TSOURCES)
	$(CC) $^ -o $@

$(TOBJECTS):
	$(CC) $(CFLAGS) $(TSOURCES)

clean:
	rm $(OBJECTS) $(TOBJECTS) cache thread debug verbose
