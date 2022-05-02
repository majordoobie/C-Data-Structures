CC=cc
CFLAGS+=-Wall -Wextra -Wpedantic -Waggregate-return -Wwrite-strings -Wvla -Wfloat-equal -I../../include

BIN=../../bin
SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c, $(BIN)/%.o, $(SRCS))
TARGET=libintersect_cli.a

all: $(TARGET)

.PHONY: debug clean check profile

profile: CFLAGS+= -pg
profile: $(TARGET)

debug: CFLAGS+= -g
debug: $(TARGET)

$(TARGET): $(OBJS)
	ar rcs $(BIN)/$(TARGET) $(BIN)/arg_parser.o $(BIN)/file_parser.o
	ranlib $(BIN)/$(TARGET)

$(BIN)/arg_parser.o: arg_parser.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN)/file_parser.o: file_parser.c
	$(CC) $(CFLAGS) -c $< -o $@

clean_up:
	rm -rf $(OBJS)

clean:
	rm -rf $(OBJS)
	rm -rf $(BIN)/$(TARGET)
