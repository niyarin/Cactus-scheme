CC=gcc
PROGRAM = bin/cactus-scheme

all:
	mkdir -p bin
	$(CC) src/main.c src/object.c src/pair.c src/symbol.c src/c_interface.c -o $(PROGRAM) -g

clean:
	rm -rf bin
