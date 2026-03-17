#ifndef __CXFUNCTION_H__
#define __CXFUNCTION_H__

/*
 * cxFunction.h - function
 *
 * Copyright (C) 2005-2007 Michael H. Kinney
 *
 * \author $Author: erico $
 * \version $Revision: 1.16 $
 * \date $Date: 2007/12/04 00:23:46 $
 *
 */

#include <string>
#include <memory>

// Typedefs for various function pointer types
typedef std::string (*funcPtr0)();
typedef std::string (*funcPtr2)(void *p1, void *p2);
typedef std::string (*funcPtr4)(void *p1, void *p2, void *p3, void *p4);

/**
 * \class cxFunction
 * \brief Base class for cxFunction2 and cxFunction4.  This class is pure
 * \brief virtual, and this can be used in collections where a set of
 * \brief cxFunction2 and/or cxFunction4 objects are needed.
 */
class cxFunction
{
   public:
      /**
       * \brief Constructor
       *
       * @param pUseReturnVal Whether or not the caller should use the return
       *  value of the function
       * @param pExitAfterRun Whether or not the caller should exit after the
       *  function is run
       * @param pRunOnLeaveFunction Whether or not the caller should run its
       *  onLeave function after the function runs (if pExitAfterRun is true)
       */
      cxFunction(bool pUseReturnVal, bool pExitAfterRun, bool pRunOnLeaveFunction);

      virtual ~cxFunction();

      /**
       * Returns whether the internal function pointer is set (non-null).
       * @return Returns true if the internal function pointer is set (not null), or false otherwise.
       */
      virtual bool functionIsSet() const = 0;

      /**
       * \brief Runs the function pointed to by the cxFunction.  This is a
       * \brief pure virtual method and must be overridden in deriving classes.
       *
       * @return Returns return value of function pointed to if pointer is not null,
       *		otherwise returns empty string.
       */
      virtual std::string runFunction() const = 0;

      virtual void* getFuncPtr() const = 0;

      /**
       * \brief Accessor for whether the caller should use the return value
       * \brief of the function.
       *
       * @return Whether or not the caller should use the return value of
       *  the function
       */
      virtual bool getUseReturnVal() const;

      /**
       * \brief Setter for whether or not the caller should make use of the return value.
       *
       * @param pUseReturnVal Whether or nto the caller should make use of
       *  the return value
       */
      virtual void setUseReturnVal(bool pUseReturnVal);

      /**
       * \brief Accessor for whether the caller should exit after the function
       * \brief is run.
       *
       * @return Whether or not the caller should exit after the function is run
       */
      virtual bool getExitAfterRun() const;

      /**
       * \brief Setter for whether or not the caller should quit what
       * \brief it's doing (i.e., input loops, etc.) after the function is run.
       *
       * @param pExitAfterRun Whether or nto the caller should quit what it's
       *  doing after the function is run.
       */
      virtual void setExitAfterRun(bool pExitAfterRun);

      /**
       * \brief Accessor for whether the caller should run its onLeave function
       * \brief after the function is run (if exitAfterRun is true).
       *
       * @return Whether or not the caller should run its onLeav function
       *  after the function is run (if exitAfterRun is true).
       */
      virtual bool getRunOnLeaveFunction() const;

      /**
       * \brief Setter for whether or not the caller should run its
       * \brief onLeave function when it exits after running the function.
       * \brief This is useful if mExitAfterRun is true.
       *
       * @param pRunOnLeaveFunction Whether or not the caller should run
       *  its onLeave function when it exits after running the function.
       */
      virtual void setRunOnLeaveFunction(bool pRunOnLeaveFunction);

      /**
       * \brief Returns the name of the class (either "cxFunction2"
       * \brief or "cxFunction4", depending on the derived implementation).
       *
       * @return The name of the cxWidgets class.
       */
      virtual std::string cxTypeStr() const = 0;

   private:
      bool mUseReturnVal;
      bool mExitAfterRun;
      bool mRunOnLeaveFunction;
}; // end of class cxFunction

/** \class cxFunction0
 * \brief A more intelligent function pointer, which can run a
 * \brief function that takes no parameters.  Note that
 * \brief the function must have the following signature:<br>
 * \brief  string func()<br>
 *
 * \author Eric Oulashin
 * \author Mike Kinney
 * \version $Revision: 1.16 $
 * \date $Date: 2007/12/04 00:23:46 $
*/
class cxFunction0 : public cxFunction
{
   public:
      /**
       * \brief Default constructor.  All parameters have default values
       * \brief available.  Note that function must have this signature:
       * \brief string func(void*, void*)
       *
       * @param pFuncPtr Pointer to the function to be run.  Defaults to null.
       * @param pUseReturnVal Indicates whether caller will make use of return
       *  value.  Defaults to false
       * @param pExitAfterRun Whether or not the caller should exit from its
       *  input loop once the function is done.  Defaults to false.
       * @param pRunOnLeaveFunction Whether or not the caller should run its
       *  onLeave function when it exits (useful if pExitAfterRun is true).
       *  This defaults to true.
       */
      explicit cxFunction0(funcPtr0 pFuncPtr = nullptr, bool pUseReturnVal = false,
                           bool pExitAfterRun = false,
                           bool pRunOnLeaveFunction = true);

      /**
       * Destructor
       */
      virtual ~cxFunction0();

      /**
       * Returns whether the internal funtion pointer is set.
       * @return Returns true if the internal function pointer is set (not null), or false otherwise.
       */
      virtual bool functionIsSet() const override;

      /**
       * Sets the internal function pointer
       * @param pFuncPtr The function to which to point
       */
      void setFunction(funcPtr0 pFuncPtr);

      /**
       * \brief Accessor for the internal function pointer
       *
       * @return The internal function pointer
       */
      funcPtr0 getFunction() const;

