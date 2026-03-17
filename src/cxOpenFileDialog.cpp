// Copyright (c) 2026 Eric N. Oulashin
//
// cxOpenFileDialog.cpp - Implementation of the cxOpenFileDialog dialog.
// This was made with the help of Claude AI.

#include "cxOpenFileDialog.h"
#include "cxKeyDefines.h"
#include "cxReturnCodes.h"
#include "cxBase.h"

#include <filesystem>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <cctype>
#include <cstring>

using std::string;
using std::vector;
using std::make_shared;

namespace fs = std::filesystem;

// ---------------------------------------------------------------------------
// Constructor / Destructor
// ---------------------------------------------------------------------------

cxOpenFileDialog::cxOpenFileDialog(cxWindow *pParentWindow, int pRow, int pCol,
                                   const string& pTitle, const string& pInitialDir)
   : cxWindow(pParentWindow,
              (pRow == -1) ? (LINES - DIALOG_HEIGHT) / 2 : pRow,
              (pCol == -1) ? (COLS  - DIALOG_WIDTH)  / 2 : pCol,
              DIALOG_HEIGHT,
              DIALOG_WIDTH,
              pTitle,
              "",
              "",
              eBS_SINGLE_LINE),
     mFocusItem(FocusItem::FILE_LIST)
{
   // Set default directory to the current working directory
   try
   {
      if (pInitialDir.empty())
      {
         mCurrentPath = fs::current_path().string();
      }
      else
      {
         mCurrentPath = fs::canonical(pInitialDir).string();
      }
   }
   catch (...)
   {
      
      mCurrentPath = fs::current_path().string();
   }

   // Default filter: show all files
   mFilters.push_back({"All Files (*)", "*"});

   initWidgets();
}

cxOpenFileDialog::~cxOpenFileDialog()
{
}

// ---------------------------------------------------------------------------
// cxWindow overrides
// ---------------------------------------------------------------------------

string cxOpenFileDialog::cxTypeStr() const
{
   return "cxOpenFileDialog";
}

void cxOpenFileDialog::draw()
{
   cxWindow::draw();
   wcolor_set(mWindow, mMessageColorPair, nullptr);
   drawContent();
}

long cxOpenFileDialog::showModal(bool pShowSelf, bool pBringToTop,
                                 bool pShowSubwindows)
{
   if (!isEnabled()) return cxID_EXIT;

   mIsModal = true;
   long returnCode = cxID_CANCEL;

   if (!runOnFocusFunction() && !getLeaveNow())
   {
      // Load directory contents
      loadDirectory();

      if (pShowSelf)
      {
         show(pBringToTop, pShowSubwindows);
         // Keep the filename input hidden until the user activates it
         if (mFileNameInput) mFileNameInput->hide();
      }

      // Set initial visual focus
      setFocusItem(mFocusItem);

      int prevCursor = curs_set(0);
      returnCode = doInputLoop();
      curs_set(prevCursor);

      hide();
      runOnLeaveFunction();
   }

   mLeaveNow = false;
   mIsModal  = false;
   return returnCode;
}

long cxOpenFileDialog::show(bool pBringToTop, bool pShowSubwindows)
{
   long rc = cxWindow::show(pBringToTop, pShowSubwindows);
   // Keep filename input hidden; it is shown only when activated
   if (mFileNameInput) mFileNameInput->hide();
   return rc;
}

void cxOpenFileDialog::hide(bool pHideSubwindows)
{
   cxWindow::hide(pHideSubwindows);
   if (mFileNameInput) mFileNameInput->hide(false);
}

void cxOpenFileDialog::unhide(bool pUnhideSubwindows)
{
   if (!isEnabled()) return;
   cxWindow::unhide(pUnhideSubwindows);
   // mFileNameInput stays hidden until the user activates it
   if (mFileNameInput) mFileNameInput->hide(false);
}

bool cxOpenFileDialog::move(int pNewRow, int pNewCol, bool pRefresh)
{
   int origTop  = top();
   int origLeft = left();
   bool moved = cxWindow::move(pNewRow, pNewCol, pRefresh);
   if (moved)
   {
      int dy = top()  - origTop;
      int dx = left() - origLeft;
      if (mFileNameInput) mFileNameInput->moveRelative(dy, dx, pRefresh);
   }
   return moved;
}

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

