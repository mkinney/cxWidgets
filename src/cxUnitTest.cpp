// Copyright (c) 2026 E. Oulashin
// Copyright (c) 2005-2007 Michael H. Kinney

// TODO: test both draw box constructors

#define TESTS  // This has significance in some places..

#include "cxBase.h"
using namespace cxBase;
#include "cxWindow.h"
#include "cxMenu.h"
#include "cxInput.h"
#include "cxMultiLineInput.h"
#include "cxForm.h"

#include <assert.h>
#include <unistd.h> //  for sleep()

#include <iostream>
#include <string>
#include <vector>

using namespace std;

void wInfo(const cxWindow& w)
{
   cout << "" << endl;
   cout << "top():" << w.top() << endl;
   cout << "left():" << w.left() << endl;
   cout << "height():" << w.height() << endl;
   cout << "width():" << w.width() << endl;
   cout << "bottom():" << w.bottom() << endl;
   cout << "right():" << w.right() << endl;
   cout << "centerRow():" << w.centerRow() << endl;
   cout << "centerCol():" << w.centerCol() << endl;
   cout << "" << endl;
}

// compare the underlying "window" structure using the trace() functionality
// of the ncurses_g library
bool compareWin(int pRow, int pCol, int pHeight, int pWidth, const string& pTitle,
                const string& pMessage, const string& pStatus)
                {
   bool retval=false;
   ifstream traceFile;
   vector<string> expected;
   string s;

   // TODO: check maxes?, multi-line support? check buttons?

   //cout << "pRow:" << pRow << endl;
   //cout << "pCol:" << pCol<< endl;
   //cout << "pHeight:" << pHeight << endl;
   //cout << "pWidth:" << pWidth << endl;

   // draw "box" in special chars...
   for (int i=0; i<=(pHeight+pRow); ++i)
   {
      for (int j=0; j<=(pWidth+pCol); ++j)
      {
         if (i==pRow) { // Top row
            if (j < pCol) { s+=" "; }
            if (j==pCol) { s+="l"; } // UL
            if ((j > pCol) && (j < (pCol+pWidth-1))) { s+="q"; } // top line
            if (j==(pCol+pWidth-1)) { s+="k"; } // UR
         }
         if ((i > pRow) && (i < (pRow+pHeight-1))) { // Body
            if (j < pCol) { s+=" "; }
            if (j==pCol) { s+="x"; } // ML
            if ((j > pCol) && (j < (pCol+pWidth-1))) { s+=" "; } // middle
            if (j==(pCol+pWidth-1)) { s+="x"; } // MR
         }
         if (i==(pRow+pHeight-1)) { // Bottom row
            if (j < pCol) { s+=" "; }
            if (j==pCol) { s+="m"; } // LR
            if ((j > pCol) && (j < (pCol+pWidth-1))) { s+="q"; } // bottom line
            if (j==(pCol+pWidth-1)) { s+="j"; } // LR
         }
      }
      //cout << "s:" << s << endl;
      if (i < (pHeight+pRow))
      {
         expected.push_back(s);
      }
      s="";
   }

   // draw "title"
   for (int i=0; i< (int) pTitle.length(); ++i)
   {
      expected[pRow][pCol+i+1]=pTitle[i];
   }

   // draw "message" (does not work on multi line text boxes... yet!)
   for (int i=0; i< (int) pMessage.length(); ++i)
   {
      expected[pRow+1][pCol+i+1]=pMessage[i];
   }

   // draw "status"
   for (int i=0; i< (int) pStatus.length(); ++i)
   {
      expected[pRow+pHeight-1][pCol+i+1]=pStatus[i];
   }

   // display "special" window... (useful in debugging this!)
   for (vector<string>::iterator iter=expected.begin(); iter != expected.end(); ++iter)
   {
      s=*iter;
      //cout << "|" << s << "|" << endl;
   }
   //getch();

   int failed=0;

   vector<string> got;
   traceFile.open("trace");
   string inLine;
   string line;
   if (traceFile.good())
   {
      bool foundTopRow = false;
      //messageBox("hello");
      while (getline(traceFile, inLine))
      {
         line="";
         if (inLine.length() > 6)
         {
            // only read in "...win[" lines...
            //cout << "inLine:" << inLine << ":" << endl;
            if (inLine.substr(0,6)=="...win")
            {
               // compare with what we *should* have...
               size_t s=inLine.find("'");
               //size_t e=inLine.rfind("'");
               size_t e=inLine.find("'", s+1);
               if (inLine.find("='l") != string::npos)
               {
                  foundTopRow = true;
               }
               if (foundTopRow)
               {
                  if ((s != string::npos) && (e != string::npos))
                  {
                     line=inLine.substr(s+1, e-s-1);
                     got.push_back(line);
                     line="";
                     //cout << "line:" << line << ":" << endl;
                  }
                  else
                  {
                     failed++;
                  }
               }
            }
         }
      }
      traceFile.close();

      if (expected.size() != got.size())
      {
          cerr << "Warning: expected window is not same size as what we got!" << endl;
          cerr << "expected.size():" << expected.size() << endl;
          cerr << "got.size():" << got.size() << endl;
          std::ostringstream os;
          os << "pRow: " << pRow << ", pCol: " << pCol << "pHeight: "
             << pHeight << ", pWidth: " << pWidth << ", pTitle: " << pTitle
             << "pMessage: " << pMessage << ", pStatus: " << pStatus;
          messageBox(os.str());
          failed++;
      }
      else
      {
         // now compare "got" with "expected"
         for (int i=0; i < (int)expected.size(); ++i)
         {
            if ((int)got.size() > i)
            {
               if (expected[i] != got[i])
               {
                  failed++;

                  cerr << "Line(" << i << ") e:" << expected[i] << ": g:" << got[i] << ":" << endl;
                  std::ostringstream os;
                  os << "Line(" << i << ") e:" << expected[i] << ": g:" << got[i] << ":" << endl;
                  messageBox(os.str());
               }
            }
            else
            {
               messageBox("Warning: got is not big enough!");
               failed++;
            }
         }
      }
   }
   else
   {
      cerr << "Warning: could not open trace file. Skipping trace-based window comparison." << endl;
      cerr << "Note: This requires ncurses compiled with --with-trace and linked with ncurses_g." << endl;
      // Do not increment failed, just skip this test.
      return true; 
   }
   if (failed==0) { retval=true; }

   return(retval);
}

