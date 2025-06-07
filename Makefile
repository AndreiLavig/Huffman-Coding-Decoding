CC=gcc
CFLAGS=-Wall -g
OBJ=main.o hfm.o

all: huffman

huffman: $(OBJ)
	$(CC) -o hfm $(OBJ)

main.o: main.c hfm.h
huffman.o: hfm.c hfm.h

clean:
	rm -f *.o hfm
