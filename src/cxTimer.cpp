// Copyright (c) 2005-2007 Michael H. Kinney
// Copyright (c) 2026 Eric N. Oulashin

#include "cxTimer.h"
#include <thread>
#include <time.h>
using std::string;
using std::shared_ptr;
using std::lock_guard;

cxTimer::cxTimer(const shared_ptr<cxFunction>& pFuncPtr, unsigned int pDelayMS, bool pWaitInSeparateThread)
   : mFunction(pFuncPtr),
     mDelay(pDelayMS),
     mWaitInSeparateThread(pWaitInSeparateThread)
{
   if (mFunction != nullptr)
   {
     start();
   }
}

cxTimer::~cxTimer()
{
}

void cxTimer::function(const shared_ptr<cxFunction>& pFuncPtr)
{
   lock_guard<std::mutex> lock(mWaitMutex);
   mFunction = pFuncPtr;
}

shared_ptr<cxFunction> cxTimer::function() const
{
   return(mFunction);
}

void cxTimer::delay(unsigned int pDelay)
{
   lock_guard<std::mutex> lock(mWaitMutex);
   mDelay = pDelay;
}

unsigned int cxTimer::delay() const
{
   return mDelay;
}

void cxTimer::waitInSeparateThread(bool pWaitInSeparateThread)
{
   lock_guard<std::mutex> lock(mWaitMutex);
   mWaitInSeparateThread = pWaitInSeparateThread;
}

bool cxTimer::waitInSeparateThread() const
{
   return(mWaitInSeparateThread);
}

void cxTimer::start()
{
   if (!mIsWaiting.load() && mFunction != nullptr)
   {
      wait();
   }
}

void cxTimer::stop(bool pRunFunction)
{
   mRunFunctionAfterWaiting.store(pRunFunction);
   mIsWaiting.store(false);
}

bool cxTimer::isWaiting() const
{
   return(mIsWaiting.load());
}

string cxTimer::cxTypeStr() const
{
   return("cxTimer");
} // cxTypeStr

void cxTimer::wait()
{
   mRunFunctionAfterWaiting = true; // Default
   // If the function pointer is null, then do nothing & return now
   if (mFunction == nullptr)
   {
      mIsWaiting = false;
      return;
   }

   // A lambda/anonymous function to wait until the correct time.
   // mIsWaiting could be set back to false by stop().
   auto doWaitLoop = [&]() {
      // Lock on mWaitMutex for the duration of this function.
      // This will protect everything in here in case
      // someone tries to run multiple instances of this function
      // in separate threads.
      lock_guard<std::mutex> lock(mWaitMutex);

      if (mFunction == nullptr) { // Check just in case
         mIsWaiting = false;
         return;
      }

      // Figure out the number of clocks per millisecond,
      // the current time, and the time we need to stop
      // waiting.
      const unsigned long clocksPerMS = CLOCKS_PER_SEC / 1000;
      clock_t now = clock();
      const clock_t endTime = now + (mDelay * clocksPerMS);

      mIsWaiting = true;
      while (now < endTime && mIsWaiting)
      {
         now = clock();
      }
      mIsWaiting = false;

      // If mRunFunctionAfterWaiting is true, then run the function
      if (mRunFunctionAfterWaiting && mFunction != nullptr)
      {
         mFunction->runFunction();
      }
      mRunFunctionAfterWaiting = true; // Reset back to default
   };

   // Perform the wait & run in a separate thread if configured to do so;
   // otherwise, run it in the current thread.
   if (mWaitInSeparateThread)
   {
      std::thread t(doWaitLoop);
      t.detach();
   }
   else
   {
      doWaitLoop();
   }
}
