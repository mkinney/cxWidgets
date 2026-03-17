// Copyright (c) 2005-2007 Michael H. Kinney
//

#include "cxFunction.h"
using std::string;

//// cxFunction class methods

cxFunction::cxFunction(bool pUseReturnVal, bool pExitAfterRun, bool pRunOnLeaveFunction)
   : mUseReturnVal(pUseReturnVal),
     mExitAfterRun(pExitAfterRun),
     mRunOnLeaveFunction(pRunOnLeaveFunction)
{
}

cxFunction::~cxFunction()
{
}

bool cxFunction::getUseReturnVal() const
{
   return(mUseReturnVal);
}

void cxFunction::setUseReturnVal(bool pUseReturnVal)
{
   mUseReturnVal = pUseReturnVal;
}

bool cxFunction::getExitAfterRun() const
{
   return(mExitAfterRun);
}

void cxFunction::setExitAfterRun(bool pExitAfterRun)
{
   mExitAfterRun = pExitAfterRun;
}

bool cxFunction::getRunOnLeaveFunction() const
{
   return(mRunOnLeaveFunction);
}

void cxFunction::setRunOnLeaveFunction(bool pRunOnLeaveFunction)
{
   mRunOnLeaveFunction = pRunOnLeaveFunction;
}

//// cxFunction0 class methods

cxFunction0::cxFunction0(funcPtr0 pFuncPtr, bool pUseReturnVal,
                         bool pExitAfterRun, bool pRunOnLeaveFunction)
   : cxFunction(pUseReturnVal, pExitAfterRun, pRunOnLeaveFunction),
     mFunction(pFuncPtr)
{
}

cxFunction0::~cxFunction0()
{
}

bool cxFunction0::functionIsSet() const
{
   return(mFunction != nullptr);
}

void cxFunction0::setFunction(funcPtr0 pFuncPtr)
{
   mFunction = pFuncPtr;
} // setFunction

funcPtr0 cxFunction0::getFunction() const
{
   return(mFunction);
} // getFunction

// Runs the function.
string cxFunction0::runFunction() const
{
   if (mFunction != nullptr)
   {
      return(mFunction());
   }
   else
   {
      return("");
   }
}

void* cxFunction0::getFuncPtr() const
{
   return (void*)mFunction;
}

string cxFunction0::cxTypeStr() const
{
   return("cxFunction0");
} // cxTypeStr

//// cxFunction2 class methods

cxFunction2::cxFunction2(funcPtr2 pFuncPtr, void *pParam1, void *pParam2,
                         bool pUseReturnVal, bool pExitAfterRun,
                         bool pRunOnLeaveFunction)
   : cxFunction(pUseReturnVal, pExitAfterRun, pRunOnLeaveFunction),
     mFunction(pFuncPtr),
     mParam1(pParam1),
     mParam2(pParam2)
{
}

cxFunction2::~cxFunction2()
{
}

bool cxFunction2::functionIsSet() const
{
   return(mFunction != nullptr);
}

void cxFunction2::setFunction(funcPtr2 pFuncPtr)
{
   mFunction = pFuncPtr;
}

funcPtr2 cxFunction2::getFunction() const
{
   return(mFunction);
} // getFunction

// Sets the parameters to pass to the function.
void cxFunction2::setParams(void *pParam1, void *pParam2)
{
   mParam1 = pParam1;
   mParam2 = pParam2;
}

void cxFunction2::setParam1(void *pParam)
{
   mParam1 = pParam;
}

void cxFunction2::setParam2(void *pParam)
{
   mParam2 = pParam;
}

// Accessor for the first function parameter
void* cxFunction2::getParam1() const
{
   return(mParam1);
}

// Accessor for the second function parameter
void* cxFunction2::getParam2() const
{
   return(mParam2);
}

// Runs the function.
string cxFunction2::runFunction() const
{
   if (mFunction != nullptr)
   {
      return(mFunction(mParam1, mParam2));
   }
   else
   {
      return("");
   }
}

void* cxFunction2::getFuncPtr() const
{
   return (void*)mFunction;
}

string cxFunction2::cxTypeStr() const
{
   return("cxFunction2");
} // cxTypeStr

//// cxFunction4 class methods

cxFunction4::cxFunction4(funcPtr4 pFuncPtr, void *pParam1, void *pParam2,
                         void *pParam3, void *pParam4, bool pUseReturnVal,
                         bool pExitAfterRun, bool pRunOnLeaveFunction)
   : cxFunction(pUseReturnVal, pExitAfterRun, pRunOnLeaveFunction),
     mFunction(pFuncPtr),
     mParam1(pParam1),
     mParam2(pParam2),
     mParam3(pParam3),
     mParam4(pParam4)
{
}

cxFunction4::~cxFunction4()
{
}

bool cxFunction4::functionIsSet() const
{
   return(mFunction != nullptr);
}

void cxFunction4::setFunction(funcPtr4 pFuncPtr)
{
   mFunction = pFuncPtr;
}

funcPtr4 cxFunction4::getFunction() const
{
   return mFunction;
}

// Sets the parameters to pass to the function.
void cxFunction4::setParams(void *pParam1, void *pParam2, void *pParam3, void *pParam4)
{
   mParam1 = pParam1;
   mParam2 = pParam2;
   mParam3 = pParam3;
   mParam4 = pParam4;
}

void cxFunction4::setParam1(void *pParam)
{
   mParam1 = pParam;
}

void cxFunction4::setParam2(void *pParam)
{
   mParam2 = pParam;
}

void cxFunction4::setParam3(void *pParam)
{
   mParam3 = pParam;
}

void cxFunction4::setParam4(void *pParam)
{
   mParam4 = pParam;
}

// Accessor for the first function parameter
void* cxFunction4::getParam1() const
{
   return(mParam1);
}

// Accessor for the second function parameter
void* cxFunction4::getParam2() const
{
   return(mParam2);
}

// Accessor for the 3rd function parameter
void* cxFunction4::getParam3() const
{
   return(mParam3);
}

// Accessor for the 4th function parameter
void* cxFunction4::getParam4() const
{
   return(mParam4);
}

// Runs the function.
string cxFunction4::runFunction() const
{
   if (mFunction != nullptr)
   {
      return(mFunction(mParam1, mParam2, mParam3, mParam4));
   }
   else
   {
      return("");
   }
}

void* cxFunction4::getFuncPtr() const
{
   return (void*)mFunction;
}

string cxFunction4::cxTypeStr() const
{
   return("cxFunction4");
} // cxTypeStr
