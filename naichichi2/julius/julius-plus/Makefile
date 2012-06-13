LIBSENT=../libsent
LIBJULIUS=../libjulius

CC=g++
CFLAGS=-g -O5 -msoft-float

####
#### When using system-installed libraries
####
# CPPFLAGS=`libjulius-config --cflags` `libsent-config --cflags`
# LDFLAGS=`libjulius-config --libs` `libsent-config --libs`

####
#### When using within-package libraries
####
CPPFLAGS=-std=gnu++0x -Wwrite-strings -I$(LIBJULIUS)/include -I$(LIBSENT)/include  `$(LIBSENT)/libsent-config --cflags` `$(LIBJULIUS)/libjulius-config --cflags` -msoft-float
LDFLAGS=-lboost_thread -lboost_system -L$(LIBJULIUS) `$(LIBJULIUS)/libjulius-config --libs` -L$(LIBSENT) `$(LIBSENT)/libsent-config --libs`

TARGET = julius-plus
OBJ = \
main.o \
JuliusPlus.o \
JuliusPlusRule.o \
ActionImplement.o 
############################################################

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ $(OBJ) $(LDFLAGS) 

#julius-plus: julius-simple.c
#	$(CC) $(CFLAGS) $(CPPFLAGS) -o julius-simple julius-simple.c $(LDFLAGS)

clean:
	$(RM) *.o *.bak *~ core TAGS

distclean:
	$(RM) *.o *.bak *~ core TAGS
	$(RM) julius-plus

