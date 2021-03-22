LLVM     := /Users/adishvartak/Software/LLVM/install

CXXFLAGS := `$(LLVM)/bin/llvm-config --cxxflags`
LDFLAGS  := `$(LLVM)/bin/llvm-config --ldflags --libs all`
LDFLAGS  += -lz -lcurses -lm -lxml2

SOURCES  := $(wildcard ast/src/*.cc)
SOURCES  += $(wildcard input/src/*.cc)
SOURCES  += $(wildcard lexis/src/*.cc)
SOURCES  += $(wildcard passes/src/*.cc)
SOURCES  += $(wildcard start/src/*.cc)
OBJECTS  := $(patsubst %.cc,%.o,$(SOURCES))
DEPENDS  := $(patsubst %.cc,%.d,$(SOURCES))

INCLUDES := -I$(PWD)/ast/include 
INCLUDES += -I$(PWD)/input/include 
INCLUDES += -I$(PWD)/lexis/include 
INCLUDES += -I$(PWD)/passes/include 
INCLUDES += -I$(PWD)/start/include 

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
