#ifndef __CXOPENFILEDIALOG_H__
#define __CXOPENFILEDIALOG_H__

// Copyright (c) 2026 Eric N. Oulashin
//
// cxOpenFileDialog.h - A directory & file chooser dialog, similar to
// OpenFileDialog in .NET or QFileDialog in Qt, adapted for text-based display.
// This was made with the help of Claude AI.
//
// cxOpenFileDialog presents a modal file browser dialog.  The layout
// (72 wide x 20 tall) is:
//
//  +-- Open File --------------------------------------------------+
//  | /current/directory/path                                       |
//  +------------------------+--------------------------------------+
//  |  Directories           |  Files                               |
//  |  ..                    |  readme.txt                  1.2 KB  |
//  |  subfolder1            |  document.pdf                3.4 MB  |
//  |  subfolder2            |  notes.doc                 512   B   |
//  |                        |                                      |
//  +------------------------+--------------------------------------+
//  | File name: [_______________________________________________]  |
//  | File type: All Files (*)                 [ OK ] [ Cancel ]    |
//  +---------------------------------------------------------------+
//
// Navigation:
//  Tab / Shift-Tab  : cycle focus between dir list -> file list ->
//                     filename -> file type -> OK -> Cancel
//  Arrow keys       : navigate directory or file lists when focused
//  Enter            : enter directory / confirm file / edit filename /
//                     cycle filter / activate OK or Cancel
//  Escape           : cancel the dialog
//  Mouse click      : click items in lists, buttons, etc.
//
// Return codes from showModal():
//  cxID_OK     : user confirmed a file selection
//  cxID_CANCEL : user cancelled or pressed Escape
//
// Usage:
//   cxOpenFileDialog dlg;
//   dlg.setDirectory("/home/user");
//   dlg.addFilter("Text Files (*.txt)", "*.txt");
//   dlg.addFilter("All Files (*)", "*");
//   if (dlg.showModal() == cxID_OK)
//   {
//       std::string path = dlg.getSelectedFilePath();
//   }

#include "cxWindow.h"
#include "cxInput.h"
#include <string>
#include <vector>
#include <memory>

class cxOpenFileDialog : public cxWindow
{
public:
   // -----------------------------------------------------------------------
   // Layout constants
   // -----------------------------------------------------------------------
   static constexpr int DIALOG_WIDTH   = 72;
   static constexpr int DIALOG_HEIGHT  = 20;

   // Row offsets within the ncurses WINDOW (0 = top border)
   static constexpr int ROW_PATH       = 1;
   static constexpr int ROW_SEP_TOP    = 2;
   static constexpr int ROW_HEADER     = 3;
   static constexpr int ROW_LIST_FIRST = 4;
   static constexpr int ROW_LIST_LAST  = 15;
   static constexpr int LIST_VISIBLE   = 12;  // ROW_LIST_LAST - ROW_LIST_FIRST + 1
   static constexpr int ROW_SEP_BOTTOM = 16;
   static constexpr int ROW_FILENAME   = 17;
   static constexpr int ROW_CONTROLS   = 18;

   // Column offsets
   static constexpr int COL_DIVIDER    = 24;
   static constexpr int COL_LABEL      = 2;
   static constexpr int COL_VALUE      = 14;

   // Directory list display area: cols 1..(COL_DIVIDER-1)
   static constexpr int DIR_COL_START  = 1;
   static constexpr int DIR_DISPLAY_W  = COL_DIVIDER - DIR_COL_START; // 23

   // File list display area: cols (COL_DIVIDER+1)..(DIALOG_WIDTH-2)
   static constexpr int FILE_COL_START = COL_DIVIDER + 1;
   static constexpr int FILE_DISPLAY_W = DIALOG_WIDTH - 2 - FILE_COL_START; // 45

   // Filename input area
   static constexpr int FILENAME_INPUT_W = DIALOG_WIDTH - 2 - COL_VALUE; // 56

   // Bottom controls (row 18)
   static constexpr int FILTER_DISPLAY_W = 30;
   static constexpr int COL_OK_TEXT      = 50;
   static constexpr int OK_TEXT_W        = 6;   // "[ OK ]"
   static constexpr int COL_CANCEL_TEXT  = 58;
   static constexpr int CANCEL_TEXT_W    = 10;  // "[ Cancel ]"

   // -----------------------------------------------------------------------
   // Constructors / Destructor
   // -----------------------------------------------------------------------

