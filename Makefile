IDIR =./include
CC=gcc
CFLAGS=-I$(IDIR) -Wall -Werror -g

SDIR=./src
ODIR=./obj
TDIR=./target
LDIR =./lib

LIBS=-lm

DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_DEPS = chunk.h common.h debug.h memory.h value.h 
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

_OBJ = chunk.o debug.o main.o memory.o value.o 

$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

clox: $(OBJ)
	$(CC) -o $(TDIR)/$@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o $(TDIR)/* *~ core $(INCDIR)/*~ 