      /**
       * If function pointer is not null, runs the function and returns its return value;
       * If function pointer is nullptr, returns empty string
       * @return Returns return value of function pointed to if pointer is not null,
       *		otherwise returns empty string
       */
      virtual std::string runFunction() const override;

      virtual void* getFuncPtr() const override;

      /**
       * \brief Returns the name of the cxWidgets class, "cxFunction0".  This can be
       * \brief used to determine the type of cxWidgets object that deriving
       * \brief classes derive from in applications.
       *
       * @return The name of the cxWidgets class ("cxFunction0").
       */
      virtual std::string cxTypeStr() const override;

   private:
      funcPtr0 mFunction; // This is the actual function pointer
}; // end of class cxFunction2

/** \class cxFunction2
 * \brief A more intelligent function pointer, which can run a function with
 * \brief 2 parameters.  This is used for running external code for an event
 * \brief (such as a keypress,  etc.).  This class also stores whether the
 * \brief function's return value should be used.  Note that the function must
 * \brief have the following signature:<br>
 * \brief   string func(void*, void*)<br>
 *  The parameters are void pointers
 *  because it can't be known by this
 *  class what types of objects your
 *  functions will need.
 *  This class lets you set the function
 *  pointer, a boolean to signify whether
 *  the caller should make use of the
 *  return value, and also set the
 *  parameters to be passed to the
 *  function.  This class was originally
 *  intended for use with cxInput and
 *  cxMultiLineInput, so this class
 *  also has a boolean to specify whether
 *  the input should exit its input
 *  upon exit of the function.
 *
 * \author Eric Oulashin
 * \author Mike Kinney
 * \version $Revision: 1.16 $
 * \date $Date: 2007/12/04 00:23:46 $
*/
class cxFunction2 : public cxFunction
{
   public:
      /**
       * \brief Default constructor.  All parameters have default values
       * \brief available.  Note that function must have this signature:
       * \brief string func(void*, void*)
       *
       * @param pFuncPtr Pointer to the function to be run.  Defaults to nullptr.
       * @param pParam1 The first parameter to pass to the function when it's
       *  run - This is a void pointer.
       * @param pParam2 The second parameter to pass to the function when it's
       *  run - This is a void pointer.
       * @param pUseReturnVal Indicates whether caller will make use of return
       *  value.  Defaults to false
       * @param pExitAfterRun Whether or not the caller should exit from its
       *  input loop once the function is done.  Defaults to false.
       * @param pRunOnLeaveFunction Whether or not the caller should run its
       *  onLeave function when it exits (useful if pExitAfterRun is true).
       *  This defaults to true.
       */
      explicit cxFunction2(funcPtr2 pFuncPtr = nullptr, void *pParam1 = nullptr,
                           void *pParam2 = nullptr, bool pUseReturnVal = false,
                           bool pExitAfterRun = false,
                           bool pRunOnLeaveFunction = true);

      /**
       * Destructor
       */
      virtual ~cxFunction2();

      /**
       * Returns whether the internal funtion pointer is set.
       * @return Returns true if the internal function pointer is set (not null), or false otherwise.
       */
      virtual bool functionIsSet() const override;

      /**
       * Sets the internal function pointer
       * @param pFuncPtr The function to which to point
       */
      void setFunction(funcPtr2 pFuncPtr);

      /**
       * \brief Accessor for the internal function pointer
       *
       * @return The internal function pointer
       */
      funcPtr2 getFunction() const;

      /**
       * Sets the parameters to pass to the function.
       * @param pParam1 First parameter to pass to function
       * @param pParam2 Second parameter to pass to function
       */
      void setParams(void *pParam1, void *pParam2);

      /**
       * Sets the first parameter
       * @param pParam First parameter to pass to function
       */
      void setParam1(void *pParam);

      /**
       * Sets the second parameter
       * @param pParam Second parameter to pass to function
       */
      void setParam2(void *pParam);

      /**
       * Accessor for the first function parameter
       * @return Returns pointer to first parameter
       */
      void* getParam1() const;

      /**
       * Accessor for the second function pointer
       * @return Returns pointer to second parameter
       */
      void* getParam2() const;

      /**
       * If function pointer is not null, runs the function and returns its return value;
       * If function pointer is nullptr, returns empty string
       * @return Returns return value of function pointed to if pointer is not null,
       *		otherwise returns empty string
       */
      virtual std::string runFunction() const override;

      virtual void* getFuncPtr() const override;

      /**
       * \brief Returns the name of the cxWidgets class, "cxFunction2".  This can be
       * \brief used to determine the type of cxWidgets object that deriving
       * \brief classes derive from in applications.
       *
       * @return The name of the cxWidgets class ("cxFunction2").
       */
      virtual std::string cxTypeStr() const override;

   private:
      funcPtr2 mFunction; // The actual function pointer
      void *mParam1;  // The first parameter to pass to the function
      void *mParam2;  // The second parameter to pass to the function
}; // end of class cxFunction2

/** \class cxFunction4
 * \brief This class is just like cxFunction2, but for functions
 * \brief  with 4 void pointer parameters instead of 2.
 *
*/
class cxFunction4 : public cxFunction
{
   public:
      /**
       * \brief Default constructor.  All parameters have default values
       * \brief available.  Note that function must have this signature:
       * \brief string func(void*, void*)
       *
       * @param pFuncPtr Pointer to the function to be run.  Defaults to nullptr.
       * @param pParam1 The first parameter to pass to the function when it's
       *  run - This is a void pointer.
       * @param pParam2 The second parameter to pass to the function when it's
       *  run - This is a void pointer.
       * @param pParam3 The 3rd parameter to pass to the function when it's
       *  run - This is a void pointer.
       * @param pParam4 The 4th parameter to pass to the function when it's
       *  run - This is a void pointer.
       * @param pUseReturnVal Indicates whether caller will make use of return
       *  value.  Defaults to false
       * @param pExitAfterRun Whether or not the caller should exit from its
       *  input loop once the function is done.  Defaults to false.
       * @param pRunOnLeaveFunction Whether or not the caller should run its
       *  onLeave function when it exits (useful if pExitAfterRun is true).
       *  This defaults to true.
       */
      explicit cxFunction4(funcPtr4 pFuncPtr = nullptr, void *pParam1 = nullptr,
                           void *pParam2 = nullptr, void *pParam3 = nullptr,
                           void *pParam4 = nullptr, bool pUseReturnVal = false,
                           bool pExitAfterRun = false,
                           bool pRunOnLeaveFunction = true);

