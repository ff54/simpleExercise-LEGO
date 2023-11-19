CC=g++
FLAGS=-std=c++17 -pthread -o
FILENAME=DemoApp

DemoApp:
	$(CC) $(FLAGS) $(FILENAME) deviceManager.cc demo.cc

all:
	$(CC) $(FLAGS) $(FILENAME) deviceManager.cc demo.cc

clean:
	rm $(FILENAME)
