
all: build

build: 
	gcc -Wall src/main.c src/dpll.c src/formula.c src/parser.c -Isrc -o solver

run:
	./solver

clean:
	rm solver
