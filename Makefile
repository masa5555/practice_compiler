CFLAGS=-std=c11 -fstack-protector -g -static 
SRCS=$(wildcard src/*.c)
OBJS=$(SRCS:src.c=.o)

9cc: $(OBJS)
		$(CC) -o 9cc $(OBJS) $(LDFLAGS)

$(OBJS): src/9cc.h

test: 9cc
		./test.sh

clean:
		rm -f 9cc *.o *~ exe*

.PHONY: test clean