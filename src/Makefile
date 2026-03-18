# Makefile for cxWidgets (Character window library in C++ wrapping nCurses)
#
# Copyright (c) 2005-2007 Michael H. Kinney

# Get OS name
OS = $(shell uname)

# These help us swig cxWidgets for Perl
PERL = /usr/bin/perl
SWIG = /usr/bin/swig
PERL_INCS = $(shell $(PERL) -e 'use Config; print $$Config{archlib};print "/CORE"')
SWIG_LIB = $(shell $(SWIG) -swiglib)

# If ccache exists on the system, then use it.
CCACHE = $(shell whereis ccache)
ifeq ($(CCACHE),ccache:) # $CCACHE will just be "ccache:" if it's not found
	CC = g++
else
	CC = ccache g++
endif

# Note: The /home/local/include/ncurses is for homer (to compile for AIX)
INCDIR =-I/usr/local/include -I/usr/include -I/home/local/include/ncurses -I/usr/local/include/ncurses
# Different filenames & paths & options are needed for Darwin (discovered on MacOS X
#  10.4, which is based on Darwin 8.6.1).
ifeq ($(OS),Darwin)
	LIBDIR=/usr/lib
	INSTALL_INC=/usr/include/cx
	DYNAMIC_LIB=libcxWidgets.dylib
	# It appears that the debug versions of panel & ncurses are unavailable on MacOS X
	LIBS=-lmenu -lpanel -lncurses
else
	LIBDIR=/usr/local/lib
	INSTALL_INC=/usr/local/include/cx
	DYNAMIC_LIB=libcxWidgets.so
	LIBS=-lmenu -lpanel -lncurses
endif
LIBDIR_OPT=-L$(LIBDIR) -L/usr/local/lib
LIBS_OPT=$(LIBDIR_OPT) $(LIBS)

# C++ standard for g++
#CPP_STANDARD=c++98
CPP_STANDARD=c++17

# -DWANT_TIMEOUT: Option to include/exclude the timeout handling code in cxInput.
#                 Added this option because of brain-dead SCO does not know about
#                 the sigaction struct. (But, it *should*.)
# -fPIC is used to enable position-independent code, because
#  all the object code is put into a shared library.
# But, AIX don't like -fPIC
ifeq ($(OS),AIX)
   #CFLAGS=-g -Wall -DWANT_TIMEOUT -std=$(CPP_STANDARD) -pedantic -ansi $(INCDIR) $(LIBDIR_OPT)
   #CFLAGS=-Wall -DWANT_TIMEOUT -std=$(CPP_STANDARD) -pedantic -ansi $(INCDIR) $(LIBDIR_OPT)
   CFLAGS=-Wall -DWANT_TIMEOUT -std=$(CPP_STANDARD) -pedantic $(INCDIR) $(LIBDIR_OPT)
else
   ifeq ($(OS),SCO)
      #CFLAGS=-fPIC -Wall -std=$(CPP_STANDARD) -pedantic -ansi $(INCDIR) $(LIBDIR_OPT)
	  CFLAGS=-fPIC -Wall -std=$(CPP_STANDARD) -pedantic $(INCDIR) $(LIBDIR_OPT)
   else
      #CFLAGS=-g -fPIC -DWANT_TIMEOUT -Wall -std=$(CPP_STANDARD) -pedantic -ansi $(INCDIR)
      #CFLAGS=-fPIC -DWANT_TIMEOUT -Wall -std=$(CPP_STANDARD) -pedantic -ansi $(INCDIR) 
	  CFLAGS=-fPIC -DWANT_TIMEOUT -Wall -std=$(CPP_STANDARD) -pedantic $(INCDIR) 
   endif
endif

WIDGETS_OBJS = cxApp.o cxBase.o cxButton.o cxComboBox.o cxDate.o cxDatePicker.o cxDialog.o cxFileViewer.o cxForm.o cxFunction.o cxInput.o cxMenu.o cxMessageDialog.o cxMultiForm.o cxMultiLineInput.o cxNotebook.o cxObject.o cxOpenFileDialog.o cxPanel.o cxScrolledWindow.o cxSearchPanel.o cxStringUtils.o cxTextValidator.o cxTimer.o cxValidators.o cxWidgetsException.o cxWindow.o cxBorderChars.o cxBorderStyles.o
STATIC_LIB = libcxWidgets.a

