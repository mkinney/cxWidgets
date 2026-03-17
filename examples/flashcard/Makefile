CC = g++
CX_SRC_DIR = ../../src
STATIC_LIB = $(CX_SRC_DIR)/libcxWidgets.a
INCDIR = -I$(CX_SRC_DIR)
CFLAGS = -Wall -g -std=c++17 $(INCDIR)
LIBS = -L$(CX_SRC_DIR) -lcxWidgets -lmenu -lpanel -lncurses

all: flashcard_demo

$(STATIC_LIB):
	$(MAKE) -C $(CX_SRC_DIR) libcxWidgets.a

flashcard_demo: main.cpp FlashcardApp.h $(STATIC_LIB)
	$(CC) $(CFLAGS) -o flashcard_demo main.cpp $(LIBS)

clean:
	rm -f flashcard_demo
