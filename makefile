all: index.o registers.o inputOutput.o programaTrab.o csvFunctions.o
	gcc index.o inputOutput.o csvFunctions.o registers.o programaTrab.o -o programaTrab -std=c99 -Wall

csvFunctions.o:
	gcc -c csvFunctions.c -o csvFunctions.o
	
index.o:
	gcc -c index.c -o index.o
	
registers.o:
	gcc -c registers.c -o registers.o
	
inputOutput.o:
	gcc -c inputOutput.c -o inputOutput.o

programaTrab.o:
	gcc -c programaTrab.c -o programaTrab.o

make run:
	./programaTrab

clean:
	rm *.o programaTrab
