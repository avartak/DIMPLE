LLVM     := /mnt/c/Users/adishvar/Work/Software/LLVM/Install

CXXFLAGS := `$(LLVM)/bin/llvm-config --cxxflags`
LDFLAGS  := `$(LLVM)/bin/llvm-config --ldflags --libs all`
LDFLAGS  += -lz -lcurses -lm -lxml2 -ldl -lpthread

SOURCES  := $(wildcard start/src/*.cc)
SOURCES  += $(wildcard ast/src/*.cc)
SOURCES  += $(wildcard IR/src/*.cc)
SOURCES  += $(wildcard lexis/src/*.cc)
SOURCES  += $(wildcard parser/src/*.cc)
SOURCES  += $(wildcard translator/src/*.cc)
SOURCES  += $(wildcard backend/src/*.cc)
OBJECTS  := $(patsubst %.cc,%.o,$(SOURCES))
DEPENDS  := $(patsubst %.cc,%.d,$(SOURCES))

INCLUDES := -I$(PWD)/start/include 
INCLUDES += -I$(PWD)/ast/include 
INCLUDES += -I$(PWD)/IR/include 
INCLUDES += -I$(PWD)/lexis/include 
INCLUDES += -I$(PWD)/parser/include 
INCLUDES += -I$(PWD)/translator/include 
INCLUDES += -I$(PWD)/backend/include 

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
