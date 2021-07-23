CC=cc
CFLAGS+=-Wall -Wextra -Wpedantic -Waggregate-return -Wwrite-strings -Wvla -Wfloat-equal -I../../include

BIN=../../bin
SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c, $(BIN)/%.o, $(SRCS))

all: libbst.a clean_up

.PHONY: debug
debug: CFLAGS+= -g
debug: libbst.a

libbst.a: $(OBJS)
	ar rcs $(BIN)/libbst.a $(BIN)/bst.o
	ranlib $(BIN)/libbst.a

$(OBJS): $(SRCS)
	$(CC) $(CFLAGS) -c $< -o $@

clean_up:
	rm -rf $(BIN)/*.o

.PHONY: clean
clean:
	rm -rf $(BIN)/*.o
	rm -rf $(BIN)/*.a
