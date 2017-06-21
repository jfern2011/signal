# Remember to place this makefile in the src directory!
# Assign the compiler that we want to use:
CC=g++

# Include directory:
IDIR=../

# Object file directory:
ODIR=obj

# Library path:
LDIR=../lib

# Libraries you want to include (e.g. libmath):
LIB=-lm

# Assign the options we'll pass to the compiler:
CFLAGS=-c -Wall -I$(IDIR)

# Header dependencies:
_DEPS = factorial.h
# Pattern substitution:
DEPS = $(patsubst %, $(IDIR)/%, $(_DEPS))

# Object files:
_OBJ = main.o factorial.o hello.o
OBJ = $(patsubst %, $(ODIR)/%, $(_OBJ))

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: hello

hello: $(OBJ)
	$(CC) $^ ($CFLAGS) -o $@

main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp

factorial.o: factorial.cpp
	$(CC) $(CFLAGS) factorial.cpp

hello.o: hello.cpp
	$(CC) $CFLAGS hello.cpp

clean:
	rm *.o hello

# This target is always out-of-date
.PHONY: clean++

clean++:
	rm -f $(ODIR)/*.o *~ core $(IDIR)/*~
	echo clean++: all clean!