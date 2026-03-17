#include "cxApp.h"
// Note: This is not complete!

cxApp::cxApp()
{
}

cxApp::~cxApp()
{
}

long cxApp::mainLoop()
{
   return(cxID_EXIT);
}

long cxApp::onRun()
{
   return(cxID_EXIT);
}

long cxApp::run()
{
   return(onRun());
}

long cxApp::onInit()
{
   return(cxID_EXIT);
}

long cxApp::onExit()
{
   return(cxID_EXIT);
}

std::string cxApp::cxTypeStr() const
{
   return("cxApp");
} // cxTypeStr