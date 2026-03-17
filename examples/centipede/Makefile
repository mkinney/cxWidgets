CC = g++
CX_SRC_DIR = ../../src
STATIC_LIB = $(CX_SRC_DIR)/libcxWidgets.a
INCDIR = -I$(CX_SRC_DIR)
CFLAGS = -Wall -g -std=c++17 $(INCDIR)
LIBS = -L$(CX_SRC_DIR) -lcxWidgets -lmenu -lpanel -lncurses

all: centipede_demo

$(STATIC_LIB):
	$(MAKE) -C $(CX_SRC_DIR) libcxWidgets.a

centipede_demo: main.cpp CentipedeGame.h $(STATIC_LIB)
	$(CC) $(CFLAGS) -o centipede_demo main.cpp $(STATIC_LIB) -lmenu -lpanel -lncurses

clean:
	rm -f centipede_demo
