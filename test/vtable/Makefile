.PHONY: all clean

all: hello

clean:
	$(RM) *.o

printChar.o: printChar.c Makefile
	clang -c printChar.c

polymorph.o: polymorph.dmp Makefile
	../../dimple polymorph.dmp polymorph.o

hello: printChar.o polymorph.o
	clang -o polymorph printChar.o polymorph.o