void cxOpenFileDialog::setDirectory(const string& pPath)
{
   try
   {
      fs::path p(pPath);
      if (fs::exists(p) && fs::is_directory(p))
      {
         mCurrentPath = fs::canonical(p).string();
      }
   }
   catch (...)
   {
   }
}

string cxOpenFileDialog::getDirectory() const
{
   return mCurrentPath;
}

string cxOpenFileDialog::getSelectedFilePath() const
{
   if (mSelectedFileName.empty()) return "";
   // If it is already an absolute path, return as-is
   if (mSelectedFileName[0] == '/') return mSelectedFileName;
   string path = mCurrentPath;
   if (!path.empty() && path.back() != '/') path += '/';
   return path + mSelectedFileName;
}

string cxOpenFileDialog::getSelectedFileName() const
{
   return mSelectedFileName;
}

void cxOpenFileDialog::setFileName(const string& pFileName)
{
   mSelectedFileName = pFileName;
}

void cxOpenFileDialog::addFilter(const string& pDescription,
                                  const string& pPattern)
{
   mFilters.push_back({pDescription, pPattern});
}

void cxOpenFileDialog::clearFilters()
{
   mFilters.clear();
   mCurrentFilter = 0;
}

void cxOpenFileDialog::setShowHiddenFiles(bool pShow)
{
   mShowHidden = pShow;
}

bool cxOpenFileDialog::getShowHiddenFiles() const
{
   return mShowHidden;
}

void cxOpenFileDialog::setAllowNewFile(bool pAllow)
{
   mAllowNewFile = pAllow;
}

bool cxOpenFileDialog::getAllowNewFile() const
{
   return mAllowNewFile;
}

// ---------------------------------------------------------------------------
// Private: widget creation
// ---------------------------------------------------------------------------

void cxOpenFileDialog::initWidgets()
{
   // Filename input - hidden until the user activates the filename field.
   // Uses the same pattern as cxDatePicker's mYearInput.
   mFileNameInput = make_shared<cxInput>(
      this,
      top() + ROW_FILENAME,
      left() + COL_VALUE,
      FILENAME_INPUT_W,
      "",              // no label
      eBS_NOBORDER,
      eINPUT_EDITABLE
   );
   mFileNameInput->hide();
}

// ---------------------------------------------------------------------------
// Private: directory loading
// ---------------------------------------------------------------------------

void cxOpenFileDialog::loadDirectory()
{
   mDirEntries.clear();
   mFileEntries.clear();

   try
   {
      fs::path dirPath(mCurrentPath);
      if (!fs::exists(dirPath) || !fs::is_directory(dirPath))
      {
         mCurrentPath = fs::current_path().string();
         dirPath = fs::path(mCurrentPath);
      }

      mCurrentPath = fs::canonical(dirPath).string();

      // Always add ".." unless at filesystem root
      if (mCurrentPath != "/")
      {
         mDirEntries.push_back("..");
      }

      for (const auto& entry : fs::directory_iterator(
               dirPath, fs::directory_options::skip_permission_denied))
      {
         string name = entry.path().filename().string();
         if (name.empty()) continue;

         // Skip hidden files/dirs unless mShowHidden is true
         if (!mShowHidden && !name.empty() && name[0] == '.') continue;

         try
         {
            if (entry.is_directory())
            {
               mDirEntries.push_back(name);
            }
            else if (entry.is_regular_file())
            {
               if (matchesFilter(name))
               {
                  uintmax_t sz = 0;
                  try { sz = entry.file_size(); }
                  catch (...) {}
                  mFileEntries.push_back({name, sz});
               }
            }
         }
         catch (...)
         {
            // Skip entries we cannot stat
         }
      }
   }
   catch (...)
   {
      // Could not read directory
   }

   // Sort directories alphabetically, keeping ".." at the top
   int startIdx = (!mDirEntries.empty() && mDirEntries[0] == "..") ? 1 : 0;
   if ((int)mDirEntries.size() > startIdx)
   {
      std::sort(mDirEntries.begin() + startIdx, mDirEntries.end(),
         [](const string& a, const string& b)
         {
            return strcasecmp(a.c_str(), b.c_str()) < 0;
         });
   }

   // Sort files alphabetically
   std::sort(mFileEntries.begin(), mFileEntries.end(),
      [](const FileEntry& a, const FileEntry& b)
      {
         return strcasecmp(a.name.c_str(), b.name.c_str()) < 0;
      });

   mDirSelection    = 0;
   mDirScrollOffset = 0;
   mFileSelection   = 0;
   mFileScrollOffset = 0;
}