all: $(DYNAMIC_LIB) $(STATIC_LIB) testApp keys cxUnitTest test_cxStringUtils test_cxDate test_cxValidators test_sizing test_wordwrap examples

examples: $(STATIC_LIB)
	$(MAKE) -C ../examples/flashcard
	$(MAKE) -C ../examples/centipede
	$(MAKE) -C ../examples/snake

test_sizing: ../tests/test_sizing.cpp $(STATIC_LIB)
	$(CC) -o test_sizing $(CFLAGS) -I. ../tests/test_sizing.cpp $(STATIC_LIB) $(LIBS_OPT)

# Library file targets
$(DYNAMIC_LIB): $(WIDGETS_OBJS)
ifeq ($(OS),Darwin)
	$(CC) -dynamiclib -install_name $(DYNAMIC_LIB) -o $(DYNAMIC_LIB) $(CFLAGS) $(WIDGETS_OBJS) $(LIBS_OPT) -lc
else
ifeq ($(OS),AIX)
	rm -f $(STATIC_LIB)
	ar rvu $(STATIC_LIB) $(WIDGETS_OBJS)
	ranlib $(STATIC_LIB)
else
	$(CC) -shared -Wl,-soname,$(DYNAMIC_LIB) -o $(DYNAMIC_LIB) $(WIDGETS_OBJS) $(LIBS_OPT) -lc
	# Create a static library for non-AIX too..
	ar rvu $(STATIC_LIB) $(WIDGETS_OBJS)
	ranlib $(STATIC_LIB)
endif
endif

$(STATIC_LIB): $(WIDGETS_OBJS)
	ar rvu $(STATIC_LIB) $(WIDGETS_OBJS)
	ranlib $(STATIC_LIB)

install: $(DYNAMIC_LIB) $(STATIC_LIB)
	rm -rf $(INSTALL_INC)
	mkdir -p $(INSTALL_INC) 2>/dev/null
	cp *.h $(INSTALL_INC)
	cp $(STATIC_LIB) $(LIBDIR)
	cp $(DYNAMIC_LIB) $(LIBDIR)

# cxWidgets object file targets

cxApp.o: cxApp.h cxApp.cpp cxObject.o
	$(CC) -c $(CFLAGS) -o cxApp.o cxApp.cpp

cxBase.o: cxBase.h cxBase.cpp cxColors.h cxInputOptions.h cxInputTypes.h cxMessageDialogStyles.h cxWidgetsException.o cxWindow.o cxMessageDialog.o cxStringUtils.o
	$(CC) -c $(CFLAGS) -o cxBase.o cxBase.cpp

cxButton.o: cxButton.h cxButton.cpp cxWindow.o cxFunction.o
	$(CC) -c $(CFLAGS) -o cxButton.o cxButton.cpp

cxComboBox.o: cxComboBox.h cxComboBox.cpp cxMultiLineInput.o cxMenu.o cxBase.o
	$(CC) -c $(CFLAGS) -o cxComboBox.o cxComboBox.cpp

cxDate.o: cxDate.h cxDate.cpp
	$(CC) -c $(CFLAGS) -o cxDate.o cxDate.cpp

cxDatePicker.o: cxDatePicker.h cxDatePicker.cpp cxWindow.o cxButton.o cxComboBox.o cxInput.o cxDate.o
	$(CC) -c $(CFLAGS) -o cxDatePicker.o cxDatePicker.cpp

cxDialog.o: cxDialog.h cxDialog.cpp cxWindow.o
	$(CC) -c $(CFLAGS) -o cxDialog.o cxDialog.cpp

cxOpenFileDialog.o: cxOpenFileDialog.h cxOpenFileDialog.cpp cxWindow.o cxInput.o
	$(CC) -c $(CFLAGS) -o cxOpenFileDialog.o cxOpenFileDialog.cpp

cxFileViewer.o: cxFileViewer.h cxFileViewer.cpp cxScrolledWindow.o cxBase.o cxStringUtils.o
	$(CC) -c $(CFLAGS) -o cxFileViewer.o cxFileViewer.cpp

cxForm.o: cxForm.h cxForm.cpp cxWindow.o cxMultiLineInput.o cxComboBox.o cxMenu.o cxBase.o
	$(CC) -c $(CFLAGS) -o cxForm.o cxForm.cpp

