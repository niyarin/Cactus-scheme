CC=gcc
PROGRAM = bin/cactus-scheme

all:
	mkdir -p bin
	$(CC) src/main.c src/object.c src/pair.c src/symbol.c src/c_interface.c src/reader.c src/gc.c src/ephemeron.c src/eval.c src/namespace.c src/boot.c src/util.c src/vector.c src/procedure.c src/string.c src/write.c src/identifier.c src/macro.c -o $(PROGRAM) -g

clean:
	rm -rf bin
