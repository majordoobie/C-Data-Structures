CC=cc
CFLAGS+=-Wall -Wextra -Wpedantic -Waggregate-return -Wwrite-strings -Wvla -Wfloat-equal -I../include -I.

BIN=../bin
OBJ=../bin

SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c, $(OBJ)/%.o, $(SRCS))
TARGET=test_exe
LIB=$(BIN)/libbst.a $(BIN)/libtree_map.a $(BIN)/libintersect.a $(BIN)/libintersect_cli.a

all: $(TARGET)

.PHONY: debug clean check profile

profile: CFLAGS+= -pg
profile: $(TARGET)

debug: CFLAGS+= -g
debug: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) $(OBJS) $(LIB) -lcheck -lm -lrt -lpthread -lsubunit $(LDLIBS) -o $(BIN)/$(TARGET)

$(OBJ)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean_up:
	rm -rf $(OBJS)

check: $(OBJS)
	$(info )
	$(info )
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) $(OBJS) $(LIB) -lcheck -lm -lrt -lpthread -lsubunit $(LDLIBS) -o $(BIN)/$(TARGET)
	$(info )
	$(info )
	$(info ************  RUNNING TEST CASES - NO VALGRIND ************)
	../bin/test_exe
check: memory

memory:
	$(info )
	$(info )
	$(info ************  RUNNING TEST CASES WITH VALGRIND ************)
	CK_FORK=no valgrind -s --leak-check=full --track-origins=yes --show-leak-kinds=all ../bin/test_exe


clean:
	rm -rf $(OBJS)
