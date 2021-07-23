CC=cc
CFLAGS+=-Wall -Wextra -Wpedantic -Waggregate-return -Wwrite-strings -Wvla -Wfloat-equal -I../../include -I.

BIN=../../bin
OBJ=../../bin

SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c, $(OBJ)/%.o, $(SRCS))
LIB=$(BIN)/libbst.a

all: bst_test clean_up

.PHONY: debug
debug: CFLAGS+= -g
debug: bst_test

bst_test: $(OBJS)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -g $(OBJS) $(LIB) -lcheck -lm -lrt -lpthread -lsubunit $(LDLIBS) -o $(BIN)/bst_test_exe

$(OBJ)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean_up:
	rm -rf $(BIN)/*.o

.PHONY: clean
clean:
	rm -rf $(BIN)/*.o
	rm -rf $(BIN)/*.a

