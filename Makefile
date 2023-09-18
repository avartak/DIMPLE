LLVM     := /mnt/c/Users/adishvar/Work/Software/LLVM/Install

CXXFLAGS := `$(LLVM)/bin/llvm-config --cxxflags`
LDFLAGS  := `$(LLVM)/bin/llvm-config --ldflags --libs all`
LDFLAGS  += -lz -lcurses -lm -lxml2 -ldl -lpthread

SOURCES  := $(wildcard src/*/*.cc)
OBJECTS  := $(patsubst %.cc,%.o,$(SOURCES))
DEPENDS  := $(patsubst %.cc,%.d,$(SOURCES))

INCLUDES := -I$(PWD)/include 

CC       := clang++

.PHONY: all clean

all: dimple

clean:
	$(RM) $(OBJECTS) $(DEPENDS)

%.o: %.cc Makefile
	$(CC) -O3 -c -MMD -MP $(CXXFLAGS) $(INCLUDES) -o $@ $<

dimple: $(OBJECTS)
	$(CC) -O3 -o $@ $(OBJECTS) $(LDFLAGS)

-include $(DEPENDS)
