CC = gcc
CFLAGS = -Wall -Wextra -std=c11
TARGET = build/mygit

SRC = src/main.c src/util.c

all:
	mkdir -p build
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)


