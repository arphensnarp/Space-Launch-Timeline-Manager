CC = gcc
CPPFLAGS = -Iinclude
CFLAGS = -std=c11 -Wall -Wextra -Werror -O2 -pipe
LDFLAGS =
OBJ = src/main.o src/list.o src/csv.o src/date.o src/util.o

all: sltm

sltm: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LDFLAGS)

clean:
	rm -f $(OBJ) sltm

.PHONY: all clean
