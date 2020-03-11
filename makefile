IDIR =include
CC=gcc
CFLAGS=-I$(IDIR)

ODIR=obj
LDIR =lib

LIBS=-lm

_DEPS_fir = sndutil.h fir.h soundstruct.h
DEPS_fir = $(patsubst %,$(IDIR)/%,$(_DEPS_fir))

_OBJ_fir = simple_fir.o fir.o sndutil.o
OBJ_fir = $(patsubst %,$(ODIR)/%,$(_OBJ_fir))

_DEPS_iir = sndutil.h iir.h soundstruct.h
DEPS_iir = $(patsubst %,$(IDIR)/%,$(_DEPS_iir))

_OBJ_iir = simple_iir.o iir.o sndutil.o
OBJ_iir = $(patsubst %,$(ODIR)/%,$(_OBJ_iir))


$(ODIR)/%.o: %.c $(DEPS_fir)
	$(CC) -c -o $@ $< $(CFLAGS)

simple_fir:	$(OBJ_fir)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

simple_iir:	$(OBJ_iir)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ 