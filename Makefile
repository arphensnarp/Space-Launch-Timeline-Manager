# Build: GNU Make + GCC (C11). Produces an executable named 'sltm'.
CC      := gcc
CFLAGS  := -std=c11 -Wall -Wextra -Werror -O2 -pipe
LDFLAGS :=

BIN := sltm
SRC := $(wildcard src/*.c)
OBJ := $(SRC:.c=.o)

.PHONY: all clean run test valgrind

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LDFLAGS)

# Remove objects and the binary
clean:
	rm -f $(OBJ) $(BIN)

# Run the program (interactive REPL). Pass extra args as:
#   make run ARGS="..."
run: $(BIN)
	./$(BIN) $(ARGS)

valgrind: $(BIN)
	valgrind --leak-check=yes ./$(BIN)

# (Optional) If you later add shell tests in tests/test*.sh,
# this will discovery-run them:
test: $(BIN)
	@set -e; \
	for t in $(wildcard tests/test*.sh); do \
	  echo "==> $$t"; bash "$$t"; \
	done || { echo "No test scripts found under tests/"; exit 0; }