#cxFrame.o: cxFrame.h

cxFunction.o: cxFunction.h cxFunction.cpp
	$(CC) -c $(CFLAGS) -o cxFunction.o cxFunction.cpp

cxInput.o: cxInput.h cxInput.cpp cxInputOptions.h cxColors.h cxWindow.o cxFunction.o cxTextValidator.o cxStringUtils.o cxBase.o
	$(CC) -c $(CFLAGS) -o cxInput.o cxInput.cpp

cxMenu.o: cxMenu.h cxMenu.cpp cxMenuItemType.h cxWindow.o cxFunction.o cxBase.o cxMultiLineInput.o cxStringUtils.o
	$(CC) -c $(CFLAGS) -o cxMenu.o cxMenu.cpp

cxMessageDialog.o: cxMessageDialog.h cxMessageDialog.cpp cxMessageDialogStyles.h cxDialog.o cxButton.o
	$(CC) -c $(CFLAGS) -o cxMessageDialog.o cxMessageDialog.cpp

cxMultiForm.o: cxMultiForm.h cxMultiForm.cpp cxForm.o cxFunction.o cxBase.o
	$(CC) -c $(CFLAGS) -o cxMultiForm.o cxMultiForm.cpp

cxMultiLineInput.o: cxMultiLineInput.h cxMultiLineInput.cpp cxInputTypes.h cxWindow.o cxInput.o cxFunction.o cxBase.o cxStringUtils.o cxValidators.o
	$(CC) -c $(CFLAGS) -o cxMultiLineInput.o cxMultiLineInput.cpp

cxNotebook.o: cxNotebook.h cxNotebook.cpp cxPanel.o cxMenu.o cxBase.o cxStringUtils.o
	$(CC) -c $(CFLAGS) -o cxNotebook.o cxNotebook.cpp

cxObject.o: cxObject.h cxObject.cpp cxWidgetsException.o
	$(CC) -c $(CFLAGS) -o cxObject.o cxObject.cpp

cxPanel.o: cxPanel.h cxPanel.cpp cxWindow.o cxButton.o cxForm.o cxMultiForm.o
	$(CC) -c $(CFLAGS) -o cxPanel.o cxPanel.cpp

cxScrolledWindow.o: cxScrolledWindow.h cxScrolledWindow.cpp cxWindow.o cxFunction.o cxMultiLineInput.o cxStringUtils.o cxBase.o cxValidators.o
	$(CC) -c $(CFLAGS) -o cxScrolledWindow.o cxScrolledWindow.cpp

cxSearchPanel.o: cxSearchPanel.h cxSearchPanel.cpp cxPanel.o cxForm.o cxMenu.o
	$(CC) -c $(CFLAGS) -o cxSearchPanel.o cxSearchPanel.cpp

cxStringUtils.o: cxStringUtils.h cxStringUtils.cpp cxReturnCodes.h cxKeyDefines.h cxMiscDefines.h cxInputOptions.h cxBorderStyles.h
	$(CC) -c $(CFLAGS) -o cxStringUtils.o cxStringUtils.cpp

cxTextValidator.o: cxTextValidator.h cxTextValidator.cpp cxObject.o
	$(CC) -c $(CFLAGS) -o cxTextValidator.o cxTextValidator.cpp

cxTimer.o: cxTimer.h cxTimer.cpp cxFunction.o
	$(CC) -c $(CFLAGS) -o cxTimer.o cxTimer.cpp

cxUnitTest: cxUnitTest.cpp $(WIDGETS_OBJS)
	$(CC) -o cxUnitTest $(CFLAGS) cxUnitTest.cpp $(WIDGETS_OBJS) $(LIBS_OPT)

cxValidators.o: cxValidators.h cxValidators.cpp
	$(CC) -c $(CFLAGS) -o cxValidators.o cxValidators.cpp

cxWidgetsException.o: cxWidgetsException.h cxWidgetsException.cpp
	$(CC) -c $(CFLAGS) -o cxWidgetsException.o cxWidgetsException.cpp

cxWindow.o: cxWindow.h cxWindow.cpp cxObject.o cxWidgetsException.o cxFunction.o cxStringUtils.o
	$(CC) -c $(CFLAGS) -o cxWindow.o cxWindow.cpp

