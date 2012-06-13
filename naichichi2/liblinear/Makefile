CXX ?= g++
CC ?= gcc

#FPUHINT=-march=armv7-a -mtune=cortex-a8 -mfpu=neon -mfloat-abi=softfp -mabi=aapcs-linux -fforce-addr -fomit-frame-pointer -fstrength-reduce
FPUHINT=

CFLAGS = -Wall -Wconversion -O3 -fPIC $(FPUHINT)
LIBS = blas/blas.a
SHVER = 1
#LIBS = -lblas
AR     = ar rcv
RANLIB = ranlib 

all: train predict lib

lib: linear.o tron.o blas/blas.a
	$(CXX) -shared -dynamiclib linear.o tron.o blas/blas.a -o liblinear.so.$(SHVER)
	$(AR) liblinear.a $?
	$(RANLIB) liblinear.a


train: tron.o linear.o train.c blas/blas.a
	$(CXX) $(CFLAGS) -o train train.c tron.o linear.o $(LIBS)

predict: tron.o linear.o predict.c blas/blas.a
	$(CXX) $(CFLAGS) -o predict predict.c tron.o linear.o $(LIBS)

tron.o: tron.cpp tron.h
	$(CXX) $(CFLAGS) -c -o tron.o tron.cpp

linear.o: linear.cpp linear.h
	$(CXX) $(CFLAGS) -c -o linear.o linear.cpp

blas/blas.a:
	cd blas; make OPTFLAGS='$(CFLAGS)' CC='$(CC)';

clean:
	cd blas;	make clean
	cd matlab;	make clean
	rm -f *~ tron.o linear.o train predict liblinear.so.$(SHVER)
