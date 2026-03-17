/*
 * keys.cpp - test the keys (helpful for testing terminal emulation programs)
 *
 *  Copyright (c) 2006-2007 Michael H. Kinney
 */
#include "cxBase.h"
using namespace cxBase;
#include "cxWindow.h"
#include "cxStringUtils.h"
using namespace cxStringUtils;

#include <stdlib.h>
#include <iostream>
#include <string>
using namespace std;

//// MAIN ////
int main(int argc, char* argv[])
{
   cxBase::init();

   cxObject ourcxObject;
   ourcxObject.UseColors(true);

   bool continueOn=true;
   int key=0;
   string keyStr;


   while (continueOn)
   {
      mvwprintw(stdscr, 0, 0, "Press ESC to quit.");
      refresh();
      key=getch();
      erase();
      mvwprintw(stdscr, 0, 0, "Press ESC to quit.");
      keyStr=cxBase::getKeyStr(key);
      if (keyStr == "ESC")
      {
         continueOn=false;
      }
      mvwprintw(stdscr, 1, 0, "You pressed:%d which is:%s", key, keyStr.c_str());
      refresh();

   }

   // cleanup
   cxBase::cleanup();
   return(0);
} // main

