// Copyright (c) 2005-2007 Michael H. Kinney

#include "cxBase.h"
#include "cxFileViewer.h"
#include "cxStringUtils.h"
#include <fstream>
using std::string;
using std::set;
using std::map;
using std::ifstream;

cxFileViewer::cxFileViewer(cxWindow *pParentWindow, const string& pFilename,
                           int pRow, int pCol, int pHeight, int pWidth)
   : cxScrolledWindow(pParentWindow, pRow, pCol, pHeight, pWidth, pFilename, "", "", eBS_SINGLE_LINE),
     mFilename(pFilename)
{
   // Load the file
   load();

   // Set the status text to display the number of file lines.
   // Note: This adds the text in position 2 because the left scroll arrow will
   // be drawn in position 1 of the border.
   addStatusString(2, "Lines: " + cxStringUtils::toString(mMessageLines.size()));

   // Make sure that the line numbers get updated after each cycle
   // through the input loop
   auto loopEndFunc = cxFunction1RefTemplated<cxFileViewer>::create(cxFileViewer::updateLineNumbers, *this);
   setLoopEndFunction(loopEndFunc);
}

cxFileViewer::cxFileViewer(const cxFileViewer& pThatFileViewer)
   : cxScrolledWindow(pThatFileViewer.getParent(), pThatFileViewer.top(),
           pThatFileViewer.left(), pThatFileViewer.height(),
           pThatFileViewer.width(), pThatFileViewer.getTitle(), "",
           pThatFileViewer.getStatus(), pThatFileViewer.getBorderStyle(),
           pThatFileViewer.getExtTitleWindow(),
           pThatFileViewer.getExtStatusWindow(),
           pThatFileViewer.getHotkeyHighlighting()),
     mFilename(pThatFileViewer.mFilename)
{
   // Load the file
   load();

   // Set the status text to display the number of file lines.
   // Note: This adds the text in position 2 because the left scroll arrow will
   // be drawn in position 1 of the border.
   addStatusString(2, "Lines: " + cxStringUtils::toString(mMessageLines.size()));

   // Make sure that the line numbers get updated after each cycle
   // through the input loop
   auto loopEndFunc = cxFunction1RefTemplated<cxFileViewer>::create(cxFileViewer::updateLineNumbers, *this);
   setLoopEndFunction(loopEndFunc);
}

cxFileViewer::~cxFileViewer()
{
}

long cxFileViewer::show(bool pBringToTop, bool pShowSubwindows)
{
   long retval = cxScrolledWindow::show(pBringToTop, pShowSubwindows);
   displayLineNumbers();
   return(retval);
} // show

void cxFileViewer::load()
{
   mMessageLines.clear();

   // TODO: how to exclude binary files...
   // open file
   ifstream inFile;
   string line;

   inFile.open(mFilename.c_str());
   if (inFile.good())
   {
      // read lines into mMessageLines;
      while (getline(inFile, line))
      {
         // only display "printable" characters
         for (size_t i=0; i<line.length(); ++i)
         {
            if (!isprint(line[i]))
            {
               line[i]=' ';
            }
         }
         mMessageLines.push_back(line);
      }
      // close file
      inFile.close();
   }
} // load

int cxFileViewer::widestLine()
{
   int widest=0;
   string tmpString;
   for (messageLineContainer::iterator iter=mMessageLines.begin();
        iter != mMessageLines.end(); ++iter)
        {
      tmpString=*iter;
      if ((int)tmpString.length() > widest)
      {
         widest=tmpString.length();
      }
   }
   return(widest);
} // widestLine

string cxFileViewer::cxTypeStr() const
{
   return("cxFileViewer");
} // cxTypeStr

//// Protected functions

void cxFileViewer::copyCxFileViewerStuff(const cxFileViewer* pThatFileViewer)
{
   if ((pThatFileViewer != nullptr) && (pThatFileViewer != this))
   {
      // Copy the cxWindow stuff inherited from the parent, then copy
      //  this class' stuff
      try
      {
         copyCxWinStuff((const cxWindow*)pThatFileViewer);
      }
      catch (const cxWidgetsException& exc)
      {
         // Free up the other memory used
         cxWindow::freeWindow();
         throw(cxWidgetsException("Couldn't copy base cxWindow stuff (copying a cxFileViewer)."));
      }

      mFilename = pThatFileViewer->mFilename;
      // Load the file
      load();
   }
} // copyCxFileViewerStuff

//// Private helper functions

void cxFileViewer::displayLineNumbers()
{
   int lineNumber = getLineNumber();
   int subWinHeight = getSubWinHeight();

   int firstLineNum = lineNumber + 1;
   int lastLineNum = lineNumber + subWinHeight;
   // For the last line #, if there aren't enough line numbers
   //  to fill the subwindow, then use mMessageLines.size()
   //  as the last line #.
   if (mMessageLines.size() < (unsigned)subWinHeight)
   {
      lastLineNum = (int)mMessageLines.size();
   }

   // Enable the status attributes
   enableAttrs(mWindow, eSTATUS);
   if (useColors)
   {
      wcolor_set(mWindow, mStatusColorPair, nullptr);
   }
   mvwprintw(mWindow, bottom()-top(), right()-left()-10, "%4d-%-4d", firstLineNum, lastLineNum);
   // Disable the attributes
   disableAttrs(mWindow, eSTATUS);
   // Disable the colors
   if (useColors)
   {
      wcolor_set(mWindow, 0, nullptr);
   }
   wrefresh(mWindow);
} // displayLineNumbers

string cxFileViewer::updateLineNumbers(cxFileViewer& pFileViewer)
{
   pFileViewer.displayLineNumbers();
   return("");
} // updateLineNumbers

// This is private so that it can't be called from the outside
void cxFileViewer::setLoopEndFunction(const std::shared_ptr<cxFunction>& pFuncPtr)
{
   cxScrolledWindow::setLoopEndFunction(pFuncPtr);
} // setLoopEndFunction