// ---------------------------------------------------------------------------
// Private: drawing
// ---------------------------------------------------------------------------

void cxOpenFileDialog::drawContent()
{
   if (mWindow == nullptr)
      return;

   wcolor_set(mWindow, mMessageColorPair, nullptr);

   drawPath();
   drawSeparators();
   drawListHeaders();
   drawDirList();
   drawFileList();
   drawFilenameRow();
   drawControlsRow();

   // Do NOT call wnoutrefresh(mWindow) here.  All callers follow up with
   // update_panels() + doupdate(), which composites panels from bottom to
   // top.  Calling wnoutrefresh first would clear the touch flags on this
   // window, preventing update_panels() from re-compositing the dialog's
   // cells on top of any lower panels.
}

void cxOpenFileDialog::drawPath()
{
   mvwhline(mWindow, ROW_PATH, 1, ' ', DIALOG_WIDTH - 2);

   string pathDisplay = mCurrentPath;
   int maxW = DIALOG_WIDTH - 4;
   if ((int)pathDisplay.size() > maxW)
   {
      pathDisplay = "..." + pathDisplay.substr(pathDisplay.size() - maxW + 3);
   }

   wattron(mWindow, A_BOLD);
   mvwaddstr(mWindow, ROW_PATH, COL_LABEL, pathDisplay.c_str());
   wattroff(mWindow, A_BOLD);
}

void cxOpenFileDialog::drawSeparators()
{
   // Top separator (row 2) with T-junction at the column divider
   mvwhline(mWindow, ROW_SEP_TOP, 1, ACS_HLINE, DIALOG_WIDTH - 2);
   mvwaddch(mWindow, ROW_SEP_TOP, 0, ACS_LTEE);
   mvwaddch(mWindow, ROW_SEP_TOP, DIALOG_WIDTH - 1, ACS_RTEE);
   mvwaddch(mWindow, ROW_SEP_TOP, COL_DIVIDER, ACS_TTEE);

   // Vertical divider between the two lists (rows 3 through 15)
   for (int r = ROW_HEADER; r <= ROW_LIST_LAST; ++r)
   {
      mvwaddch(mWindow, r, COL_DIVIDER, ACS_VLINE);
   }

   // Bottom separator (row 16) with inverted T-junction
   mvwhline(mWindow, ROW_SEP_BOTTOM, 1, ACS_HLINE, DIALOG_WIDTH - 2);
   mvwaddch(mWindow, ROW_SEP_BOTTOM, 0, ACS_LTEE);
   mvwaddch(mWindow, ROW_SEP_BOTTOM, DIALOG_WIDTH - 1, ACS_RTEE);
   mvwaddch(mWindow, ROW_SEP_BOTTOM, COL_DIVIDER, ACS_BTEE);
}

void cxOpenFileDialog::drawListHeaders()
{
   // "Directories" header
   bool dirFocused = (mFocusItem == FocusItem::DIR_LIST);
   if (dirFocused) wattron(mWindow, A_BOLD);
   mvwhline(mWindow, ROW_HEADER, DIR_COL_START, ' ', DIR_DISPLAY_W);
   mvwaddstr(mWindow, ROW_HEADER, DIR_COL_START + 1, "Directories");
   if (dirFocused) wattroff(mWindow, A_BOLD);

   // "Files" header
   bool fileFocused = (mFocusItem == FocusItem::FILE_LIST);
   if (fileFocused) wattron(mWindow, A_BOLD);
   mvwhline(mWindow, ROW_HEADER, FILE_COL_START, ' ', FILE_DISPLAY_W);
   mvwaddstr(mWindow, ROW_HEADER, FILE_COL_START + 1, "Files");
   if (fileFocused) wattroff(mWindow, A_BOLD);
}

