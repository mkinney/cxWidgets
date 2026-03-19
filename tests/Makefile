# Makefile for cxWidgets tests
#
# Copyright (c) 2026 E. Oulashin

OS = $(shell uname)
CC = g++
CPP_STANDARD = c++17

INCDIR = -I../src -I.
LIBDIR_OPT = -L../src

ifeq ($(OS),Darwin)
    LIBS = -lmenu -lpanel -lncurses
else
    LIBS = -lmenu -lpanel -lncurses
endif

CFLAGS = -Wall -Werror -std=$(CPP_STANDARD) -pedantic $(INCDIR)
STATIC_LIB = ../src/libcxWidgets.a

TEST_SRCS = test_string_utils.cpp test_date.cpp test_validators.cpp test_ui_sizing.cpp test_cx_widgets.cpp
TEST_EXES = $(TEST_SRCS:.cpp=)

all: $(TEST_EXES)

%: %.cpp $(STATIC_LIB)
	$(CC) -o $@ $(CFLAGS) $< $(STATIC_LIB) $(LIBDIR_OPT) $(LIBS)

run: all
	@for test in $(TEST_EXES); do \
		echo "Running $$test..."; \
		./$$test; \
	done

clean:
	rm -f $(TEST_EXES) *.o test_wordwrap
	$(MAKE) -C manual/cxInput clean
