CC=cc
CFLAGS+=-Wall -Wextra -Wpedantic -Waggregate-return -Wwrite-strings -Wvla -Wfloat-equal -I../../include -I.

BIN=../../bin
SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c, $(BIN)/%.o, $(SRCS))
TARGET=intersect

LIB=$(BIN)/libbst.a $(BIN)/libtree_map.a $(BIN)/libintersect.a $(BIN)/libintersect_cli.a
LINK_OBJ=$(BIN)/bst.o $(BIN)/intersect.o $(BIN)/arg_parser.o $(BIN)/file_parser.o

all: $(TARGET)

.PHONY: debug clean check profile

profile: CFLAGS+= -pg
profile: $(TARGET)

debug: CFLAGS+= -g
debug: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) $(OBJS) $(LINK_OBJ) $(LIB) $(LDLIBS) -o $(BIN)/$(TARGET)

$(BIN)/main.o: main.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJS)
