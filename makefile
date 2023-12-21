CXX=gcc
RM=rm -f
BIN=bin

all: snake

snake: snake.c
#	mkdir $(BIN)
	$(CXX) -O3 $< -s -o $(BIN)/snake -lSDL2 

clean:
	$(RM) snake.exe
	$(RM) snake.o
	$(RM) snake.layout
	$(RM) Makefile.win
	
