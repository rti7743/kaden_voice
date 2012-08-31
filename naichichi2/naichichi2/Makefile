CC=gcc
CXX=g++
#FPUHINT=-march=armv7-a -mtune=cortex-a8 -mfpu=neon -mfloat-abi=softfp -mabi=aapcs-linux -fforce-addr -fomit-frame-pointer -fstrength-reduce
FPUHINT=
CFLAGS=-g -Os $(FPUHINT)

CPPFLAGS= \
$(CFLAGS) \
-std=gnu++0x \
-Wwrite-strings \
-lboost_thread -lboost_system -lpthread \
-I../julius/libsent/include \
-I../julius/libjulius/include \
`../julius/libsent/libsent-config --cflags` `../julius/libjulius/libjulius-config --cflags` 

LDFLAGS= \
../lua/liblua.a \
../liblinear/liblinear.a \
../liblinear/blas/blas.a \
-L../julius/libjulius `../julius/libjulius/libjulius-config --libs` \
-L../julius/libsent `../julius/libsent/libsent-config --libs`

TARGET=naichichi2

OBJ = \
main.o \
ActionImplement.o \
ActionScriptManager.o \
Config.o \
HttpServer.o \
MainWindow.o \
Recognition_Factory.o \
Recognition_JuliusPlus.o \
Recognition_JuliusPlusRule.o \
ScriptManager.o \
ScriptRunner.o \
ScriptWebRunner.o \
WebMenu.o \
Callbackable.o \
Speak_Factory.o \
Speak_GoogleTranslate.o \
TriggerManager.o \
XLDebugUtil.o \
XLFileUtil.o \
XLException.o \
XLSocket.o \
XLHttpHeader.o \
XLHttpSocket.o \
XLStringUtil.o \
XLSoundPlay.o \
XLUrlParser.o 

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CFLAGS) $(CPPFLAGS) -o $@ $(OBJ) $(LDFLAGS)

clean:
	$(RM) *.o *.bak *~ core TAGS
	$(RM) $(TARGET)
