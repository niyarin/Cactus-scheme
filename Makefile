CC=gcc
PROGRAM = bin/cactus-scheme

all:
	mkdir -p bin
	$(CC) src/main.c src/object.c src/pair.c src/symbol.c src/c_interface.c src/reader.c src/gc.c src/ephemeron.c -o $(PROGRAM) -g

clean:
	rm -rf bin
