all: bTree_index.o registers.o inputOutput.o main.o csv_functions.o
	gcc bTree_index.o inputOutput.o csv_functions.o registers.o main.o -o main -std=c99 -Wall

csvFunctions.o:
	gcc -c csv_functions.c -o csv_functions.o
	
bTree_index.o:
	gcc -c bTree_index.c -o bTree_index.o
	
registers.o:
	gcc -c registers.c -o registers.o
	
inputOutput.o:
	gcc -c inputOutput.c -o inputOutput.o

main.o:
	gcc -c main.c -o main.o

make run:
	./main

clean:
	rm *.o main
