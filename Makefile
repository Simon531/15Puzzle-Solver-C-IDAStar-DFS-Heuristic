CC = gcc
CPPFLAGS = -g -Wall -Werror -O3 -lm

# Uncomment below if you want to use debug flags
#
#CPPFLAGS=-g

SRC = puzzle.o func_LC_LM.o
TARGET = 15puzzle

all: $(SRC)
	$(CC) -o $(TARGET) $(SRC) $(CPPFLAGS)

clean:
	rm -f $(TARGET) *.o