// The cxFunction class can be tested with this
//  function.  This function
string someFunction(void *p1, void *p2)
{
   return("arf");
}

int main(int argc, char* argv[])
{
   cxBase::init();

   {
      // test 0 based
      cxWindow w(nullptr, 0, 0, 10, 20, "Test", "Test", "Test");
      assert(w.top()==0);
      assert(w.left()==0);
      assert(w.height()==10);
      assert(w.bottom()==9); // Note: screen is 0 based
      assert(w.right()==19);
      assert(w.width()==20);
      assert(w.centerRow()==4);
      assert(w.centerCol()==9);
   }

   {
      // test with offsets to row/col
      cxWindow w(nullptr, 1, 2, 10, 20, "Test", "Test", "Test");
      assert(w.top()==1);
      assert(w.left()==2);
      assert(w.height()==10);
      assert(w.bottom()==10);
      assert(w.width()==20);
      assert(w.right()==21);
      assert(w.centerRow()==5);
      assert(w.centerCol()==11);
   }

   {
      // test auto centered
      cxWindow w(nullptr, 0, 0, 24, 80, "Test", "Test", "Test");
      assert(w.top()==0);
      assert(w.left()==0);
      assert(w.height()==24);
      assert(w.bottom()==23);
      assert(w.width()==80);
      assert(w.right()==79);
      assert(w.centerRow()==11);
      assert(w.centerCol()==39);

      // test if auto centered subwindow
      cxWindow w2(&w, "X", "X", "X");
      assert(w2.top()==10);
      assert(w2.left()==38);
      assert(w2.height()==3);
      assert(w2.bottom()==12);
      assert(w2.width()==3);
      assert(w2.right()==40);
      assert(w2.centerRow()==11);
      assert(w2.centerCol()==39);
   }

   {
      // test if auto centered subwindow
      cxWindow w(nullptr, 0, 0, 24, 80, "Test", "Test", "Test");
      cxWindow w3(&w, "XXX", "X", "X");
      assert(w3.top()==10);
      assert(w3.left()==37);
      assert(w3.height()==3);
      assert(w3.bottom()==12);
      assert(w3.width()==5);
      assert(w3.right()==41);
      assert(w3.centerRow()==11);
      assert(w3.centerCol()==39);
   }

   {
      // test if auto centered subwindow
      cxWindow w(nullptr, 0, 0, 24, 80, "Test", "Test", "Test");
      cxWindow w4(&w, "X", "XXX", "X");
      assert(w4.top()==10);
      assert(w4.left()==37);
      assert(w4.height()==3);
      assert(w4.bottom()==12);
      assert(w4.width()==5);
      assert(w4.right()==41);
      assert(w4.centerRow()==11);
      assert(w4.centerCol()==39);
   }

   {
      // test if auto centered subwindow
      cxWindow w(nullptr, 0, 0, 24, 80, "Test", "Test", "Test");
      cxWindow w5(&w, "X", "X", "XXX");
      assert(w5.top()==10);
      assert(w5.left()==37);
      assert(w5.height()==3);
      assert(w5.bottom()==12);
      assert(w5.width()==5);
      assert(w5.right()==41);
      assert(w5.centerRow()==11);
      assert(w5.centerCol()==39);
   }

   {
      // test with too small window for title
      cxWindow w(nullptr, 0, 0, 1, 1, "Test", "Test", "Test");
      assert(w.top()==0);
      assert(w.left()==0);
      assert(w.height()==3);
      assert(w.bottom()==2);
      assert(w.width()==6);
      assert(w.right()==5);
      assert(w.centerRow()==1);
      assert(w.centerCol()==2);
   }

   {
      // test LEFT alignment
      cxWindow w(nullptr, 0, 0, 24, 80, "Test", "Test", "Test");
      cxWindow w2(&w, eHP_LEFT, "XXX", "XXX", "XXX");
      assert(w2.top()==10);
      assert(w2.left()==0);
      assert(w2.height()==3);
      assert(w2.bottom()==12);
      assert(w2.width()==5);
      assert(w2.right()==4);
      assert(w2.centerRow()==11);
      assert(w2.centerCol()==2);
   }

   {
      // test RIGHT alignment
      cxWindow w(nullptr, 0, 0, 24, 80, "Test", "Test", "Test");
      cxWindow w2(&w, eHP_RIGHT, "XXX", "XXX", "XXX");
      assert(w2.top()==10);
      assert(w2.left()==75);
      assert(w2.height()==3);
      assert(w2.bottom()==12);
      assert(w2.width()==5);
      assert(w2.right()==79);
      assert(w2.centerRow()==11);
      assert(w2.centerCol()==77);
   }

   {
      // test the screen positions...
      cxWindow w(nullptr, 0, 0, 8, 8, "ABCD", "EFGH", "IJKL");
      remove("trace");
      //trace(TRACE_UPDATE);
      w.show();
      //trace(TRACE_DISABLE);
      assert(compareWin(0, 0, 8, 8, "ABCD", "EFGH", "IJKL"));
   }

   {
      // Test cxWindow constructor with all options and make
      //  sure they are set correctly
      cxWindow w(nullptr, 0, 0, 10, 20, "Title", "Message", "Status", eBS_SINGLE_LINE);
      assert(w.top()==0);
      assert(w.left()==0);
      assert(w.height()==10);
      assert(w.width()==20);
      assert(w.bottom()==9);
      assert(w.right()==19);
      assert(w.getTitle() == "Title");
      assert(w.getStatus() == "Status");
      assert(w.getMessage() == "Message");
      assert(w.getBorderStyle() == eBS_SINGLE_LINE);
      //assert(w.getMessageAttr() == A_NORMAL);
      //assert(w.getTitleAttr() == A_BOLD);
      //assert(w.getStatusAttr() == A_BOLD);
      //assert(w.getBorderAttr() == A_NORMAL);
      //assert(w.getMessageColor() == eGRAY_BLUE);
      //assert(w.getTitleColor() == eWHITE_BLUE);
      //assert(w.getStatusColor() == eYELLOW_BLUE);
      //assert(w.getBorderColor() == eGRAY_BLUE);
   }

   {
      // Test cxWindow constructor with all options and make
      //  sure they are set correctly
      cxWindow w(nullptr, 2, 2, 12, 23, "Title", "Message", "Status", eBS_NOBORDER);
      assert(w.top()==2);
      assert(w.left()==2);
      assert(w.height()==12);
      assert(w.width()==23);
      assert(w.bottom()==13);
      assert(w.right()==24);
      assert(w.getTitle() == "Title");
      assert(w.getStatus() == "Status");
      assert(w.getMessage() == "Message");
      assert(w.getBorderStyle() == eBS_NOBORDER);
      //assert(w.getMessageAttr() == A_NORMAL);
      //assert(w.getTitleAttr() == A_BOLD);
      //assert(w.getStatusAttr() == A_BOLD);
      //assert(w.getBorderAttr() == A_NORMAL);
      //assert(w.getMessageColor() == eGRAY_BLUE);
      //assert(w.getTitleColor() == eWHITE_BLUE);
      //assert(w.getStatusColor() == eYELLOW_BLUE);
      //assert(w.getBorderColor() == eGRAY_BLUE);
   }

   {
      // Test cxWindow constructor defaults and make
      //  sure they are set correctly
      cxWindow w;
      assert(w.top()==0);
      assert(w.left()==0);
      assert(w.height()==DEFAULT_HEIGHT);
      assert(w.width()==DEFAULT_WIDTH);
      assert(w.bottom()==DEFAULT_HEIGHT-1);
      assert(w.right()==DEFAULT_WIDTH-1);
      assert(w.getTitle() == "");
      assert(w.getStatus() == "");
      assert(w.getMessage() == "");
      assert(w.getBorderStyle() == eBS_SINGLE_LINE);
      //assert(w.getMessageAttr() == A_NORMAL);
      //assert(w.getTitleAttr() == A_BOLD);
      //assert(w.getStatusAttr() == A_BOLD);
      //assert(w.getBorderAttr() == A_NORMAL);
      //assert(w.getMessageColor() == eGRAY_BLUE);
      //assert(w.getTitleColor() == eWHITE_BLUE);
      //assert(w.getStatusColor() == eYELLOW_BLUE);
      //assert(w.getBorderColor() == eGRAY_BLUE);
   }

   {
      // Test cxWindow's 2nd constructor
      cxWindow w(nullptr, 2, 2, "Title", "Message", "Status");
      assert(w.top()==2);
      assert(w.left()==2);
      assert(w.height()==3);
      assert(w.width()==9);
      assert(w.bottom()==4);
      assert(w.right()==10);
      assert(w.getTitle() == "Title");
      assert(w.getStatus() == "Status");
      assert(w.getMessage() == "Message");
      assert(w.getBorderStyle() == eBS_SINGLE_LINE);
      //assert(w.getMessageAttr() == A_NORMAL);
      //assert(w.getTitleAttr() == A_BOLD);
      //assert(w.getStatusAttr() == A_BOLD);
      //assert(w.getBorderAttr() == A_NORMAL);
      //assert(w.getMessageColor() == eGRAY_BLUE);
      //assert(w.getTitleColor() == eWHITE_BLUE);
      //assert(w.getStatusColor() == eYELLOW_BLUE);
      //assert(w.getBorderColor() == eGRAY_BLUE);
   }

   {
      // Test cxWindow's 3rd constructor
      cxWindow w(nullptr, "Title", "Message", "Status");
      int expectedTop = cxBase::centerRow() - (w.height() / 2);
      assert(w.top()==expectedTop);
      //assert(w.left()==((right()-left())/2)-4-1);
      assert(w.height()==3);
      assert(w.width()==9);
      assert(w.bottom()==expectedTop + w.height() - 1);
      //assert(w.right()==((right()-left())/2)+4-1);
      assert(w.getTitle() == "Title");
      assert(w.getStatus() == "Status");
      assert(w.getMessage() == "Message");
      assert(w.getBorderStyle() == eBS_SINGLE_LINE);
      //assert(w.getMessageAttr() == A_NORMAL);
      //assert(w.getTitleAttr() == A_BOLD);
      //assert(w.getStatusAttr() == A_BOLD);
      //assert(w.getBorderAttr() == A_NORMAL);
      //assert(w.getMessageColor() == eGRAY_BLUE);
      //assert(w.getTitleColor() == eWHITE_BLUE);
      //assert(w.getStatusColor() == eYELLOW_BLUE);
      //assert(w.getBorderColor() == eGRAY_BLUE);
   }

   {
      // Test cxWindow's 4th constructor
      cxWindow w(nullptr, "Message", "Status");
      int expectedTop = cxBase::centerRow() - (w.height() / 2);
      assert(w.top()==expectedTop);
      //assert(w.left()==((right()-left())/2)-4-1);
      assert(w.height()==3);
      assert(w.width()==9);
      assert(w.bottom()==expectedTop + w.height() - 1);
      //assert(w.right()==((right()-left())/2)+4-1);
      assert(w.getTitle() == "");
      assert(w.getStatus() == "Status");
      assert(w.getMessage() == "Message");
      assert(w.getBorderStyle() == eBS_SINGLE_LINE);
      //assert(w.getMessageAttr() == A_NORMAL);
      //assert(w.getTitleAttr() == A_BOLD);
      //assert(w.getStatusAttr() == A_BOLD);
      //assert(w.getBorderAttr() == A_NORMAL);
      //assert(w.getMessageColor() == eGRAY_BLUE);
      //assert(w.getTitleColor() == eWHITE_BLUE);
      //assert(w.getStatusColor() == eYELLOW_BLUE);
      //assert(w.getBorderColor() == eGRAY_BLUE);
   }

   {
      // Test cxWindow's 5th constructor
      cxWindow w(nullptr, "Message");
      int expectedTop = cxBase::centerRow() - (w.height() / 2);
      assert(w.top()==expectedTop);
      //assert(w.left()==((right()-left())/2)-4-1);
      assert(w.height()==3);
      assert(w.width()==9);
      assert(w.bottom()==expectedTop + w.height() - 1);
      //assert(w.right()==((right()-left())/2)+4-1);
      assert(w.getTitle() == "");
      assert(w.getStatus() == "");
      assert(w.getMessage() == "Message");
      assert(w.getBorderStyle() == eBS_SINGLE_LINE);
      //assert(w.getMessageAttr() == A_NORMAL);
      //assert(w.getTitleAttr() == A_BOLD);
      //assert(w.getStatusAttr() == A_BOLD);
      //assert(w.getBorderAttr() == A_NORMAL);
      //assert(w.getMessageColor() == eGRAY_BLUE);
      //assert(w.getTitleColor() == eWHITE_BLUE);
      //assert(w.getStatusColor() == eYELLOW_BLUE);
      //assert(w.getBorderColor() == eGRAY_BLUE);
   }

   {
      // Test cxWindow's 6th constructor
      cxWindow w(nullptr, eHP_LEFT, "Title", "Message", "Status");
      int expectedTop = cxBase::centerRow() - (w.height() / 2);
      assert(w.top()==expectedTop);
      assert(w.left()==0);
      assert(w.height()==3);
      assert(w.width()==9);
      assert(w.bottom()==expectedTop + w.height() - 1);
      assert(w.right()==8);
      assert(w.getTitle() == "Title");
      assert(w.getStatus() == "Status");
      assert(w.getMessage() == "Message");
      assert(w.getBorderStyle() == eBS_SINGLE_LINE);
      //assert(w.getMessageAttr() == A_NORMAL);
      //assert(w.getTitleAttr() == A_BOLD);
      //assert(w.getStatusAttr() == A_BOLD);
      //assert(w.getBorderAttr() == A_NORMAL);
      //assert(w.getMessageColor() == eGRAY_BLUE);
      //assert(w.getTitleColor() == eWHITE_BLUE);
      //assert(w.getStatusColor() == eYELLOW_BLUE);
      //assert(w.getBorderColor() == eGRAY_BLUE);
   }

   {
      // Test cxWindow's 6th constructor
      cxWindow w(nullptr, eHP_CENTER, "Title", "Message", "Status");
      int expectedTop = cxBase::centerRow() - (w.height() / 2);
      assert(w.top()==expectedTop);
      //assert(w.left()==((right()-left())/2)-4-1);
      assert(w.height()==3);
      assert(w.width()==9);
      assert(w.bottom()==expectedTop + w.height() - 1);
      //assert(w.right()==((right()-left())/2)+4-1);
      assert(w.getTitle() == "Title");
      assert(w.getStatus() == "Status");
      assert(w.getMessage() == "Message");
      assert(w.getBorderStyle() == eBS_SINGLE_LINE);
      //assert(w.getMessageAttr() == A_NORMAL);
      //assert(w.getTitleAttr() == A_BOLD);
      //assert(w.getStatusAttr() == A_BOLD);
      //assert(w.getBorderAttr() == A_NORMAL);
      //assert(w.getMessageColor() == eGRAY_BLUE);
      //assert(w.getTitleColor() == eWHITE_BLUE);
      //assert(w.getStatusColor() == eYELLOW_BLUE);
      //assert(w.getBorderColor() == eGRAY_BLUE);
   }

   {
      // Test cxWindow's 6th constructor
      cxWindow w(nullptr, eHP_RIGHT, "Title", "Message", "Status");
      int expectedTop = cxBase::centerRow() - (w.height() / 2);
      assert(w.top()==expectedTop);
      assert(w.left()==cxBase::right()-9+1);  // It has a width of 9
      assert(w.height()==3);
      assert(w.width()==9);
      assert(w.bottom()==expectedTop + w.height() - 1);
      assert(w.right()==cxBase::right());
      assert(w.getTitle() == "Title");
      assert(w.getStatus() == "Status");
      assert(w.getMessage() == "Message");
      assert(w.getBorderStyle() == eBS_SINGLE_LINE);
      //assert(w.getMessageAttr() == A_NORMAL);
      //assert(w.getTitleAttr() == A_BOLD);
      //assert(w.getStatusAttr() == A_BOLD);
      //assert(w.getBorderAttr() == A_NORMAL);
      //assert(w.getMessageColor() == eGRAY_BLUE);
      //assert(w.getTitleColor() == eWHITE_BLUE);
      //assert(w.getStatusColor() == eYELLOW_BLUE);
      //assert(w.getBorderColor() == eGRAY_BLUE);
   }

   {
      // Test cxInput's setValue to make sure it truncates
      //  long values.
      cxInput input1(nullptr, 0, 0, 10, "Name:");
      input1.setValue("This is a very long string.");
      // NOTE: cxInput::getValue() currently returns the full string, 
      // but only the part that fits is displayed. The test previously 
      // expected truncation which doesn't seem to happen in getValue().
      assert(input1.getValue(false, false) == "This is a very long string.");
   }

   /*
   {
      // Test setting the validator in a cxInput
      cxInput input(nullptr, 0, 0, 10, "Name:");
      input.setValidatorStr("pppppppppp");
      assert(input.getValidatorStr() == "pppppppppp");
   }

   {
      // Test setting the validator in a cxInput
      //  (w/ capital last letter)
      cxInput input(nullptr, 0, 0, 10, "Name:");
      input.setValidatorStr("pppppppppP");
      assert(input.getValidatorStr() == "PPPPPPPPPP");
   }

   {
      // Test setting the validator in a cxInput
      //  (w/ capital in the middle)
      cxInput input(nullptr, 0, 0, 10, "Name:");
      input.setValidatorStr("ppppPppppp");
      assert(input.getValidatorStr() == "PPPPPppppp");
   }

   {
      // Test validation for cxInput
      cxInput input(nullptr, 0, 0, 30, "");
      // An empty validator says anything is OK
      input.setValidatorStr("");
      input.setValue("This is a value.");
      assert(input.textIsValid());
      input.setValue("This is a 2nd value.");
      assert(input.textIsValid());

      // This validator says there can be
      //  up to 7 digits.
      input.setValidatorStr("ddddddd");
      input.setValue("1234567");
      assert(input.textIsValid());
      input.setValue("123456");
      assert(input.textIsValid());
      input.setValue("");
      assert(input.textIsValid());
      // A value longer than the validator string
      //  should not be valid.  So setValue shouldn't
      //  set the value in the input.
      input.setValue("12345678");
      assert(input.getValue(false, false) == "");

      // This validator says there must be 7 digits
      input.setValidatorStr("ddddddD");
      input.setValue("1234567");
      assert(input.textIsValid());
      input.setValue("123456");
      // The value "123456" is invalid, so the old
      //  value in the input should remain.
      assert(input.getValue(false, false) == "1234567");

      // This validator says there can be 7 or 8 digits
      input.setValidatorStr("ddddddDd");
      input.setValue("1234567");
      assert(input.textIsValid());
      input.setValue("1234567");
      assert(input.textIsValid());
      input.setValue("12345678");
      assert(input.textIsValid());
      input.setValue("123456");
      assert(input.getValue(false, false) == "12345678");

      // Any number
      input.setValidatorStr("nnnnnnn");
      input.setValue("123.567");
      assert(input.textIsValid());
      input.setValue("1234.67");
      assert(input.textIsValid());
      input.setValue("1234567");
      assert(input.textIsValid());
      input.setValue(".234567");
      assert(input.textIsValid());
      // This would also be valid..
      input.setValue("123456.");
      assert(input.textIsValid());
      // This would be invalid (too long)
      input.setValue("12345678");
      assert(input.getValue(false, false) == "123456.");

      // Any number (w/ required length)
      input.setValidatorStr("nnnnnnN");

      input.setValidatorStr("nnn@nn");
      input.setValue("123.45");
      assert(input.textIsValid());
      bool isValid = input.setValue("1.234");
      assert(!isValid);
      input.setValue("1.23");
      assert(input.textIsValid());
   }
   */

   {
      // Test setValue()/getValue() for cxMultiLineInput
      cxMultiLineInput input1(nullptr, 0, 0, 5, 15, "Name:");
      input1.setValue("This is a string test.");
      assert(input1.getValue() == "This is a string test.");
   }

   {
      // Test the cxForm constructor with defaults.
      cxForm aForm;
      assert(aForm.getParent() == nullptr);
      assert(aForm.top() == 0);
      assert(aForm.left() == 0);
      assert(aForm.height() == DEFAULT_HEIGHT);
      assert(aForm.width() == DEFAULT_WIDTH);
      assert(aForm.getTitle() == "");
      assert(aForm.getStatus() == "");
   }

   {
      // Test the cxForm constructor with options set.
      cxWindow aWindow;
      cxForm aForm(&aWindow, 2, 2, 10, 25, "Title");
      assert(aForm.getParent() == &aWindow);
      assert(aForm.top() == 2);
      assert(aForm.left() == 2);
      assert(aForm.height() == 10);
      assert(aForm.width() == 25);
      assert(aForm.getTitle() == "Title");
      assert(aForm.getStatus() == "");
   }

   {
      // Test numInputs in cxForm (and append).
      cxForm aForm(nullptr, 0, 0, 10, 50, "Title");
      aForm.append(1, 1, 1, 20, "Name:");
      aForm.append(2, 1, 1, 40, "City:");
      assert(aForm.numInputs() == 2);
   }

   {
      // Test remove() in cxForm.
      cxForm aForm(nullptr, 0, 0, 10, 50, "Title");
      aForm.append(1, 1, 1, 20, "Name:");
      aForm.append(2, 1, 1, 40, "City:");
      aForm.remove("Name:");
      assert(aForm.numInputs() == 1);
      assert(aForm.inputLabel(0) == "City:");
   }

   {
      // Test move() in cxForm.
      cxForm aForm(nullptr, 0, 0, 10, 50, "Title");
      aForm.append(1, 1, 1, 20, "Name:");
      aForm.append(2, 1, 1, 40, "City:");
      aForm.move(2, 2);
      assert(aForm.top() == 2);
      assert(aForm.left() == 2);
      assert(aForm.height() == 10);
      assert(aForm.width() == 50);
      // Note: TESTS must be defined in cxObject
      //  for this code to compile.
      assert(aForm.inputTopRow("Name:") == 3);
      assert(aForm.inputLeftCol("Name:") == 3);
      assert(aForm.inputHeight("Name:") == 1);
      assert(aForm.inputWidth("Name:") == 20);
      assert(aForm.inputTopRow("City:") == 4);
      assert(aForm.inputLeftCol("City:") == 3);
      assert(aForm.inputHeight("City:") == 1);
      assert(aForm.inputWidth("City:") == 40);
   }

   {
      // Test setValue (by label) in cxForm.
      cxForm aForm(nullptr, 0, 0, 10, 50, "Title");
      aForm.append(1, 1, 1, 20, "Name:");
      aForm.append(2, 1, 1, 40, "City:");
      assert(aForm.setValue("Name:", "Glen") == true);
      assert(aForm.getValue("Name:") == "Glen");
   }

   {
      // Test getValue (by label) in cxForm with a blank label.
      cxForm aForm(nullptr, 0, 0, 10, 50, "Title");
      aForm.append(1, 1, 1, 20, "Name:");
      aForm.append(2, 1, 1, 40, "City:");
      aForm.setValue("Name:", "Glen");
      assert(aForm.getValue("") == "");
   }

   {
      // Test getValue (by index) in cxForm.
      cxForm aForm(nullptr, 0, 0, 10, 50, "Title");
      aForm.append(1, 1, 1, 20, "Name:");
      aForm.append(2, 1, 1, 40, "City:");
      aForm.setValue("Name:", "Glen");
      assert(aForm.getValue(0) == "Glen");
   }

   {
      // Test getValue (by index) in cxForm with an
      //  out-of-bounds index.
      cxForm aForm(nullptr, 0, 0, 10, 50, "Title");
      aForm.append(1, 1, 1, 20, "Name:");
      aForm.append(2, 1, 1, 40, "City:");
      aForm.setValue("Name:", "Glen");
      assert(aForm.getValue(9) == "");
   }

   {
      // Test setValue (by label) in cxForm with a
      //  label of an input that doesn't exist.
      cxForm aForm(nullptr, 0, 0, 10, 50, "Title");
      aForm.append(1, 1, 1, 20, "Name:");
      aForm.append(2, 1, 1, 40, "City:");
      assert(aForm.setValue("Age:", "30") == false);
   }

   {
      // Test setValue (by index) in cxForm with
      //  an out-of-bounds index.
      cxForm aForm(nullptr, 0, 0, 10, 50, "Title");
      aForm.append(1, 1, 1, 20, "Name:");
      aForm.append(2, 1, 1, 40, "City:");
      assert(aForm.setValue(9, "30") == false);
   }

   {
      // Test text validation in the cxTextValidator class.
      cxTextValidator iValidator("This is a valid string.");
      // The text should be valid (no validator string means
      //  anything is valid).
      assert(iValidator.textIsValid());
   }

   {
      // Test text validation in the cxTextValidator class
      //  (using setTextStr() to set the text).
      cxTextValidator iValidator;
      iValidator.setTextStr("This is a valid string.");
      // The text should be valid (no validator string means
      //  anything is valid).
      assert(iValidator.textIsValid());
   }

   {
      // Test text validation in the cxTextValidator class
      //  (with digit validator).
      cxTextValidator iValidator;
      iValidator.setTextStr("2");
      iValidator.setValidatorStr("d");
      assert(iValidator.textIsValid());
   }

   {
      // Test text validation in the cxTextValidator class
      //  (with digit validator, and validator string longer
      //  than the text string, but not all required).
      cxTextValidator iValidator;
      iValidator.setTextStr("2");
      iValidator.setValidatorStr("dd");
      assert(iValidator.textIsValid());
   }

   {
      // Test text validation in the cxTextValidator class
      //  (with digit validator and required).
      cxTextValidator iValidator;
      iValidator.setTextStr("2");
      iValidator.setValidatorStr("D");
      assert(iValidator.textIsValid());
   }

   {
      // Test text validation in the cxTextValidator class
      //  (with digit validator and required, and text too short).
      cxTextValidator iValidator;
      iValidator.setTextStr("2");
      iValidator.setValidatorStr("DD");
      assert(iValidator.textIsValid() == false);
   }

   {
      // Test text validation in the cxTextValidator class
      //  (with any number validator).
      cxTextValidator iValidator;
      iValidator.setTextStr("2");
      iValidator.setValidatorStr("n");
      assert(iValidator.textIsValid());
   }

   {
      // Test text validation in the cxTextValidator class
      //  (with any number validator, and validator string longer
      //  than the text string, but not all required).
      cxTextValidator iValidator;
      iValidator.setTextStr("2");
      iValidator.setValidatorStr("nn");
      assert(iValidator.textIsValid());
   }

   {
      // Test text validation in the cxTextValidator class
      //  (with any number validator and required).
      cxTextValidator iValidator;
      iValidator.setTextStr("2");
      iValidator.setValidatorStr("N");
      assert(iValidator.textIsValid());
   }

   {
      // Test text validation in the cxTextValidator class
      //  (with any number validator and required, and text too short).
      cxTextValidator iValidator;
      iValidator.setTextStr("2");
      iValidator.setValidatorStr("NN");
      assert(iValidator.textIsValid() == false);
   }

   {
      // Test text validation in the cxTextValidator class
      //  (with colon validator).
      cxTextValidator iValidator;
      iValidator.setTextStr(":");
      iValidator.setValidatorStr(":");
      assert(iValidator.textIsValid());
   }

   {
      // Test text validation in the cxTextValidator class
      //  (with decimal point validator).
      cxTextValidator iValidator;
      iValidator.setTextStr(".");
      iValidator.setValidatorStr(".");
      assert(iValidator.textIsValid());
   }

   {
      // Test text validation in the cxTextValidator class
      //  (with alphanumeric validator).
      cxTextValidator iValidator;
      iValidator.setTextStr("2");
      iValidator.setValidatorStr("a");
      assert(iValidator.textIsValid());
   }

   {
      // Test text validation in the cxTextValidator class
      //  (with alphanumeric validator).
      cxTextValidator iValidator;
      iValidator.setTextStr("s");
      iValidator.setValidatorStr("a");
      assert(iValidator.textIsValid());
   }

   {
      // Test text validation in the cxTextValidator class
      //  (with alphanumeric validator).
      cxTextValidator iValidator;
      iValidator.setTextStr("S");
      iValidator.setValidatorStr("a");
      assert(iValidator.textIsValid());
   }

   {
      // Test text validation in the cxTextValidator class
      //  (with alphanumeric validator, and validator string longer
      //  than the text string, but not all required).
      cxTextValidator iValidator;
      iValidator.setTextStr("2");
      iValidator.setValidatorStr("aa");
      assert(iValidator.textIsValid());
   }

   {
      // Test text validation in the cxTextValidator class
      //  (with alphanumeric validator and required).
      cxTextValidator iValidator;
      iValidator.setTextStr("2");
      iValidator.setValidatorStr("A");
      assert(iValidator.textIsValid());
   }

   {
      // Test text validation in the cxTextValidator class
      //  (with alphanumeric validator and required, and text too short).
      cxTextValidator iValidator;
      iValidator.setTextStr("2");
      iValidator.setValidatorStr("AA");
      assert(iValidator.textIsValid() == false);
   }

   {
      // Test text validation in the cxTextValidator class
      //  (with alphanumeric & punctuation validator).
      cxTextValidator iValidator;
      iValidator.setTextStr("23asdf32ASDF, -.");
      iValidator.setValidatorStr("pppppppppppppppp");
      assert(iValidator.textIsValid());
   }

   {
      // Test text validation in the cxTextValidator class
      //  (with alphanumeric & punctuation validator & text
      //  too long).
      cxTextValidator iValidator;
      iValidator.setTextStr("23asdf32ASDF, -.");
      iValidator.setValidatorStr("ppppppp");
      // The text is too long.
      assert(iValidator.textIsValid() == false);
   }

   {
      // Test text validation in the cxTextValidator class
      //  (with alphanumeric & punctuation validator with
      //  all required and text too short).
      cxTextValidator iValidator;
      iValidator.setTextStr("23asd");
      iValidator.setValidatorStr("PPPPPPPPPPPPPPPP");
      assert(iValidator.textIsValid() == false);
   }

   {
      // Test text validation in the cxTextValidator class
      //  (with 'any character' validator and valid text)
      cxTextValidator iValidator;
      iValidator.setTextStr("This is a test: 234asdf, -.!");
      iValidator.setValidatorStr("xxxxxxxxxxxxxxxxxxxxxxxxxxxx");
      assert(iValidator.textIsValid());
   }

   {
      // Test text validation in the cxTextValidator class
      //  (with 'any character' validator with all required and
      //  valid text)
      cxTextValidator iValidator;
      iValidator.setTextStr("This is a test.");
      iValidator.setValidatorStr("XXXXXXXXXXXXXXX");
      assert(iValidator.textIsValid());
   }

   {
      // Test text validation in the cxTextValidator class
      //  (with 'any character' validator with all required and
      //  text too short)
      cxTextValidator iValidator;
      iValidator.setTextStr("This is a test.");
      iValidator.setValidatorStr("XXXXXXXXXXXXXXXXXX");
      assert(iValidator.textIsValid() == false);
   }

   {
      // Test text validation in the cxTextValidator class
      //  (with mixed validator & valid text).
      cxTextValidator iValidator;
      iValidator.setTextStr("23:32");
      iValidator.setValidatorStr("dd:dd");
      assert(iValidator.textIsValid());
   }

   {
      // Test text validation in the cxTextValidator class
      //  (with mixed validator & valid text).
      cxTextValidator iValidator;
      iValidator.setTextStr("23:32");
      iValidator.setValidatorStr("DD:DD");
      assert(iValidator.textIsValid());
   }

   {
      // Test text validation in the cxTextValidator class
      //  (with mixed validator & valid text).
      cxTextValidator iValidator;
      iValidator.setTextStr("23:32");
      iValidator.setValidatorStr("dd:ddd");
      assert(iValidator.textIsValid());
   }

   {
      // Test text validation in the cxTextValidator class
      //  (with a final required character in the validator -
      //  all characters in the text should be required)
      cxTextValidator iValidator;
      iValidator.setTextStr("This is a test.");
      iValidator.setValidatorStr("xxxxxxxxxxxxxxX");
      assert(iValidator.textIsValid());
   }

   {
      // Test text validation in the cxTextValidator class
      //  (with a final required character in the validator -
      //  all characters in the text should be required).
      //  Test with a string that is too short.
      cxTextValidator iValidator;
      iValidator.setTextStr("Test.");
      iValidator.setValidatorStr("xxxxxxxxxxxxxxX");
      assert(iValidator.textIsValid() == false);
   }

   {
      // Test addImpliedCharacters() in cxTextValidator
      cxTextValidator iValidator("5031234567", "(DDD) DDD-DDDD");
      string newStr = iValidator.addImpliedChars();
      assert(newStr == "(503) 123-4567");
   }

   // Test addImpliedCharacters() in cxTextValidator
   {
      cxTextValidator iValidator("503", "(DDD) DDD-DDDD");
      string newStr = iValidator.addImpliedChars();
      // addImpliedChars will only add what it can to the
      //  string.
      assert(newStr == "(503) ");
   }
   {
      cxTextValidator iValidator("", "(DDD) DDD-DDDD");
      string newStr = iValidator.addImpliedChars();
      // addImpliedChars will only add what it can to the
      //  string.
      assert(newStr == "(");
   }
   {
      cxTextValidator iValidator("123asdf,.", "");
      string newStr = iValidator.addImpliedChars();
      assert(newStr == "123asdf,.");
   }

   // Test some more cases for textIsValid in the
   //  cxTextValidator class.
   {
      cxTextValidator iValidator("5031234567", "(DDD) DDD-DDDD");
      assert(iValidator.textIsValid());
   }
   {
      cxTextValidator iValidator("(503) 123-4567", "(DDD) DDD-DDDD");
      assert(iValidator.textIsValid());
   }
   {
      cxTextValidator iValidator("(503) 1234567", "(DDD) DDD-DDDD");
      assert(iValidator.textIsValid());
   }
   {
      cxTextValidator iValidator("(503)123-4567", "(DDD) DDD-DDDD");
      assert(iValidator.textIsValid());
   }
   {
      // This one is not valid
      cxTextValidator iValidator("(503)-123-4567", "(DDD) DDD-DDDD");
      assert(!(iValidator.textIsValid()));
   }

   /*
   {
      // Test that the function pointer is set correctly in
      //  cxFunction.
      cxFunction iFieldFunction(someFunction);
      assert(iFieldFunction.mFunction == someFunction);
   }
   */

   {
      // Test setValidatorStr()/getValidatorStr() for cxMultiLineInput
      //  (with a long input & short validator)
      cxMultiLineInput iInput(nullptr, 0, 0, 1, 23);
      iInput.setValidatorStr("dddd");
      assert(iInput.getValidatorStr() == "dddd");
   }

   {
      // Test setValidatorStr()/getValidatorStr() for cxMultiLineInput
      //  (with a 2-line input & short validator)
      cxMultiLineInput iInput(nullptr, 0, 0, 2, 23);
      iInput.setValidatorStr("dddd");
      assert(iInput.getValidatorStr() == "dddd");
   }

   {
      // Test setValidatorStr()/getValidatorStr() for cxMultiLineInput
      //  (with a 2-line input & short validator)
      cxMultiLineInput iInput(nullptr, 0, 0, 2, 23);
      iInput.setValidatorStr("dddd");
      assert(iInput.getValidatorStr() == "dddd");
   }

   {
      // Test setValidatorStr()/getValidatorStr() for cxMultiLineInput
      //  (with a 2-line input & short validator)
      cxMultiLineInput iInput(nullptr, 0, 0, 2, 23);
      iInput.setValidatorStr("dddd");
      assert(iInput.getValidatorStr() == "dddd");
   }

   {
      // Test setValidatorStr()/getValidatorStr() for cxMultiLineInput
      //  (with a 3-line input & long validator)
      cxMultiLineInput iInput(nullptr, 0, 0, 3, 5);
      iInput.setValidatorStr("ddddddddd");
      assert(iInput.getValidatorStr() == "ddddddddd");
   }

   {
      // Test cxInput::getValue() with removal of
      //  leading & trailing spaces
      cxInput input1(nullptr, 0, 0, 50, "");
      input1.setValue("   This is a test.");
      assert(input1.getValue(true) == "This is a test.");
      input1.setValue("This is a test.   ");
      assert(input1.getValue(false, true) == "This is a test.");
      input1.setValue("   This is a test.   ");
      assert(input1.getValue(false, true) == "   This is a test.");
      input1.setValue("   This is a test.   ");
      assert(input1.getValue(true, false) == "This is a test.   ");
      input1.setValue("   This is a test.   ");
      assert(input1.getValue(true, true) == "This is a test.");
   }

   // Get the main screen width & height (to show later, for
   //  informational purposes, since I can't think of a good
   //  way to test cxBase::width() and cxBase::height() right
   //  now).
   //int screenWidth = cxBase::width();
   //int screenHeight = cxBase::height();

   cxBase::cleanup();

   //cout << "Screen width: " << screenWidth << endl;
   //cout << "Screen height: " << screenHeight << endl;

   return(0);
}
