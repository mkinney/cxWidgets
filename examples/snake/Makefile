CC = g++
CX_SRC_DIR = ../../src
STATIC_LIB = $(CX_SRC_DIR)/libcxWidgets.a
INCDIR = -I$(CX_SRC_DIR)
CFLAGS = -Wall -g -std=c++17 $(INCDIR)
LIBS = -L$(CX_SRC_DIR) -lcxWidgets -lmenu -lpanel -lncurses

all: snake_demo

$(STATIC_LIB):
	$(MAKE) -C $(CX_SRC_DIR) libcxWidgets.a

snake_demo: main.cpp SnakeGame.h $(STATIC_LIB)
	$(CC) $(CFLAGS) -o snake_demo main.cpp $(STATIC_LIB) -lmenu -lpanel -lncurses

clean:
	rm -f snake_demo
