CC = gcc
CFLAGS = -Wall -g
SRC = data_seg.c memorymanager.c parser.c hachage.c exercice5.c exercice6.c exercice7.c exercice8.c

HEADERS = data_seg.h memorymanager.h parser.h hachage.h exercice5.h exercice6.h exercice7.h exercice8.h
TESTS = test_ex1 test_ex2 test_ex4 test_ex5 test_ex7

all: $(TESTS)

$(TESTS): %: %.c $(SRC) $(HEADERS)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f *.o $(TESTS)