      /**
       * Destructor
       */
      virtual ~cxFunction4();

      /**
       * Returns whether the internal funtion pointer is set.
       * @return Returns true if the internal function pointer is set (not null), or false otherwise.
       */
      virtual bool functionIsSet() const override;

      /**
       * Sets the internal function pointer
       * @param pFuncPtr The function to which to point
       */
      void setFunction(funcPtr4 pFuncPtr);

      /**
       * \brief Accessor for the internal function pointer
       *
       * @return The internal function pointer
       */
      funcPtr4 getFunction() const;

      /**
       * Sets the parameters to pass to the function.
       * @param pParam1 First parameter to pass to function
       * @param pParam2 Second parameter to pass to function
       * @param pParam3 3rd parameter to pass to function
       * @param pParam4 4th parameter to pass to function
       */
      void setParams(void *pParam1, void *pParam2, void *pParam3, void *pParam4);

      /**
       * Sets the first parameter
       * @param pParam First parameter to pass to function
       */
      void setParam1(void *pParam);

      /**
       * Sets the second parameter
       * @param pParam Second parameter to pass to function
       */
      void setParam2(void *pParam);

      /**
       * Sets the 3rd parameter
       * @param pParam 3rd parameter to pass to function
       */
      void setParam3(void *pParam);

      /**
       * Sets the 4th parameter
       * @param pParam 4th parameter to pass to function
       */
      void setParam4(void *pParam);

      /**
       * Accessor for the first function parameter
       * @return Returns pointer to first parameter
       */
      void* getParam1() const;

      /**
       * Accessor for the second function parameter
       * @return Returns pointer to second parameter
       */
      void* getParam2() const;

      /**
       * Accessor for the 3rd function parameter
       * @return Returns pointer to first parameter
       */
      void* getParam3() const;

      /**
       * Accessor for the 4th function parameter
       * @return Returns pointer to second parameter
       */
      void* getParam4() const;

      /**
       * If function pointer is not null, runs the function and returns its return value;
       * If function pointer is nullptr, returns empty string
       * @return Returns return value of function pointed to if pointer is not null,
       *    otherwise returns empty string
       */
      virtual std::string runFunction() const override;

      virtual void* getFuncPtr() const override;

      /**
       * \brief Returns the name of the cxWidgets class, "cxFunction4".  This can be
       * \brief used to determine the type of cxWidgets object that deriving
       * \brief classes derive from in applications.
       *
       * @return The name of the cxWidgets class ("cxFunction4").
       */
      virtual std::string cxTypeStr() const override;

   private:
      funcPtr4 mFunction; // The actual function pointer
      void *mParam1;  // The first parameter to pass to the function
      void *mParam2;  // The second parameter to pass to the function
      void *mParam3;  // The 3rd parameter to pass to the function
      void *mParam4;  // The 4th parameter to pass to the function
};

/** \class cxFunction2Templated
 * \brief This class is a cxFunction that is templated to take 2 pointers of specific types
 *
*/
template<typename T1, typename T2>
class cxFunction2Templated : public cxFunction
{
   public:
      using FuncPtr = std::string (*)(T1 *, T2 *);

      /**
       * \brief Static create method for cxFunction2Templated.  This is the recommended way to create a cxFunction2Templated, as it returns a shared pointer to the created object.
       */
      static std::shared_ptr<cxFunction2Templated<T1, T2>> create(FuncPtr pFuncPtr, T1 *pParam1, T2 *pParam2,
                                      bool pUseReturnVal = false, bool pExitAfterRun = false,
                                      bool pRunOnLeaveFunction = true)
                                      {
         return std::make_shared<cxFunction2Templated<T1, T2>>(pFuncPtr, pParam1, pParam2,
                                      pUseReturnVal, pExitAfterRun, pRunOnLeaveFunction);
      }

      /**
       * \brief Default constructor.  All parameters have default values
       * \brief available.  Note that function must have this signature:
       * \brief string func(void*, void*)
       *
       * @param pFuncPtr Pointer to the function to be run.  Defaults to nullptr.
       * @param pParam1 The first parameter to pass to the function when it's
       *  run - This is a void pointer.
       * @param pParam2 The second parameter to pass to the function when it's
       *  run - This is a void pointer.
       * @param pUseReturnVal Indicates whether caller will make use of return
       *  value.  Defaults to false
       * @param pExitAfterRun Whether or not the caller should exit from its
       *  input loop once the function is done.  Defaults to false.
       * @param pRunOnLeaveFunction Whether or not the caller should run its
       *  onLeave function when it exits (useful if pExitAfterRun is true).
       *  This defaults to true.
       */
       explicit cxFunction2Templated(FuncPtr pFuncPtr = nullptr, T1 *pParam1 = nullptr,
                                      T2 *pParam2 = nullptr, bool pUseReturnVal = false,
                                      bool pExitAfterRun = false,
                                      bool pRunOnLeaveFunction = true)
           : cxFunction(pUseReturnVal, pExitAfterRun, pRunOnLeaveFunction),
             mFunction(pFuncPtr),
             mParam1(pParam1),
             mParam2(pParam2)
        {
        }

      /**
       * Destructor
       */
      virtual ~cxFunction2Templated() { }

