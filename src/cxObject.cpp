// Copyright (c) 2005-2007 Michael H. Kinney

#include "cxObject.h"

bool cxObject::useColors(true);
bool cxObject::useBeeps(true);

cxObject::cxObject()
{
}

cxObject::~cxObject()
{
}

// Mutator for useColors
void cxObject::UseColors(bool useIt)
{
   useColors = useIt && has_colors();
}

// Outputs a beep character
void cxObject::beep()
{
   if (useBeeps)
   {
      printf("\007");
   }
} // beep

std::string cxObject::cxTypeStr() const
{
   return("cxObject");
} // cxTypeStr