void cxOpenFileDialog::drawDirList()
{
   for (int i = 0; i < LIST_VISIBLE; ++i)
   {
      int row = ROW_LIST_FIRST + i;
      int idx = mDirScrollOffset + i;

      // Clear the row area
      mvwhline(mWindow, row, DIR_COL_START, ' ', DIR_DISPLAY_W);

      if (idx < (int)mDirEntries.size())
      {
         bool isSelected = (idx == mDirSelection &&
                            mFocusItem == FocusItem::DIR_LIST);
         string display = " " + truncStr(mDirEntries[idx], DIR_DISPLAY_W - 2);

         if (isSelected) wattron(mWindow, A_REVERSE);
         mvwaddnstr(mWindow, row, DIR_COL_START, display.c_str(), DIR_DISPLAY_W);
         if (isSelected) wattroff(mWindow, A_REVERSE);
      }
   }
}

void cxOpenFileDialog::drawFileList()
{
   for (int i = 0; i < LIST_VISIBLE; ++i)
   {
      int row = ROW_LIST_FIRST + i;
      int idx = mFileScrollOffset + i;

      // Clear the row area
      mvwhline(mWindow, row, FILE_COL_START, ' ', FILE_DISPLAY_W);

      if (idx < (int)mFileEntries.size())
      {
         bool isSelected = (idx == mFileSelection &&
                            mFocusItem == FocusItem::FILE_LIST);

         // Format: " filename          size"
         string name = mFileEntries[idx].name;
         string size = formatFileSize(mFileEntries[idx].size);

         int nameW = FILE_DISPLAY_W - (int)size.size() - 2;
         if (nameW < 4) nameW = 4;
         string truncName = truncStr(name, nameW);

         // Build the display line: leading space + name + padding + size
         string display = " " + truncName;
         int padLen = FILE_DISPLAY_W - (int)display.size() - (int)size.size();
         if (padLen > 0) display += string(padLen, ' ');
         display += size;

         // Ensure it fits
         if ((int)display.size() > FILE_DISPLAY_W)
            display = display.substr(0, FILE_DISPLAY_W);

         if (isSelected) wattron(mWindow, A_REVERSE);
         mvwaddnstr(mWindow, row, FILE_COL_START, display.c_str(),
                    FILE_DISPLAY_W);
         if (isSelected) wattroff(mWindow, A_REVERSE);
      }
   }
}

void cxOpenFileDialog::drawFilenameRow()
{
   mvwhline(mWindow, ROW_FILENAME, 1, ' ', DIALOG_WIDTH - 2);
   mvwaddstr(mWindow, ROW_FILENAME, COL_LABEL, "File name:");

   bool focused = (mFocusItem == FocusItem::FILENAME_INPUT);
   if (focused) wattron(mWindow, A_REVERSE);

   // Draw the filename value, padded to fill the input area
   string display = mSelectedFileName;
   if ((int)display.size() > FILENAME_INPUT_W)
      display = display.substr(0, FILENAME_INPUT_W);
   int pad = FILENAME_INPUT_W - (int)display.size();
   if (pad > 0) display += string(pad, ' ');

   mvwaddnstr(mWindow, ROW_FILENAME, COL_VALUE, display.c_str(),
              FILENAME_INPUT_W);

   if (focused)
      wattroff(mWindow, A_REVERSE);
}

void cxOpenFileDialog::drawControlsRow()
{
   mvwhline(mWindow, ROW_CONTROLS, 1, ' ', DIALOG_WIDTH - 2);

   // "File type:" label
   mvwaddstr(mWindow, ROW_CONTROLS, COL_LABEL, "File type:");

   // Current filter text
   {
      bool focused = (mFocusItem == FocusItem::FILETYPE);
      string filterText;
      if (!mFilters.empty() && mCurrentFilter >= 0 &&
          mCurrentFilter < (int)mFilters.size())
      {
         filterText = mFilters[mCurrentFilter].description;
      }
      else
      {
         filterText = "All Files (*)";
      }
      if ((int)filterText.size() > FILTER_DISPLAY_W)
         filterText = filterText.substr(0, FILTER_DISPLAY_W);
      int fpad = FILTER_DISPLAY_W - (int)filterText.size();
      if (fpad > 0) filterText += string(fpad, ' ');

      if (focused) wattron(mWindow, A_REVERSE);
      mvwaddnstr(mWindow, ROW_CONTROLS, COL_VALUE, filterText.c_str(),
                 FILTER_DISPLAY_W);
      if (focused) wattroff(mWindow, A_REVERSE);
   }

   // "[ OK ]" text button
   {
      bool focused = (mFocusItem == FocusItem::OK_BTN);
      if (focused) wattron(mWindow, A_REVERSE);
      mvwaddstr(mWindow, ROW_CONTROLS, COL_OK_TEXT, "[ OK ]");
      if (focused) wattroff(mWindow, A_REVERSE);
   }

   // "[ Cancel ]" text button
   {
      bool focused = (mFocusItem == FocusItem::CANCEL_BTN);
      if (focused)
         wattron(mWindow, A_REVERSE);
      mvwaddstr(mWindow, ROW_CONTROLS, COL_CANCEL_TEXT, "[ Cancel ]");
      if (focused)
         wattroff(mWindow, A_REVERSE);
   }
}

