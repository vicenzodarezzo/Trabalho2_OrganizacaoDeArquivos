DEBUG = -g3 -fsanitize=address -Wall
FLAGS = -std=c99

TEST_CASE = 4.in

OBJECTS = bTree_index.o registers.o inputOutput.o overflow_management.o csv_functions.o main.o
BIN = main

all: $(OBJECTS)
	gcc $(FLAGS) $(OBJECTS) -o $(BIN)

csv_functions.o:
	gcc $(FLAGS) -c src/csv_functions.c
	
bTree_index.o:
	gcc $(FLAGS) -c src/bTree_index.c
	
overflow_management.o:
	gcc $(FLAGS) -c src/overflow_management.c
	
registers.o:
	gcc $(FLAGS) -c src/registers.c
	
inputOutput.o:
	gcc $(FLAGS) -c src/inputOutput.c

main.o:
	gcc $(FLAGS) -c main.c

run:
	./$(BIN)

clean:
	rm *.o $(BIN)

run_test:
	make run < $(TEST_CASE)

clean_test:
	rm *.in *.out *.bin
