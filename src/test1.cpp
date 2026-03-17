// Copyright (c) 2005-2007 Michael H. Kinney
//
#include "cxBase.h"
using namespace cxBase;
#include "cxDialog.h"
#include "cxMessageDialog.h"
#include "cxForm.h"
#include "cxWindow.h"
#include "cxButton.h"
#include "cxFileViewer.h"
#include "cxMenu.h"
#include "cxInput.h"
#include "cxMultiLineInput.h"
#include "cxStringUtils.h"
using namespace cxStringUtils;

#include <unistd.h> //  for sleep()
#include <stdlib.h>

#include <iostream>
using namespace std;

/*
string arrowUp(void *theForm, void *theCount, void *foo3, void *foo4) {
   //pForm->setCurrentInput(bName);
   return("");
}

string endB1(void *theForm, void *theCount, void *foo3, void *foo4) {
   int *i=(int*)theCount;
   cxForm *pForm=(cxForm*)theForm;
   (*i)++;
   std::ostringstream os;
   os << (*i);
   string aName="A" + os.str();
   string bName="B" + os.str();
   //messageBox("bName:" + bName);
   pForm->append(*i, 1, 1, 5, "", "", "", eINPUT_EDITABLE, aName);
   pForm->append(*i, 7, 1, 5, "", "", "", eINPUT_EDITABLE, bName);
   pForm->setOnLeaveFunction(bName, endB1, pForm, i, nullptr, nullptr, false);
   if (*i > 1) {
      pForm->setOnKeyFunction(bName, arrowUp, pForm, i, nullptr, nullptr, false);
   }
   pForm->setCurrentInput(bName);
   //pForm->show();
   return("");
}
*/

string updateMenu(void *theComboBox, void *unused, void *unused2, void *unused3)
{
   if (theComboBox == nullptr)
   {
      return("");
   }

   cxComboBox *pComboBox = (cxComboBox*)theComboBox;
   pComboBox->append(pComboBox->getValue(), true, false);

   return("");
} // updateMenu

string onFocus(void *theComboBox, void *unused2, void *unused3, void *unused4)
{
   //messageBox("onFocus function");
   if (theComboBox == nullptr)
   {
      return("");
   }

   cxComboBox *pComboBox = (cxComboBox*)theComboBox;
   pComboBox->removeAllItems();
   pComboBox->append("111", true, false);
   pComboBox->append("222", true, false);
   pComboBox->append("333", true, false);
   pComboBox->append("444", true, false);

   return ("");
} // onFocus


//// MAIN ////
int main(int argc, char* argv[])
{
   cxBase::init();

   cxForm iForm(nullptr, 2, 2, 9, 20, "Test");
   iForm.append(1, 1, 1, 15, "A:");
   iForm.appendComboBox(2, 1, 1, 15, "B:");
   iForm.append(3, 1, 1, 15, "C:");
   iForm.append(4, 1, 1, 15, "D:");

   // Enable reverse video for the inputs
   iForm.addAttr(eDATA_EDITABLE, A_REVERSE);

   //iForm.setOnKeyFunction("B:", updateMenu, (cxComboBox*)iForm.getInput("B:"), nullptr, nullptr, nullptr);
   iForm.setOnFocusFunction("B:", onFocus, (cxComboBox*)iForm.getInput("B:"), nullptr, nullptr, nullptr, false);

   iForm.showModal();

   /*
   cxObject ourcxObject;
   ourcxObject.UseColors(true);

   int i=1;
   cxForm iForm(nullptr, 2, 2, 8, 20, "Test form");
   iForm.append(1, 1, 1, 5, "", "", "", eINPUT_EDITABLE, "A1");
   iForm.append(1, 7, 1, 5, "", "", "", eINPUT_EDITABLE, "B1");
   iForm.setOnLeaveFunction("B1", endB1, &iForm, &i, nullptr, nullptr, false);
   iForm.setAutoExit(true);
   bool continueOn=true;
   long choice=0;
   while (continueOn) {
      choice=iForm.showModal();
      if (choice == cxID_QUIT) {
         continueOn=false;
      }
   }
   */

   cxBase::cleanup();
   return (0);
} // main