// ---------------------------------------------------------------------------
// Private: focus management
// ---------------------------------------------------------------------------

void cxOpenFileDialog::setFocusItem(FocusItem pItem)
{
   mFocusItem = pItem;
   drawContent();
   update_panels();
   doupdate();
}

// ---------------------------------------------------------------------------
// Private: input loop
// ---------------------------------------------------------------------------

long cxOpenFileDialog::doInputLoop()
{
   long returnCode = cxID_CANCEL;
   bool continueOn = true;

   while (continueOn)
   {
      int key = wgetch(mWindow);
      setLastKey(key);

      // ----- Tab / Shift-Tab: cycle focus -----
      if (key == TAB || key == SHIFT_TAB)
      {
         static const FocusItem order[] =
         {
            FocusItem::DIR_LIST,
            FocusItem::FILE_LIST,
            FocusItem::FILENAME_INPUT,
            FocusItem::FILETYPE,
            FocusItem::OK_BTN,
            FocusItem::CANCEL_BTN
         };
         constexpr int N = static_cast<int>(sizeof(order) / sizeof(order[0]));
         int cur = 0;
         for (int i = 0; i < N; ++i)
         {
            if (order[i] == mFocusItem) { cur = i; break; }
         }
         int next = (key == TAB) ? (cur + 1) % N : (cur + N - 1) % N;
         setFocusItem(order[next]);
         continue;
      }

      // ----- Escape: cancel -----
      if (key == ESCAPE)
      {
         returnCode = cxID_CANCEL;
         continueOn = false;
         break;
      }

      // ----- Focus-specific key handling -----
      switch (mFocusItem)
      {
         // ------- Directory list -------
         case FocusItem::DIR_LIST:
            switch (key)
            {
               case KEY_UP:    moveDirSelection(-1);          break;
               case KEY_DOWN:  moveDirSelection(1);           break;
               case KEY_PPAGE: moveDirSelection(-LIST_VISIBLE); break;
               case KEY_NPAGE: moveDirSelection(LIST_VISIBLE);  break;
               case KEY_HOME:  moveDirSelection(-(int)mDirEntries.size()); break;
               case KEY_END:   moveDirSelection((int)mDirEntries.size());  break;
               case ENTER:
               case KEY_ENTER:
                  enterSelectedDir();
                  break;
               default: break;
            }
            break;

         // ------- File list -------
         case FocusItem::FILE_LIST:
            switch (key)
            {
               case KEY_UP:    moveFileSelection(-1);          break;
               case KEY_DOWN:  moveFileSelection(1);           break;
               case KEY_PPAGE: moveFileSelection(-LIST_VISIBLE); break;
               case KEY_NPAGE: moveFileSelection(LIST_VISIBLE);  break;
               case KEY_HOME:  moveFileSelection(-(int)mFileEntries.size()); break;
               case KEY_END:   moveFileSelection((int)mFileEntries.size());  break;
               case ENTER:
               case KEY_ENTER:
                  // Select the file and confirm
                  if (!mFileEntries.empty() && mFileSelection >= 0 &&
                      mFileSelection < (int)mFileEntries.size())
                  {
                     mSelectedFileName = mFileEntries[mFileSelection].name;
                     returnCode = cxID_OK;
                     continueOn = false;
                  }
                  break;
               default: break;
            }
            break;

         // ------- Filename input -------
         case FocusItem::FILENAME_INPUT:
            if (key == ENTER || key == KEY_ENTER || key == ' ')
            {
               activateFilenameInput();
            }
            break;

         // ------- File type filter -------
         case FocusItem::FILETYPE:
            switch (key)
            {
               case KEY_LEFT: case KEY_UP:
                  cycleFilter(-1);
                  break;
               case KEY_RIGHT: case KEY_DOWN:
               case ENTER: case KEY_ENTER: case ' ':
                  cycleFilter(1);
                  break;
               default: break;
            }
            break;

         // ------- OK button -------
         case FocusItem::OK_BTN:
            if (key == ENTER || key == KEY_ENTER || key == ' ')
            {
               if (canAcceptSelection())
               {
                  returnCode = cxID_OK;
                  continueOn = false;
               }
            }
            break;

         // ------- Cancel button -------
         case FocusItem::CANCEL_BTN:
            if (key == ENTER || key == KEY_ENTER || key == ' ')
            {
               returnCode = cxID_CANCEL;
               continueOn = false;
            }
            break;
      } // end switch(mFocusItem)

      // ----- Mouse events -----
#ifdef NCURSES_MOUSE_VERSION
      if (continueOn && key == KEY_MOUSE)
      {
         if (getmouse(&mMouse) == OK)
         {
            int my = mMouse.y;
            int mx = mMouse.x;

            if (mMouse.bstate & (BUTTON1_CLICKED | BUTTON1_DOUBLE_CLICKED))
            {
               // Click in directory list
               if (my >= top() + ROW_LIST_FIRST &&
                   my <= top() + ROW_LIST_LAST &&
                   mx >= left() + DIR_COL_START &&
                   mx <  left() + COL_DIVIDER)
               {
                  int clickedIdx = mDirScrollOffset +
                                   (my - top() - ROW_LIST_FIRST);
                  if (clickedIdx >= 0 &&
                      clickedIdx < (int)mDirEntries.size())
                  {
                     mDirSelection = clickedIdx;
                     setFocusItem(FocusItem::DIR_LIST);
                     if (mMouse.bstate & BUTTON1_DOUBLE_CLICKED)
                     {
                        enterSelectedDir();
                     }
                  }
               }
               // Click in file list
               else if (my >= top() + ROW_LIST_FIRST &&
                        my <= top() + ROW_LIST_LAST &&
                        mx >= left() + FILE_COL_START &&
                        mx <= left() + DIALOG_WIDTH - 2)
               {
                  int clickedIdx = mFileScrollOffset +
                                   (my - top() - ROW_LIST_FIRST);
                  if (clickedIdx >= 0 &&
                      clickedIdx < (int)mFileEntries.size())
                  {
                     mFileSelection = clickedIdx;
                     mSelectedFileName = mFileEntries[mFileSelection].name;
                     setFocusItem(FocusItem::FILE_LIST);
                     if (mMouse.bstate & BUTTON1_DOUBLE_CLICKED)
                     {
                        returnCode = cxID_OK;
                        continueOn = false;
                     }
                  }
               }
               // Click on OK text button
               else if (my == top() + ROW_CONTROLS &&
                        mx >= left() + COL_OK_TEXT &&
                        mx <  left() + COL_OK_TEXT + OK_TEXT_W)
               {
                  setFocusItem(FocusItem::OK_BTN);
                  if (canAcceptSelection())
                  {
                     returnCode = cxID_OK;
                     continueOn = false;
                  }
               }
               // Click on Cancel text button
               else if (my == top() + ROW_CONTROLS &&
                        mx >= left() + COL_CANCEL_TEXT &&
                        mx <  left() + COL_CANCEL_TEXT + CANCEL_TEXT_W)
               {
                  setFocusItem(FocusItem::CANCEL_BTN);
                  returnCode = cxID_CANCEL;
                  continueOn = false;
               }
               // Click on filename area
               else if (my == top() + ROW_FILENAME &&
                        mx >= left() + COL_VALUE &&
                        mx <  left() + COL_VALUE + FILENAME_INPUT_W)
               {
                  setFocusItem(FocusItem::FILENAME_INPUT);
                  activateFilenameInput();
               }
               // Click on file type filter area
               else if (my == top() + ROW_CONTROLS &&
                        mx >= left() + COL_VALUE &&
                        mx <  left() + COL_VALUE + FILTER_DISPLAY_W)
               {
                  setFocusItem(FocusItem::FILETYPE);
                  cycleFilter(1);
               }
            }

            // Window dragging via title bar
            if (mMouse.bstate & BUTTON1_PRESSED)
            {
               if (mouseEvtWasInTitle())
               {
                  mMouse.x = mx;
                  mMouse.y = my;
               }
            }
         } // getmouse OK
      } // KEY_MOUSE
#endif



   } // while (continueOn)

   mLeaveNow = false;
   return returnCode;
}

