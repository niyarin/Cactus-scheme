CC=gcc
PROGRAM = bin/cactus-scheme

all:
	mkdir -p bin
	$(CC) src/main.c src/object.c -o $(PROGRAM)

clean:
	rm -rf bin
