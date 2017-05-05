# CPPFLAGS := -Wall -O3 -pg  -ggdb3
CPPFLAGS := -m32 -pg -Wall -O3
# CPPFLAGS := -ggdb3 -Wall -O3
CC := g++
SRC_FILE := $(wildcard *.cpp)
OBJ_FILE := $(SRC_FILE:.cpp=.o)
INCLUDES := -I./
LIBS =
all: bwtsearch


# %.o: %.cpp
# 	$(CC)  $(CPPFLAGS) $(INCLUDES) -c $< -o $@
-include $(SRC_FILE:.cpp=.d)
%.d: %.cpp
	@set -e; rm -f $@; \
    $(CC) -MM $(CPPFLAGS)  $< > $@.$$$$; \
    sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
    rm -f $@.$$$$

bwtsearch: $(OBJ_FILE)
	$(CC) $^ -o $@  -m32 -pg  $(LIBS)

.PHONY:clean
clean:
	-rm -f $(OBJ_FILE) bwtsearch *.d *.o




