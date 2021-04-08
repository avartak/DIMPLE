.PHONY: all clean

all: flip

clean:
	$(RM) *.o

printInt.o: printInt.c Makefile
	clang -c printInt.c

printChar.o: printChar.c Makefile
	clang -c printChar.c

flip.o: flip.dmp Makefile
	../../dimple flip.dmp flip.o

flip: printInt.o printChar.o flip.o
	clang -o flip printInt.o printChar.o flip.o
