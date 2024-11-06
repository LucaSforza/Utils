main: main.c
	mkdir -p build
	gcc -ggdb -Wall -Wextra -o build/main main.c -pthread

run-main:
	./build/main

all: main run-main