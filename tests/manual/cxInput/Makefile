CC = g++
OS = $(shell uname)

# C++ standard for g++
CPP_STANDARD = c++17

INCDIR = -I/usr/local/include -I/usr/include -I/usr/local/include/ncurses -I../../../src
LIBDIR_OPT = -L../../../src
STATIC_LIB = ../../../src/libcxWidgets.a

ifeq ($(OS),Darwin)
	LIBS = -lmenu -lpanel -lncurses
else
	LIBS = -lmenu -lpanel -lncurses
endif

CFLAGS = -fPIC -DWANT_TIMEOUT -Wall -std=$(CPP_STANDARD) -pedantic $(INCDIR)

all: test_cxInput

test_cxInput: test_cxInput.cpp $(STATIC_LIB)
	$(CC) -o test_cxInput $(CFLAGS) test_cxInput.cpp $(STATIC_LIB) $(LIBDIR_OPT) $(LIBS)

clean:
	rm -f *.o test_cxInput test_cxInput.log stderr.log