      /**
       * Returns whether the internal funtion pointer is set.
       * @return Returns true if the internal function pointer is set (not null), or false otherwise.
       */
      virtual bool functionIsSet() const override
      {
         return (mFunction != nullptr);
      }

      /**
       * Sets the internal function pointer
       * @param pFuncPtr The function to which to point
       */
      void setFunction(FuncPtr pFuncPtr)
      {
         mFunction = pFuncPtr;
      }

      /**
       * \brief Accessor for the internal function pointer
       *
       * @return The internal function pointer
       */
      FuncPtr getFunction() const
      {
         return mFunction;
      }

      /**
       * Sets the parameters to pass to the function.
       * @param pParam1 First parameter to pass to function
       * @param pParam2 Second parameter to pass to function
       */
      void setParams(T1 *pParam1, T2 *pParam2)
      {
         mParam1 = pParam1;
         mParam2 = pParam2;
      }

      /**
       * Sets the first parameter
       * @param pParam First parameter to pass to function
       */
      void setParam1(T1 *pParam)
      {
         mParam1 = pParam;
      }

      /**
       * Sets the second parameter
       * @param pParam Second parameter to pass to function
       */
      void setParam2(T2 *pParam)
      {
         mParam2 = pParam;
      }

      /**
       * Accessor for the first function parameter
       * @return Returns pointer to first parameter
       */
      T1* getParam1() const
      {
         return mParam1;
      }

      /**
       * Accessor for the second function parameter
       * @return Returns pointer to second parameter
       */
      T2* getParam2() const
      {
         return mParam2;
      }

      /**
       * If function pointer is not null, runs the function and returns its return value;
       * If function pointer is nullptr, returns empty string
       * @return Returns return value of function pointed to if pointer is not null,
       *    otherwise returns empty string
       */
      virtual std::string runFunction() const override
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

      virtual void* getFuncPtr() const override
      {
         return (void*)mFunction;
      }

      /**
       * \brief Returns the name of the cxWidgets class, "cxFunction2Templated".  This can be
       * \brief used to determine the type of cxWidgets object that deriving
       * \brief classes derive from in applications.
       *
       * @return The name of the cxWidgets class ("cxFunction2Templated").
       */
      virtual std::string cxTypeStr() const override
      {
         return("cxFunction2Templated");
      }

   private:
      FuncPtr mFunction; // The actual function pointer
      T1 *mParam1;  // The first parameter to pass to the function
      T2 *mParam2;  // The second parameter to pass to the function
};

/** \class cxFunction1RefTemplated
 * \brief This class is a cxFunction that is templated to take a reference of a specific type
 *
*/
template<typename T1>
class cxFunction1RefTemplated : public cxFunction
{
   public:
      using FuncPtr = std::string (*)(T1&);

      static std::shared_ptr<cxFunction1RefTemplated<T1>> create(FuncPtr pFuncPtr, T1& pParam1,
                                      bool pUseReturnVal = false, bool pExitAfterRun = false,
                                      bool pRunOnLeaveFunction = true)
                                      {
         return std::make_shared<cxFunction1RefTemplated<T1>>(pFuncPtr, pParam1,
                                      pUseReturnVal, pExitAfterRun, pRunOnLeaveFunction);
      }

      /**
       * \brief Default constructor.  All parameters have default values
       * \brief available.  Note that function must have this signature:
       * \brief string func(void*, void*)
       *
       * @param pFuncPtr Pointer to the function to be run.  Defaults to nullptr.
       * @param pParam1 The first parameter to pass to the function when it's
       *  run
       * @param pUseReturnVal Indicates whether caller will make use of return
       *  value.  Defaults to false
       * @param pExitAfterRun Whether or not the caller should exit from its
       *  input loop once the function is done.  Defaults to false.
       * @param pRunOnLeaveFunction Whether or not the caller should run its
       *  onLeave function when it exits (useful if pExitAfterRun is true).
       *  This defaults to true.
       */
       explicit cxFunction1RefTemplated(FuncPtr pFuncPtr, T1& pParam,
                                      bool pUseReturnVal = false,
                                      bool pExitAfterRun = false,
                                      bool pRunOnLeaveFunction = true)
           : cxFunction(pUseReturnVal, pExitAfterRun, pRunOnLeaveFunction),
             mFunction(pFuncPtr),
             mParam(pParam)
        {
        }

      /**
       * Destructor
       */
      virtual ~cxFunction1RefTemplated() { }

      /**
       * Returns whether the internal funtion pointer is set.
       * @return Returns true if the internal function pointer is set (not null), or false otherwise.
       */
      virtual bool functionIsSet() const override
      {
         return (mFunction != nullptr);
      }

      /**
       * Sets the internal function pointer
       * @param pFuncPtr The function to which to point
       */
      void setFunction(FuncPtr pFuncPtr)
      {
         mFunction = pFuncPtr;
      }

      /**
       * \brief Accessor for the internal function pointer
       *
       * @return The internal function pointer
       */
      FuncPtr getFunction() const
      {
         return mFunction;
      }

      /**
       * Sets the parameter to pass to the function.
       * @param pParam The parameter to pass to function
       */
      void setParam(T1& pParam)
      {
         mParam = pParam;
      }

      /**
       * Accessor for the function parameter
       * @return Returns the first parameter
       */
      const T1& getParam() const
      {
         return mParam;
      }

      /**
       * If function pointer is not null, runs the function and returns its return value;
       * If function pointer is nullptr, returns empty string
       * @return Returns return value of function pointed to if pointer is not null,
       *    otherwise returns empty string
       */
      virtual std::string runFunction() const override
      {
         if (mFunction != nullptr)
         {
            return(mFunction(mParam));
         }
         else
         {
            return("");
         }
      }

      virtual void* getFuncPtr() const override
      {
         return (void*)mFunction;
      }

