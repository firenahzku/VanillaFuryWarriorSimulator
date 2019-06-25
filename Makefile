FLAGS=-O4 -std=gnu++0x

all: simulator

objects:
	g++ -c $(FLAGS) items.cpp -o items.o
	g++ -c $(FLAGS) character.cpp -o character.o

simulator: objects
	g++ character.o items.o $(FLAGS) simulator.cpp -o simulator

clean:
	rm -f *.o simulator