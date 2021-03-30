.PHONY: all clean

all: factorial

clean:
	$(RM) *.o

printInt.o: printInt.c Makefile
	clang -c printInt.c

printChar.o: printChar.c Makefile
	clang -c printChar.c

factorial.o: factorial.avl Makefile
	../../dimple factorial.avl factorial.o

factorial: printInt.o printChar.o factorial.o
	clang -o factorial printInt.o printChar.o factorial.o