   /**
    * @brief Constructs a cxOpenFileDialog centered on screen.
    * @param pParentWindow  Parent window (nullptr = top-level).
    * @param pRow           Top row (-1 = center vertically).
    * @param pCol           Left column (-1 = center horizontally).
    * @param pTitle         Title shown in the dialog border.
    * @param pInitialDir    The initial diretory to open. Defaults to an empty string, which will cause the dialog to use the current directory.
    */
   explicit cxOpenFileDialog(cxWindow *pParentWindow = nullptr,
                             int pRow = -1,
                             int pCol = -1,
                             const std::string& pTitle = " Open File ",
                             const std::string& pInitialDir = "");

   virtual ~cxOpenFileDialog();

   // -----------------------------------------------------------------------
   // cxWindow overrides
   // -----------------------------------------------------------------------

   virtual std::string cxTypeStr() const override;
   virtual void draw() override;

   virtual long showModal(bool pShowSelf = true,
                          bool pBringToTop = true,
                          bool pShowSubwindows = true) override;

   virtual long show(bool pBringToTop = false,
                     bool pShowSubwindows = true) override;

   virtual void hide(bool pHideSubwindows = true) override;
   virtual void unhide(bool pUnhideSubwindows = true) override;
   virtual bool move(int pNewRow, int pNewCol, bool pRefresh = true) override;

   // -----------------------------------------------------------------------
   // Public API
   // -----------------------------------------------------------------------

   /// Set the initial directory to display.
   void setDirectory(const std::string& pPath);
   /// Returns the currently displayed directory.
   std::string getDirectory() const;

   /// Returns the full path of the selected file (directory + filename).
   std::string getSelectedFilePath() const;
   /// Returns just the selected filename.
   std::string getSelectedFileName() const;

   /// Pre-populate the filename input.
   void setFileName(const std::string& pFileName);

   /// Add a file type filter (e.g. "Text Files (*.txt)", "*.txt").
   void addFilter(const std::string& pDescription, const std::string& pPattern);
   /// Remove all file type filters.
   void clearFilters();

   /// Toggle display of hidden files (dotfiles).
   void setShowHiddenFiles(bool pShow);
   bool getShowHiddenFiles() const;

   /// Set whether the user may type a filename for a file that does not yet
   /// exist.  When true (the default), the dialog accepts any non-empty
   /// filename.  When false, the dialog only accepts filenames that
   /// correspond to an existing file.
   void setAllowNewFile(bool pAllow);
   bool getAllowNewFile() const;

private:
   // -----------------------------------------------------------------------
   // Internal focus tracker
   // -----------------------------------------------------------------------
   enum class FocusItem
   {
      DIR_LIST,
      FILE_LIST,
      FILENAME_INPUT,
      FILETYPE,
      OK_BTN,
      CANCEL_BTN
   };

   // -----------------------------------------------------------------------
   // Data types
   // -----------------------------------------------------------------------
   struct FileEntry
   {
      std::string name;
      uintmax_t   size;
   };

   struct FileFilter
   {
      std::string description;
      std::string pattern;
   };

   // -----------------------------------------------------------------------
   // State
   // -----------------------------------------------------------------------
   std::string              mCurrentPath;
   std::string              mSelectedFileName;
   std::vector<std::string> mDirEntries;
   std::vector<FileEntry>   mFileEntries;
   int       mDirSelection     = 0;
   int       mFileSelection    = 0;
   int       mDirScrollOffset  = 0;
   int       mFileScrollOffset = 0;
   FocusItem mFocusItem        = FocusItem::FILE_LIST;

   std::vector<FileFilter>  mFilters;
   int  mCurrentFilter = 0;
   bool mShowHidden    = false;
   bool mAllowNewFile  = true;

   // -----------------------------------------------------------------------
   // Child widget
   // -----------------------------------------------------------------------
   std::shared_ptr<cxInput> mFileNameInput;

   // -----------------------------------------------------------------------
   // Private helpers
   // -----------------------------------------------------------------------
   void initWidgets();
   void loadDirectory();

   // Drawing
   void drawContent();
   void drawPath();
   void drawSeparators();
   void drawListHeaders();
   void drawDirList();
   void drawFileList();
   void drawFilenameRow();
   void drawControlsRow();

   // Focus
   void setFocusItem(FocusItem pItem);

   // Input loop
   long doInputLoop();

   // Navigation
   void moveDirSelection(int pDelta);
   void moveFileSelection(int pDelta);
   void enterSelectedDir();
   void activateFilenameInput();
   void cycleFilter(int pDelta);

   // Validation
   bool canAcceptSelection() const;

   // Utilities
   bool matchesFilter(const std::string& pFileName) const;
   static std::string formatFileSize(uintmax_t pSize);
   static std::string truncStr(const std::string& pStr, int pMaxW);
};

#endif // __CXOPENFILEDIALOG_H__