      /**
       * \brief Returns the name of the cxWidgets class, "cxFunction1RefTemplated".  This can be
       * \brief used to determine the type of cxWidgets object that deriving
       * \brief classes derive from in applications.
       *
       * @return The name of the cxWidgets class ("cxFunction1RefTemplated").
       */
      virtual std::string cxTypeStr() const override
      {
         return("cxFunction1RefTemplated");
      }

   private:
      FuncPtr mFunction; // The actual function pointer
      T1& mParam;  // The parameter to pass to the function
};


/** \class cxFunction2RefTemplated
 * \brief This class is a cxFunction that is templated to take 2 references of specific types
 *
*/
template<typename T1, typename T2>
class cxFunction2RefTemplated : public cxFunction
{
   public:
      using FuncPtr = std::string (*)(T1&, T2&);

      /**
       * \brief Static create method for cxFunction2RefTemplated.  This is the recommended way to create a cxFunction2RefTemplated, as it returns a shared pointer to the created object.
       */
      static std::shared_ptr<cxFunction2RefTemplated<T1, T2>> create(FuncPtr pFuncPtr, T1& pParam1, T2& pParam2,
                                      bool pUseReturnVal = false, bool pExitAfterRun = false,
                                      bool pRunOnLeaveFunction = true)
                                      {
         return std::make_shared<cxFunction2RefTemplated<T1, T2>>(pFuncPtr, pParam1, pParam2,
                                      pUseReturnVal, pExitAfterRun, pRunOnLeaveFunction);
      }

      /**
       * \brief Default constructor.  All parameters have default values
       * \brief available.  Note that function must have this signature:
       * \brief string func(void*, void*)
       *
       * @param pFuncPtr Pointer to the function to be run.  Defaults to nullptr.
       * @param pParam1 The first parameter to pass to the function when it's
       *  run
       * @param pParam2 The second parameter to pass to the function when it's
       *  run
       * @param pUseReturnVal Indicates whether caller will make use of return
       *  value.  Defaults to false
       * @param pExitAfterRun Whether or not the caller should exit from its
       *  input loop once the function is done.  Defaults to false.
       * @param pRunOnLeaveFunction Whether or not the caller should run its
       *  onLeave function when it exits (useful if pExitAfterRun is true).
       *  This defaults to true.
       */
       explicit cxFunction2RefTemplated(FuncPtr pFuncPtr, T1& pParam1,
                                      T2 &pParam2, bool pUseReturnVal = false,
                                      bool pExitAfterRun = false,
                                      bool pRunOnLeaveFunction = true)
           : cxFunction(pUseReturnVal, pExitAfterRun, pRunOnLeaveFunction),
             mFunction(pFuncPtr),
             mParam1(pParam1),
             mParam2(pParam2)
        {
        }

      /**
       * Destructor
       */
      virtual ~cxFunction2RefTemplated() { }

      /**
       * Returns whether the internal funtion pointer is set.
       * @return Returns true if the internal function pointer is set (not null), or false otherwise.
       */
      virtual bool functionIsSet() const override
      {
         return (mFunction != nullptr);
      }

      /**
       * Sets the internal function pointer
       * @param pFuncPtr The function to which to point
       */
      void setFunction(FuncPtr pFuncPtr)
      {
         mFunction = pFuncPtr;
      }

      /**
       * \brief Accessor for the internal function pointer
       *
       * @return The internal function pointer
       */
      FuncPtr getFunction() const
      {
         return mFunction;
      }

      /**
       * Sets the parameters to pass to the function.
       * @param pParam1 First parameter to pass to function
       * @param pParam2 Second parameter to pass to function
       */
      void setParams(T1& pParam1, T2& pParam2)
      {
         mParam1 = pParam1;
         mParam2 = pParam2;
      }

      /**
       * Sets the first parameter
       * @param pParam First parameter to pass to function
       */
      void setParam1(T1& pParam)
      {
         mParam1 = pParam;
      }

      /**
       * Sets the second parameter
       * @param pParam Second parameter to pass to function
       */
      void setParam2(T2 &pParam)
      {
         mParam2 = pParam;
      }

      /**
       * Accessor for the first function parameter
       * @return Returns the first parameter
       */
      const T1& getParam1() const
      {
         return mParam1;
      }

      /**
       * Accessor for the second function parameter
       * @return Returns the second parameter
       */
      const T2& getParam2() const
      {
         return mParam2;
      }

      /**
       * If function pointer is not null, runs the function and returns its return value;
       * If function pointer is nullptr, returns empty string
       * @return Returns return value of function pointed to if pointer is not null,
       *    otherwise returns empty string
       */
      virtual std::string runFunction() const override
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

      virtual void* getFuncPtr() const override
      {
         return (void*)mFunction;
      }

      /**
       * \brief Returns the name of the cxWidgets class, "cxFunction2RefTemplated".  This can be
       * \brief used to determine the type of cxWidgets object that deriving
       * \brief classes derive from in applications.
       *
       * @return The name of the cxWidgets class ("cxFunction2RefTemplated").
       */
      virtual std::string cxTypeStr() const override
      {
         return("cxFunction2RefTemplated");
      }

   private:
      FuncPtr mFunction; // The actual function pointer
      T1& mParam1;  // The first parameter to pass to the function
      T2& mParam2;  // The second parameter to pass to the function
};

/** \class cxFunction3RefTemplated
 * \brief This class is a cxFunction that is templated to take 2 references of specific types
 *
*/
template<typename T1, typename T2, typename T3>
class cxFunction3RefTemplated : public cxFunction
{
   public:
      using FuncPtr = std::string (*)(T1&, T2&, T3&);

      /**
       * \brief Static create method for cxFunction3RefTemplated.  This is the recommended way to create a cxFunction3RefTemplated, as it returns a shared pointer to the created object.
       */
      static std::shared_ptr<cxFunction3RefTemplated<T1, T2, T3>> create(FuncPtr pFuncPtr, T1& pParam1, T2& pParam2, T3& pParam3,
                                      bool pUseReturnVal = false, bool pExitAfterRun = false,
                                      bool pRunOnLeaveFunction = true)
                                      {
         return std::make_shared<cxFunction3RefTemplated<T1, T2, T3>>(pFuncPtr, pParam1, pParam2, pParam3,
                                      pUseReturnVal, pExitAfterRun, pRunOnLeaveFunction);
      }

      /**
       * \brief Default constructor.  All parameters have default values
       * \brief available.  Note that function must have this signature:
       * \brief string func(void*, void*)
       *
       * @param pFuncPtr Pointer to the function to be run.  Defaults to nullptr.
       * @param pParam1 The first parameter to pass to the function when it's
       *  run
       * @param pParam2 The second parameter to pass to the function when it's
       *  run
       * @param pParam3 The third parameter to pass to the function when it's
       *  run
       * @param pUseReturnVal Indicates whether caller will make use of return
       *  value.  Defaults to false
       * @param pExitAfterRun Whether or not the caller should exit from its
       *  input loop once the function is done.  Defaults to false.
       * @param pRunOnLeaveFunction Whether or not the caller should run its
       *  onLeave function when it exits (useful if pExitAfterRun is true).
       *  This defaults to true.
       */
       explicit cxFunction3RefTemplated(FuncPtr pFuncPtr, T1& pParam1,
                                      T2& pParam2, T3& pParam3,
                                      bool pUseReturnVal = false,
                                      bool pExitAfterRun = false,
                                      bool pRunOnLeaveFunction = true)
           : cxFunction(pUseReturnVal, pExitAfterRun, pRunOnLeaveFunction),
             mFunction(pFuncPtr),
             mParam1(pParam1),
             mParam2(pParam2),
             mParam3(pParam3)
        {
        }

      /**
       * Destructor
       */
      virtual ~cxFunction3RefTemplated() { }

      /**
       * Returns whether the internal funtion pointer is set.
       * @return Returns true if the internal function pointer is set (not null), or false otherwise.
       */
      virtual bool functionIsSet() const override
      {
         return (mFunction != nullptr);
      }

      /**
       * Sets the internal function pointer
       * @param pFuncPtr The function to which to point
       */
      void setFunction(FuncPtr pFuncPtr)
      {
         mFunction = pFuncPtr;
      }

      /**
       * \brief Accessor for the internal function pointer
       *
       * @return The internal function pointer
       */
      FuncPtr getFunction() const
      {
         return mFunction;
      }

      /**
       * Sets the parameters to pass to the function.
       * @param pParam1 First parameter to pass to function
       * @param pParam2 Second parameter to pass to function
       * @param pParam3 Third parameter to pass to function
       */
      void setParams(T1& pParam1, T2& pParam2, T3& pParam3)
      {
         mParam1 = pParam1;
         mParam2 = pParam2;
         mParam3 = pParam3;
      }

      /**
       * Sets the first parameter
       * @param pParam First parameter to pass to function
       */
      void setParam1(T1& pParam)
      {
         mParam1 = pParam;
      }

      /**
       * Sets the second parameter
       * @param pParam Second parameter to pass to function
       */
      void setParam2(T2& pParam)
      {
         mParam2 = pParam;
      }

      /**
       * Sets the third parameter
       * @param pParam Second parameter to pass to function
       */
      void setParam2(T3& pParam)
      {
         mParam3 = pParam;
      }

      /**
       * Accessor for the first function parameter
       * @return Returns the first parameter
       */
      const T1& getParam1() const
      {
         return mParam1;
      }

      /**
       * Accessor for the second function parameter
       * @return Returns the second parameter
       */
      const T2& getParam2() const
      {
         return mParam2;
      }

      /**
       * Accessor for the third function parameter
       * @return Returns the second parameter
       */
      const T3& getParam3() const
      {
         return mParam3;
      }

      /**
       * If function pointer is not null, runs the function and returns its return value;
       * If function pointer is nullptr, returns empty string
       * @return Returns return value of function pointed to if pointer is not null,
       *    otherwise returns empty string
       */
      virtual std::string runFunction() const override
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

      virtual void* getFuncPtr() const override
      {
         return (void*)mFunction;
      }

      /**
       * \brief Returns the name of the cxWidgets class, "cxFunction3RefTemplated".  This can be
       * \brief used to determine the type of cxWidgets object that deriving
       * \brief classes derive from in applications.
       *
       * @return The name of the cxWidgets class ("cxFunction3RefTemplated").
       */
      virtual std::string cxTypeStr() const override
      {
         return("cxFunction3RefTemplated");
      }

   private:
      FuncPtr mFunction; // The actual function pointer
      T1& mParam1;  // The first parameter to pass to the function
      T2& mParam2;  // The second parameter to pass to the function
      T3& mParam3;  // The third parameter to pass to the function
};

/** \class cxFunction4Templated
 * \brief This class is a cxFunction that is templated to take 4 pointers of specific types
 *
*/
template<typename T1, typename T2, typename T3, typename T4>
class cxFunction4Templated : public cxFunction
{
   public:
      using FuncPtr = std::string (*)(T1 *, T2 *, T3 *, T4 *);

      /**
       * \brief Static create method for cxFunction4Templated.  This is the recommended way to create a cxFunction4Templated, as it returns a shared pointer to the created object.
       */
      static std::shared_ptr<cxFunction4Templated<T1, T2, T3, T4>> create(FuncPtr pFuncPtr, T1 *pParam1, T2 *pParam2,
                                      T3 *pParam3, T4 *pParam4, bool pUseReturnVal = false,
                                      bool pExitAfterRun = false, bool pRunOnLeaveFunction = true)
                                      {
         return std::make_shared<cxFunction4Templated<T1, T2, T3, T4>>(pFuncPtr, pParam1, pParam2,
                                      pParam3, pParam4, pUseReturnVal, pExitAfterRun,
                                      pRunOnLeaveFunction);
      }

      /**
       * \brief Default constructor.  All parameters have default values
       * \brief available.  Note that function must have this signature:
       * \brief string func(void*, void*)
       *
       * @param pFuncPtr Pointer to the function to be run.  Defaults to nullptr.
       * @param pParam1 The first parameter to pass to the function when it's
       *  run - This is a void pointer.
       * @param pParam2 The second parameter to pass to the function when it's
       *  run - This is a void pointer.
       * @param pParam3 The 3rd parameter to pass to the function when it's
       *  run - This is a void pointer.
       * @param pParam4 The 4th parameter to pass to the function when it's
       *  run - This is a void pointer.
       * @param pUseReturnVal Indicates whether caller will make use of return
       *  value.  Defaults to false
       * @param pExitAfterRun Whether or not the caller should exit from its
       *  input loop once the function is done.  Defaults to false.
       * @param pRunOnLeaveFunction Whether or not the caller should run its
       *  onLeave function when it exits (useful if pExitAfterRun is true).
       *  This defaults to true.
       */
       explicit cxFunction4Templated(FuncPtr pFuncPtr = nullptr, T1 *pParam1 = nullptr,
                                      T2 *pParam2 = nullptr, T3 *pParam3 = nullptr,
                                      T4 *pParam4 = nullptr, bool pUseReturnVal = false,
                                      bool pExitAfterRun = false,
                                      bool pRunOnLeaveFunction = true)
           : cxFunction(pUseReturnVal, pExitAfterRun, pRunOnLeaveFunction),
             mFunction(pFuncPtr),
             mParam1(pParam1),
             mParam2(pParam2),
             mParam3(pParam3),
             mParam4(pParam4)
        {
        }

      /**
       * Destructor
       */
      virtual ~cxFunction4Templated() { }

      /**
       * Returns whether the internal funtion pointer is set.
       * @return Returns true if the internal function pointer is set (not null), or false otherwise.
       */
      virtual bool functionIsSet() const override
      {
         return (mFunction != nullptr);
      }

      /**
       * Sets the internal function pointer
       * @param pFuncPtr The function to which to point
       */
      void setFunction(FuncPtr pFuncPtr)
      {
         mFunction = pFuncPtr;
      }

      /**
       * \brief Accessor for the internal function pointer
       *
       * @return The internal function pointer
       */
      FuncPtr getFunction() const
      {
         return mFunction;
      }

      /**
       * Sets the parameters to pass to the function.
       * @param pParam1 First parameter to pass to function
       * @param pParam2 Second parameter to pass to function
       * @param pParam3 3rd parameter to pass to function
       * @param pParam4 4th parameter to pass to function
       */
      void setParams(T1 *pParam1, T2 *pParam2, T3 *pParam3, T4 *pParam4)
      {
         mParam1 = pParam1;
         mParam2 = pParam2;
         mParam3 = pParam3;
         mParam4 = pParam4;
      }

      /**
       * Sets the first parameter
       * @param pParam First parameter to pass to function
       */
      void setParam1(T1 *pParam)
      {
         mParam1 = pParam;
      }

      /**
       * Sets the second parameter
       * @param pParam Second parameter to pass to function
       */
      void setParam2(T2 *pParam)
      {
         mParam2 = pParam;
      }

      /**
       * Sets the 3rd parameter
       * @param pParam 3rd parameter to pass to function
       */
      void setParam3(T3 *pParam)
      {
         mParam3 = pParam;
      }

      /**
       * Sets the 4th parameter
       * @param pParam 4th parameter to pass to function
       */
      void setParam4(T4 *pParam)
      {
         mParam4 = pParam;
      }

      /**
       * Accessor for the first function parameter
       * @return Returns pointer to first parameter
       */
      T1* getParam1() const
      {
         return mParam1;
      }

      /**
       * Accessor for the second function parameter
       * @return Returns pointer to second parameter
       */
      T2* getParam2() const
      {
         return mParam2;
      }

      /**
       * Accessor for the 3rd function parameter
       * @return Returns pointer to first parameter
       */
      T3* getParam3() const
      {
         return mParam3;
      }

      /**
       * Accessor for the 4th function parameter
       * @return Returns pointer to second parameter
       */
      T4* getParam4() const
      {
         return mParam4;
      }

      /**
       * If function pointer is not null, runs the function and returns its return value;
       * If function pointer is nullptr, returns empty string
       * @return Returns return value of function pointed to if pointer is not null,
       *    otherwise returns empty string
       */
      virtual std::string runFunction() const override
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

      virtual void* getFuncPtr() const override
      {
         return (void*)mFunction;
      }

      /**
       * \brief Returns the name of the cxWidgets class, "cxFunction4Templated".  This can be
       * \brief used to determine the type of cxWidgets object that deriving
       * \brief classes derive from in applications.
       *
       * @return The name of the cxWidgets class ("cxFunction4Templated").
       */
      virtual std::string cxTypeStr() const override
      {
         return("cxFunction4Templated");
      }

   private:
      FuncPtr mFunction; // The actual function pointer
      T1 *mParam1;  // The first parameter to pass to the function
      T2 *mParam2;  // The second parameter to pass to the function
      T3 *mParam3;  // The 3rd parameter to pass to the function
      T4 *mParam4;  // The 4th parameter to pass to the function
};

/** \class cxFunction4RefTemplated
 * \brief This class is a cxFunction that is templated to take 4 references of specific types
 *
*/
template<typename T1, typename T2, typename T3, typename T4>
class cxFunction4RefTemplated : public cxFunction
{
   public:
      using FuncPtr = std::string (*)(T1&, T2&, T3&, T4&);

