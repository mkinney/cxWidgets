CC = g++
CX_SRC_DIR = ../../src
STATIC_LIB = $(CX_SRC_DIR)/libcxWidgets.a
INCDIR = -I$(CX_SRC_DIR)
CFLAGS = -Wall -g -std=c++17 $(INCDIR)
LIBS = -lmenu -lpanel -lncurses

all: spreadsheet

$(STATIC_LIB):
	$(MAKE) -C $(CX_SRC_DIR) libcxWidgets.a

spreadsheet: main.cpp $(STATIC_LIB)
	$(CC) $(CFLAGS) -o spreadsheet main.cpp $(STATIC_LIB) $(LIBS)

clean:
	rm -f spreadsheet
	rm -rf spreadsheet.dSYM