// ---------------------------------------------------------------------------
// Private: list navigation
// ---------------------------------------------------------------------------

void cxOpenFileDialog::moveDirSelection(int pDelta)
{
   if (mDirEntries.empty()) return;

   int newSel = mDirSelection + pDelta;
   if (newSel < 0) newSel = 0;
   if (newSel >= (int)mDirEntries.size())
      newSel = (int)mDirEntries.size() - 1;

   mDirSelection = newSel;

   // Adjust scroll offset to keep selection visible
   if (mDirSelection < mDirScrollOffset)
      mDirScrollOffset = mDirSelection;
   if (mDirSelection >= mDirScrollOffset + LIST_VISIBLE)
      mDirScrollOffset = mDirSelection - LIST_VISIBLE + 1;

   drawContent();
   update_panels();
   doupdate();
}

void cxOpenFileDialog::moveFileSelection(int pDelta)
{
   if (mFileEntries.empty()) return;

   int newSel = mFileSelection + pDelta;
   if (newSel < 0) newSel = 0;
   if (newSel >= (int)mFileEntries.size())
      newSel = (int)mFileEntries.size() - 1;

   mFileSelection = newSel;

   // Adjust scroll offset to keep selection visible
   if (mFileSelection < mFileScrollOffset)
      mFileScrollOffset = mFileSelection;
   if (mFileSelection >= mFileScrollOffset + LIST_VISIBLE)
      mFileScrollOffset = mFileSelection - LIST_VISIBLE + 1;

   // Live-update the filename field
   mSelectedFileName = mFileEntries[mFileSelection].name;

   drawContent();
   update_panels();
   doupdate();
}