      /**
       * \brief Static create method for cxFunction4RefTemplated.  This is the recommended way to create a cxFunction4RefTemplated, as it returns a shared pointer to the created object.
       */
      static std::shared_ptr<cxFunction4RefTemplated<T1, T2, T3, T4>> create(FuncPtr pFuncPtr, T1& pParam1, T2& pParam2,
                                      T3& pParam3, T4& pParam4, bool pUseReturnVal = false,
                                      bool pExitAfterRun = false, bool pRunOnLeaveFunction = true)
                                      {
         return std::make_shared<cxFunction4RefTemplated<T1, T2, T3, T4>>(pFuncPtr, pParam1, pParam2,
                                      pParam3, pParam4, pUseReturnVal, pExitAfterRun,
                                      pRunOnLeaveFunction);
      }

      /**
       * \brief Default constructor.  All parameters have default values
       * \brief available.  Note that function must have this signature:
       * \brief string func(void*, void*)
       *
       * @param pFuncPtr Pointer to the function to be run.  Defaults to nullptr.
       * @param pParam1 The first parameter to pass to the function when it's
       *  run
       * @param pParam2 The second parameter to pass to the function when it's
       *  run
       * @param pParam3 The 3rd parameter to pass to the function when it's
       *  run
       * @param pParam4 The 4th parameter to pass to the function when it's
       *  run
       * @param pUseReturnVal Indicates whether caller will make use of return
       *  value.  Defaults to false
       * @param pExitAfterRun Whether or not the caller should exit from its
       *  input loop once the function is done.  Defaults to false.
       * @param pRunOnLeaveFunction Whether or not the caller should run its
       *  onLeave function when it exits (useful if pExitAfterRun is true).
       *  This defaults to true.
       */
       explicit cxFunction4RefTemplated(FuncPtr pFuncPtr, T1& pParam1,
                                      T2& pParam2, T3& pParam3, T4& pParam4,
                                      bool pUseReturnVal = false,
                                      bool pExitAfterRun = false,
                                      bool pRunOnLeaveFunction = true)
           : cxFunction(pUseReturnVal, pExitAfterRun, pRunOnLeaveFunction),
             mFunction(pFuncPtr),
             mParam1(pParam1),
             mParam2(pParam2),
             mParam3(pParam3),
             mParam4(pParam4)
        {
        }

      /**
       * Destructor
       */
      virtual ~cxFunction4RefTemplated() { }

      /**
       * Returns whether the internal funtion pointer is set.
       * @return Returns true if the internal function pointer is set (not null), or false otherwise.
       */
      virtual bool functionIsSet() const override
      {
         return (mFunction != nullptr);
      }

      /**
       * Sets the internal function pointer
       * @param pFuncPtr The function to which to point
       */
      void setFunction(FuncPtr pFuncPtr)
      {
         mFunction = pFuncPtr;
      }

      /**
       * \brief Accessor for the internal function pointer
       *
       * @return The internal function pointer
       */
      FuncPtr getFunction() const
      {
         return mFunction;
      }

      /**
       * Sets the parameters to pass to the function.
       * @param pParam1 First parameter to pass to function
       * @param pParam2 Second parameter to pass to function
       * @param pParam3 Third parameter to pass to function
       * @param pParam4 Fourth parameter to pass to function
       */
      void setParams(T1& pParam1, T2& pParam2, T3& pParam3, T4& pParam4)
      {
         mParam1 = pParam1;
         mParam2 = pParam2;
         mParam3 = pParam3;
         mParam4 = pParam4;
      }

      /**
       * Sets the first parameter
       * @param pParam First parameter to pass to function
       */
      void setParam1(T1& pParam)
      {
         mParam1 = pParam;
      }

      /**
       * Sets the second parameter
       * @param pParam Second parameter to pass to function
       */
      void setParam2(T2& pParam)
      {
         mParam2 = pParam;
      }

      /**
       * Sets the 3rd parameter
       * @param pParam Third parameter to pass to function
       */
      void setParam3(T3& pParam)
      {
         mParam3 = pParam;
      }

      /**
       * Sets the 4th parameter
       * @param pParam Fourth parameter to pass to function
       */
      void setParam4(T4& pParam)
      {
         mParam4 = pParam;
      }

      /**
       * Accessor for the first function parameter
       * @return Returns the 1st parameter
       */
      const T1& getParam1() const
      {
         return mParam1;
      }

      /**
       * Accessor for the second function parameter
       * @return Returns the 2nd parameter
       */
      const T2& getParam2() const
      {
         return mParam2;
      }

      /**
       * Accessor for the 3rd function parameter
       * @return Returns the 3rd parameter
       */
      const T3& getParam3() const
      {
         return mParam3;
      }

      /**
       * Accessor for the 4th function parameter
       * @return Returns the 4th parameter
       */
      const T4& getParam4() const
      {
         return mParam4;
      }

      /**
       * If function pointer is not null, runs the function and returns its return value;
       * If function pointer is nullptr, returns empty string
       * @return Returns return value of function pointed to if pointer is not null,
       *    otherwise returns empty string
       */
      virtual std::string runFunction() const override
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

      virtual void* getFuncPtr() const override
      {
         return (void*)mFunction;
      }

      /**
       * \brief Returns the name of the cxWidgets class, "cxFunction4RefTemplated".  This can be
       * \brief used to determine the type of cxWidgets object that deriving
       * \brief classes derive from in applications.
       *
       * @return The name of the cxWidgets class ("cxFunction4RefTemplated").
       */
      virtual std::string cxTypeStr() const override
      {
         return("cxFunction4RefTemplated");
      }

   private:
      FuncPtr mFunction; // The actual function pointer
      T1& mParam1;  // The first parameter to pass to the function
      T2& mParam2;  // The second parameter to pass to the function
      T3& mParam3;  // The third parameter to pass to the function
      T4& mParam4;  // The fourth parameter to pass to the function
};

#endif
