EXAMPLES = bin/parser bin/memtest bin/testlist
OS=$(shell uname)

CFLAGS = -fPIC -O3 -g3 -Wall -std=gnu99
all : $(EXAMPLES) libkiwi.so

%.leg.tmp.c: %.leg
	leg -o $(@) $(<)

%.o: %.c
	$(CC) $(CFLAGS) -c $(<) -o $(@)

LIB_SOURCES=src/bstrlib.c src/list.c src/content.c src/io.c src/parse.c src/stack.c
LEG_SOURCES=src/syntax.leg
LIB_SOURCES+=$(LEG_SOURCES:=.tmp.c)

BIN_SOURCES:= src/parser.c src/testlist.c src/memtest.c

LIB_OBJECTS:=$(LIB_SOURCES:.c=.o)
BIN_OBJECTS:=$(BIN_SOURCES:.c=.o)

libkiwi.so: $(LIB_OBJECTS)
ifeq ($(OS), Darwin)
	$(CC) $(CFLAGS) -dynamiclib -shared -o $(@) $(LIB_OBJECTS)
else
	$(CC) $(CFLAGS) -shared -o  $(@) $(LIB_OBJECTS)
endif

bin/%: src/%.o $(LIB_OBJECTS)
	mkdir -p bin
	$(CC) $(CFLAGS) -o $(@) $(^)

clean:
	$(RM) -r bin libkiwi.so
	$(RM) $(LIB_OBJECTS) $(LEG_SOURCES:=.tmp.c) $(BIN_OBJECTS)

.DELETE_ON_ERROR: