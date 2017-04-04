CFLAGS = -ggdb3 -Wall -O0
CC = g++
SRC_FILE := $(wildcard *.c)
OBJ_FILE := $(SRC_FILE:.c=.o)
INCLUDES = -I./
LIBS =
all: bwtsearch



bwtsearch: main.o bwt_search.o op_file.o
	$(CC) $^ -o $@  $(LIBS)

%.d: %.c
	@set -e; rm -f $@; \
    $(CC) -M $(CFLAGS) $< > $@.$$$$; \
    sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
    rm -f $@.$$$$


.PHONY:clean
clean:
	-rm -f $(OBJ_FILE) bwtsearc *.dh

