.PHONY: all clean

all: hello

clean:
	$(RM) *.o

printChar.o: printChar.c Makefile
	clang -c printChar.c

helloworld.o: helloworld.avl header.inc Makefile
	../../dimple helloworld.avl helloworld.o

hello: printChar.o helloworld.o
	clang -o hello printChar.o helloworld.o
