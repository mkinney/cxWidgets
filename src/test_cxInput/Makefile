CC = g++
INCDIR=-I/usr/local/include -I/usr/include -I/usr/local/include/cx -I..
LIBDIR=-L/usr/local/lib -L/usr/lib -L..
CFLAGS=-Wall $(INCDIR) $(LIBDIR)
LIBS=-L.. -lcxWidgets -lpanel -lncurses -lz

all: test_cxInput 

test_cxInput: test_cxInput.cpp
	$(CC) -o test_cxInput $(CFLAGS) test_cxInput.cpp -L.. $(LIBS)
	install_name_tool -change libcxWidgets.dylib @executable_path/../libcxWidgets.dylib test_cxInput

clean:
	rm -f *.o test_cxInput 
