CC=cc
CFLAGS+=-Wall -Wextra -Wpedantic -Waggregate-return -Wwrite-strings -Wvla -Wfloat-equal -I./include -I./test
SRC=src
OBJ=bin

# Create a list of all files in src/ with .c extension
SRCS=$(wildcard $(SRC)/*.c)
# Create a list using pattern sub by replacing .c files to .o in the SRCS variable
OBJS=$(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SRCS))

# outputs
BIN=bin/ws

all: make_env $(BIN)

debug: CFLAGS += -g
debug: clean
debug: $(BIN)

make_env:
	@mkdir -p bin/

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

$(OBJ)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -r bin/*.o