void cxOpenFileDialog::enterSelectedDir()
{
   if (mDirEntries.empty() || mDirSelection < 0 ||
       mDirSelection >= (int)mDirEntries.size())
   {
      return;
   }

   string dirName = mDirEntries[mDirSelection];

   try
   {
      fs::path newPath;
      if (dirName == "..")
      {
         newPath = fs::path(mCurrentPath).parent_path();
      }
      else
      {
         newPath = fs::path(mCurrentPath) / dirName;
      }

      if (fs::exists(newPath) && fs::is_directory(newPath))
      {
         mCurrentPath = fs::canonical(newPath).string();
         loadDirectory();
         mSelectedFileName.clear();
         drawContent();
         update_panels();
         doupdate();
      }
   }
   catch (...)
   {
      // Cannot enter directory (permissions, etc.)
   }
}

void cxOpenFileDialog::activateFilenameInput()
{
   if (mFileNameInput == nullptr)
      return;

   // Pre-populate with the current filename
   mFileNameInput->setValue(mSelectedFileName);

   // Show the input modally so the user can type
   /*long rc = */mFileNameInput->showModal(true, true, false);
   mFileNameInput->hide();

   // Only update if the user did not press Escape
   //if (rc != cxID_QUIT)
   if (mFileNameInput->getLastKey() != ESCAPE)
   {
      const string newVal = mFileNameInput->getValue(true, true);
      if (!newVal.empty())
      {
         try
         {
            fs::path typed(newVal);

            // Check for absolute path
            if (typed.is_absolute())
            {
               if (fs::exists(typed) && fs::is_directory(typed))
               {
                  // Navigate to that directory
                  mCurrentPath = fs::canonical(typed).string();
                  mSelectedFileName.clear();
                  loadDirectory();
               }
               else
               {
                  // Full path to a file (or new file)
                  if (mAllowNewFile)
                     mSelectedFileName = newVal;
                  else
                     mFileNameInput->clear();
               }
            }
            else
            {
               // Relative name - check if it is a subdirectory
               fs::path fullPath = fs::path(mCurrentPath) / typed;
               if (fs::exists(fullPath) && fs::is_directory(fullPath))
               {
                  mCurrentPath = fs::canonical(fullPath).string();
                  mSelectedFileName.clear();
                  loadDirectory();
               }
               else
               {
                  if (mAllowNewFile)
                     mSelectedFileName = newVal;
                  else
                     mFileNameInput->clear();
               }
            }
         }
         catch (...)
         {
            if (mAllowNewFile)
               mSelectedFileName = newVal;
            else
               mFileNameInput->clear();
         }
      }
      else
      {
         mSelectedFileName.clear();
      }
   }

   // Redraw with updated state
   drawContent();
   update_panels();
   doupdate();
}

