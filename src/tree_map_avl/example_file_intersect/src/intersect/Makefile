CC=cc
CFLAGS+=-Wall -Wextra -Wpedantic -Waggregate-return -Wwrite-strings -Wvla -Wfloat-equal -I../../include

BIN=../../bin
SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c, $(BIN)/%.o, $(SRCS))
TARGET=libintersect.a

all: $(TARGET)

.PHONY: debug clean check profile

profile: CFLAGS+= -pg
profile: $(TARGET)

debug: CFLAGS+= -g
debug: $(TARGET)

$(TARGET): $(OBJS)
	ar rcs $(BIN)/$(TARGET) $(BIN)/intersect.o
	ranlib $(BIN)/$(TARGET)


$(OBJS): $(SRCS)
	$(CC) $(CFLAGS) -c $< -o $@

clean_up:
	rm -rf $(OBJS)

clean:
	rm -rf $(OBJS)
	rm -rf $(BIN)/$(TARGET)
