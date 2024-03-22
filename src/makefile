CC = g++
CFLAG = -Wall
SRCPATH = ../src
BINPATH = ./bin

SRC = $(wildcard *.cpp)
TAR = $(SRC:.cpp=)

all: $(TAR)

%: %.cpp
	$(CC) $(CFLAG) $< -o $(BINPATH)/$@.o -I $(SRCPATH) -lpthread 2>/dev/null

clean:
	rm -f *.o