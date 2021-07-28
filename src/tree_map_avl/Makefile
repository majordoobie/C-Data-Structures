CC=cc
CFLAGS+=-Wall -Wextra -Wpedantic -Waggregate-return -Wwrite-strings -Wvla -Wfloat-equal -I../../include

BIN=../../bin
SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c, $(BIN)/%.o, $(SRCS))

all: libtree_map.a clean_up

.PHONY: debug
debug: CFLAGS+= -g
debug: libtree_map.a

libtree_map.a: $(OBJS)
	ar rcs $(BIN)/libtree_map.a $(BIN)/tree_map_avl.o
	ranlib $(BIN)/libtree_map.a

$(OBJS): $(SRCS)
	$(CC) $(CFLAGS) -c $< -o $@

clean_up:
	rm -rf $(BIN)/*.o

.PHONY: clean
clean:
	rm -rf $(BIN)/*.o
	rm -rf $(BIN)/*.a
