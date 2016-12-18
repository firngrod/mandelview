IDIR=firnlibs
GPP=g++
CFLAGS=-I$(IDIR) $(shell pkg-config --cflags opencv) -pthread -std=c++11 -O3

ODIR=obj
LDIR=

LIBS=-lgmpxx -lgmp -lmpfr -ljsoncpp $(shell pkg-config --libs opencv)

_DEPS=complex.hpp mandelbrot.hpp buildimage.hpp colors.hpp workerthread.hpp CyclicVar.hpp saveroutine.hpp bigfix.hpp
DEPS=$(LDEPS)

_LDEPS=
LDEPS=$(patsubst %,$(IDIR)/%,$(_LDEPS))

_OBJ=complex.o mandelbrot.o mandelview.o buildimage.o colors.o workerthread.o saveroutine.o mouseevents.o
OBJ=$(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.cpp $(DEPS) $(LDEPS)
	$(GPP) -c -o $@ $< $(CFLAGS)

mandelview: $(OBJ)
	$(GPP) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY:clean

clean:
	rm -f $(ODIR)/*.o $(ODIR)/firnlibs/*.o *~