floatingPtInputWithRightLabel.o: floatingPtInputWithRightLabel.h floatingPtInputWithRightLabel.cpp cxMultiLineInput.o
	$(CC) -c $(CFLAGS) -o floatingPtInputWithRightLabel.o floatingPtInputWithRightLabel.cpp

cxBorderChars.o: cxBorderChars.h cxBorderChars.cpp
	$(CC) -c $(CFLAGS) -o cxBorderChars.o cxBorderChars.cpp

cxBorderStyles.o: cxBorderStyles.h cxBorderStyles.cpp
	$(CC) -c $(CFLAGS) -o cxBorderStyles.o cxBorderStyles.cpp

# Programs

keys: keys.cpp $(STATIC_LIB)
	$(CC) -o keys $(CFLAGS) keys.cpp $(STATIC_LIB) $(LIBS_OPT)

testApp: testApp.cpp $(STATIC_LIB) floatingPtInputWithRightLabel.o
	$(CC) -o testApp $(CFLAGS) testApp.cpp floatingPtInputWithRightLabel.o $(STATIC_LIB) $(LIBS_OPT)

test1: test1.cpp $(STATIC_LIB)
	$(CC) -o test1 $(CFLAGS) test1.cpp $(STATIC_LIB) $(LIBS_OPT)

test_cxStringUtils: test_cxStringUtils.cpp $(WIDGETS_OBJS)
	$(CC) -o test_cxStringUtils $(CFLAGS) test_cxStringUtils.cpp $(WIDGETS_OBJS) $(LIBS_OPT)

test_cxDate: test_cxDate.cpp $(WIDGETS_OBJS)
	$(CC) -o test_cxDate $(CFLAGS) test_cxDate.cpp $(WIDGETS_OBJS) $(LIBS_OPT)

test_cxValidators: test_cxValidators.cpp $(WIDGETS_OBJS)
	$(CC) -o test_cxValidators $(CFLAGS) test_cxValidators.cpp $(WIDGETS_OBJS) $(LIBS_OPT)

test_wordwrap: ../tests/test_wordwrap.cpp $(STATIC_LIB)
	$(CC) -o test_wordwrap $(CFLAGS) -I. ../tests/test_wordwrap.cpp $(STATIC_LIB) $(LIBS_OPT)

alltest: test_cxStringUtils test_cxDate test_cxValidators test_wordwrap
	./test_cxStringUtils
	./test_cxDate
	./test_cxValidators
	./test_wordwrap
	@echo "Note: cxUnitTest requires ncurses tracing and might fail if not properly configured."
	# ./cxUnitTest 
	+make testcxInput

test: alltest

testcxInput:
	+@ cd ./test_cxInput; make clean; make ; bash ./test_cxInput_nav.bash

# Use this to print out a makefile var ....
#  make print-<var>  ... where <var> is replaced with the variable name like: #  make print-GEN
print-%: ; @echo $* = $($*)
printVars:
	@echo "LIBDIR_OPT:$(LIBDIR_OPT):"
	@echo "LIBDIR:$(LIBDIR):"
	@echo ":INSTALL_INC:$(INSTALL_INC)"
	@echo "CC:$(CC):"
	@echo "DYNAMIC_LIB:$(DYNAMIC_LIB):"
	@echo "CFLAGS:$(CFLAGS):"
	@echo "DEPDIR:$(DEPDIR):"

# Targets for swigging cxWidgets for Perl
swig: cxWidgets_wrap.o
	g++ -shared cxWidgets_wrap.o -o cxWidgets.so $(INCDIR) $(LIBDIR)

cxWidgets_wrap.o:   cxWidgets_wrap.cxx
	g++ -c cxWidgets_wrap.cxx -I$(PERL_INCS) $(INCDIR)

cxWidgets_wrap.cxx: $(CXH)
	$(SWIG) -perl -c++ -I$(SWIG_LIB)/perl5 cxWidgets.i

# Cleanup
clean:
	rm -f *.o *.a *.so *.dylib $(STATIC_LIB) $(DYNAMIC_LIB) testApp cxUnitTest test_cxStringUtils test_cxDate test_cxValidators test_wordwrap test1 testcxInput keys trace test_sizing
	$(MAKE) -C ../examples/flashcard clean
	$(MAKE) -C ../examples/centipede clean
	$(MAKE) -C ../examples/snake clean

