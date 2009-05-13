CC      =  gcc
CFLAGS  =  -Wall -pedantic -std=c99
EXEC    =  indexer
OBJS    =  indexer.o hash.o input.o dictionary.o
SRCS    =  indexer.c ../../shared/hash.c ../util/input.c dictionary.c
$(EXEC): $(OBJS)
	$(CC)  $(CFLAGS)  -o $(EXEC)  $(OBJS)
$(OBJS): $(SRCS)
	$(CC)  $(CFLAGS)  -c  $(SRCS)
debug: $(SRCS) 
	$(CC)  $(CFLAGS) -g -ggdb -c  $(SRCS)
	$(CC)  $(CFLAGS) -g -ggdb -o $(EXEC)  $(OBJS)
clean:
	rm -f *~
	rm -f *#
	rm -f ./indexer
	rm -f *.o
	rm -f *.gch