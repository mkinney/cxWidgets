#ifndef __CXFILEVIEWER_H__
#define __CXFILEVIEWER_H__

// Copyright (c) 2005-2007 Michael H. Kinney

// Creates a new window, opens a file, and
// displays it in that window.

#include "cxScrolledWindow.h"
#include "cxFunction.h"
#include <string>
#include <memory>

/** \class cxFileViewer
 * \brief Represents a window that displays a file.
 * While the object is being displayed with showModal(), the user
 * can go to a specific line by pressing CTRL-G (this is the default;
 * it can be changed with setGoToKey()).  The user can also search
 * for text with the '/' key (this is the default; it can be changed
 * with setSearchKey()).  By default, the previous keyword is used
 * when searching again; this behavior can be changed with useLastKeyword().<br>
 * Also, cxFileViewer supports page scrolling with the pageUp and pageDown keys.
 * In addition, cxMenu also uses alternate keys for doing a pageUp and pageDown,
 * which by default are '.' and ',', respectively.  These defaults can be changed
 * via setAltPgUpKey() and setAltPgDownKey().
 *
 * \author $Author: erico $
 * \version $Revision: 1.16 $
 * \date $Date: 2007/12/04 00:23:46 $
*/
class cxFileViewer : public cxScrolledWindow
{

   public:
      /**
       * Default constructor.  First two parameters do not have default values
       * @param pParentWindow Pointer to parent window
       * @param pFilename Name of file to view
       * @param pRow Y location of display window
       * @param pCol X location of display window
       * @param pHeight Height of display window
       * @param pWidth Width of display window
       */
      explicit cxFileViewer(cxWindow *pParentWindow, const std::string& pFilename,
                            int pRow = 0, int pCol = 0,
                            int pHeight = DEFAULT_HEIGHT,
                            int pWidth = DEFAULT_WIDTH);

      /**
       * \brief Copy constructor
       *
       * @param pThatFileViewer Another cxFileViewer to be copied
       */
      cxFileViewer(const cxFileViewer& pThatFileViewer);

      /**
       * Destructor
       */
      virtual ~cxFileViewer();

      /**
       * \brief Shows the window.
       * @param pBringToTop Whether or not to bring the window to the top.  Defaults
       *   to true.
       * @param pShowSubwindows Whether or not to show sub-windows also.  Defaults
       *   to true.
       * @return This method returns a long to support overriding in derived classes
       *   that might need to return a value.  For cxWindow, cxFIRST_AVAIL_RETURN_CODE
       *   is always returned.
       */
      virtual long show(bool pBringToTop = true, bool pShowSubwindows = true) override;

      /**
       * \brief Loads the contents of the file
       */
      void load();

      /**
       * Reports the longest line
       * @return Lenght of widest line in characters
       */
      int widestLine();

      /**
       * \brief Returns the name of the cxWidgets class.  This can be used to
       * \brief determine the type of cxWidgets object that deriving classes
       * \brief derive from in applications.
       *
       * @return The name of the cxWidgets class.
       */
      virtual std::string cxTypeStr() const override;

   protected:
      /**
       * \brief Makes a copy of a cxFileViewer's member variables
       *
       * @param pThatFileViewer A pointer to another cxFileViewer whose members to copy
       */
      void copyCxFileViewerStuff(const cxFileViewer* pThatFileViewer);

   private:
      std::string mFilename;

      // Displays the line numbers in the bottom border
      void displayLineNumbers();

      // Calls displayLineNumbers() on a cxFileViewer.  This
      // function is to be used as the 'loop function' to be
      // run at the end of each cycle through the input loop
      // in the parent class.
      static std::string updateLineNumbers(cxFileViewer& pFileViewer);

      // Disable the use of setLoopEndFunction from the outside
      void setLoopEndFunction(const std::shared_ptr<cxFunction>& pFuncPtr) override;
};

#endif
