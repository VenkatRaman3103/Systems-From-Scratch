CC = gcc
CFLAGS = -Wall -Wextra -std=c11
TARGET = build/mygit

SRC = src/main.c src/util.c src/hash.c src/tree.c src/commit.c
LIBS = -lssl -lcrypto

all:
	mkdir -p build
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LIBS)

clean:
	rm -rf build
