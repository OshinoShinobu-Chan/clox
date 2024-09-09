IDIR =./include
CC=gcc
CFLAGS=-I$(IDIR) -Wall -Werror -g

SDIR=./src
ODIR=./obj
TDIR=./target
LDIR =./lib

LIBS=-lm

DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_DEPS = chunk.h common.h compiler.h debug.h memory.h object.h scanner.h table.h value.h vm.h 
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

_OBJ = chunk.o compiler.o debug.o main.o memory.o object.o scanner.o table.o value.o vm.o 

$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

clox: $(OBJ)
	$(CC) -o $(TDIR)/$@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o $(TDIR)/* *~ core $(INCDIR)/*~ 