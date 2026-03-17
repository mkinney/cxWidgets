// Copyright (c) 2005-2007 Michael H. Kinney

#include "cxWidgetsException.h"
using std::string;

cxWidgetsException::cxWidgetsException(const string& pErrorMsg)
   : mErrorMsg(pErrorMsg)
{
}

cxWidgetsException::~cxWidgetsException()
{
}

const string& cxWidgetsException::getErrorMsg() const
{
   return(mErrorMsg);
}

string cxWidgetsException::cxTypeStr() const
{
   return("cxWidgetsException");
} // cxTypeStr
