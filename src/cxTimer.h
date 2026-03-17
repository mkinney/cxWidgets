#ifndef __CXTIMER_H__
#define __CXTIMER_H__

// Copyright (c) 2005-2007 Michael H. Kinney
// Copyright (c) 2026 Eric N. Oulashin

#include "cxFunction.h"
#include <string>
#include <memory>
#include <mutex>
#include <atomic>

/**
 * \brief Represents a timer that will run a function after
 * \brief a specified number of milliseconds.  <b>This class is not
 * \brief finished yet and currently shouldn't be used</b>.
 */
class cxTimer
{
   public:
      /**
       * \brief Constructor
       *
       * @param pFuncPtr A pointer to the function to run once the time is up
       * @param pDelayMS The amount of time (in milliseconds) to delay before firing the function
       * @param pWaitInSeparateThread Whether or not to wait in a separate thread. Defaults to true.
       */
      cxTimer(const std::shared_ptr<cxFunction>& pFuncPtr, unsigned int pDelayMS, bool pWaitInSeparateThread = true);

      virtual ~cxTimer();

      /**
       * \brief Sets/changes the function pointer
       *
       * @param pFuncPtr A pointer to the function to run once the time is up
       */
      virtual void function(const std::shared_ptr<cxFunction>& pFuncPtr);

      /**
       * \brief Returns the function pointer that is set in the object
       *
       * @return The function pointer that is set in the object
       */
      virtual std::shared_ptr<cxFunction> function() const;

      /**
       * \brief Sets the delay (in MS) for the timeout
       *
       * @param pDelay The delay (in MS) for the timeout
       */
      virtual void delay(unsigned int pDelay);

      /**
       * \brief Returns the delay (in MS) that is set in the object
       *
       * @return The delay (in MS) that is set in the object
       */
      virtual unsigned int delay() const;

      /**
       * \brief Sets whether or not to wait in a separate thread
       *
       * @param pWaitInSeparateThread Whether or not to wait in a separate thread
       */
      virtual void waitInSeparateThread(bool pWaitInSeparateThread);

      /**
       * \brief Returns the setting for whether or not to wait in a separate thread
       *
       * @return The setting for whether or not to wait in a separate thread
       */
      virtual bool waitInSeparateThread() const;

      /**
       * \brief Starts the timer
       */
      virtual void start();

      /**
       * \brief Stops the timer
       *
       * @param pRunFunction Boolean: Whether to still run the function. Defaults to true.
       */
      virtual void stop(bool pRunFunction = true);

      /**
       * \brief Returns whether the timer object is currently waiting for time to elapse
       */
      virtual bool isWaiting() const;

      /**
       * \brief Returns the name of the cxWidgets class.  This can be used to
       * \brief determine the type of cxWidgets object that deriving classes
       * \brief derive from in applications.
       *
       * @return The name of the cxWidgets class.
       */
      virtual std::string cxTypeStr() const;

   private:
      std::shared_ptr<cxFunction> mFunction; // The function to run after the delay
      unsigned int mDelay;                   // The delay in MS
      bool mWaitInSeparateThread;            // Whether or not to wait in a separate thread
      int mPID = -1;                         // The process ID of wait()
      std::atomic<bool> mIsWaiting{false};               // Status indicator for whether the timer is currently waiting for time to elapse
      std::atomic<bool> mRunFunctionAfterWaiting{true};  // Control for whether to run the function after the time elapses
      std::mutex mWaitMutex;                 // Mutex to be used in wait(), to lock anything used in there

      // Waits a number of milliseconds equal to mDelay
      // and then runs the function.
      void wait();
};

#endif
