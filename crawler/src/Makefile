CC      =  gcc
CFLAGS  =  -g -Wall -pedantic -std=c99
EXEC    =  crawler
OBJS    =  crawler.o html.o hash.o
SRCS    =  crawler.c ../../shared/html.c ../../shared/hash.c
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
	rm -f ./crawler
	rm -f *.o
