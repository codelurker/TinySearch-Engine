CC      =  gcc
CFLAGS  =  -g -Wall -pedantic -std=c99
EXEC    =  query
OBJS    =  query.o logic.o hash.o util.o input.o dictionary.o
SRCS    =  query.c logic.c hash.c dictionary.c ../util/util.c ../util/input.c dictionary.c
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
	rm -f ./query
	rm -f *.o
	rm -f *.gch