void cxOpenFileDialog::cycleFilter(int pDelta)
{
   if (mFilters.empty()) return;

   mCurrentFilter += pDelta;
   if (mCurrentFilter < 0)
      mCurrentFilter = (int)mFilters.size() - 1;
   if (mCurrentFilter >= (int)mFilters.size())
      mCurrentFilter = 0;

   // Reload directory with the new filter applied
   loadDirectory();
   drawContent();
   update_panels();
   doupdate();
}

// ---------------------------------------------------------------------------
// Private: filter matching
// ---------------------------------------------------------------------------

bool cxOpenFileDialog::canAcceptSelection() const
{
   if (mSelectedFileName.empty())
      return false;
   if (mAllowNewFile)
      return true;

   // When new files are not allowed, the selected file must exist
   try
   {
      fs::path fullPath;
      if (!mSelectedFileName.empty() && mSelectedFileName[0] == '/')
         fullPath = mSelectedFileName;
      else
         fullPath = fs::path(mCurrentPath) / mSelectedFileName;
      return fs::exists(fullPath) && fs::is_regular_file(fullPath);
   }
   catch (...)
   {
      return false;
   }
}

bool cxOpenFileDialog::matchesFilter(const string& pFileName) const
{
   if (mFilters.empty() || mCurrentFilter < 0 ||
       mCurrentFilter >= (int)mFilters.size())
   {
      return true;
   }

   const string& pattern = mFilters[mCurrentFilter].pattern;
   if (pattern == "*" || pattern == "*.*") return true;

   // Pattern can be semicolon-separated, e.g. "*.cpp;*.h"
   size_t start = 0;
   while (start < pattern.size())
   {
      size_t end = pattern.find(';', start);
      if (end == string::npos) end = pattern.size();

      string pat = pattern.substr(start, end - start);
      // Trim whitespace
      while (!pat.empty() && pat.front() == ' ') pat.erase(0, 1);
      while (!pat.empty() && pat.back() == ' ')  pat.pop_back();

      if (pat == "*" || pat == "*.*") return true;

      // Handle "*.ext" pattern
      if (pat.size() >= 2 && pat[0] == '*')
      {
         string ext = pat.substr(1); // e.g. ".txt"
         if ((int)pFileName.size() >= (int)ext.size())
         {
            string fileTail = pFileName.substr(pFileName.size() - ext.size());
            bool match = true;
            for (size_t i = 0; i < ext.size(); ++i)
            {
               if (std::tolower(static_cast<unsigned char>(fileTail[i])) !=
                   std::tolower(static_cast<unsigned char>(ext[i])))
               {
                  match = false;
                  break;
               }
            }
            if (match) return true;
         }
      }

      start = end + 1;
   }

   return false;
}

// ---------------------------------------------------------------------------
// Private: utilities
// ---------------------------------------------------------------------------

string cxOpenFileDialog::formatFileSize(uintmax_t pSize)
{
   std::ostringstream oss;
   if (pSize < 1024)
   {
      oss << pSize << " B";
   }
   else if (pSize < 1024ULL * 1024)
   {
      oss << std::fixed << std::setprecision(1)
          << static_cast<double>(pSize) / 1024.0 << " KB";
   }
   else if (pSize < 1024ULL * 1024 * 1024)
   {
      oss << std::fixed << std::setprecision(1)
          << static_cast<double>(pSize) / (1024.0 * 1024.0) << " MB";
   }
   else
   {
      oss << std::fixed << std::setprecision(1)
          << static_cast<double>(pSize) / (1024.0 * 1024.0 * 1024.0) << " GB";
   }
   return oss.str();
}

string cxOpenFileDialog::truncStr(const string& pStr, int pMaxW)
{
   if (pMaxW <= 0) return "";
   if ((int)pStr.size() <= pMaxW) return pStr;
   if (pMaxW <= 3) return pStr.substr(0, pMaxW);
   return pStr.substr(0, pMaxW - 3) + "...";
}
