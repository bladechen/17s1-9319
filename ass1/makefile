CFLAGS = -ggdb3 -Wall -O3
CC = g++
SRC_FILE := $(wildcard *.cpp)
OBJ_FILE := $(SRC_FILE:.cpp=.o)
INCLUDES = -I./
LIBS =
all: rldecode rlencode

rldecode: rldecode.o rle.o op_file.o
	$(CC) $^ -o $@  $(LIBS)


rlencode: rlencode.o rle.o op_file.o
	$(CC) $^ -o $@  $(LIBS)

%.o: %.cpp
	$(CC)  $(CFLAGS) $(INCLUDES) -c $< -o $@


.PHONY:clean
clean:
	-rm -f $(OBJ_FILE) rldecode rlencode

