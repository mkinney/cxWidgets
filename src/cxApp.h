#ifndef __CXAPP_H__
#define __CXAPP_H__

/*
 *
 * cxApp.h - application framework
 *
 * Copyright (c) 2005-2007 Michael H. Kinney
 *
 * \author $Author: erico $
 * \version $Revision: 1.10 $
 * \date $Date: 2007/12/04 00:23:45 $
 */

#include "cxObject.h"
#include "cxFrame.h"
#include <memory>

/**
 * \brief Holds application setup & exit logic, as well as the application's main frame.
 * \brief <b>This class is not finished yet and currently shouldn't be used.</a>
 */
class cxApp : public cxObject
{
   public:
      cxApp();

      virtual ~cxApp();

      /**
       * \brief Enters the main loop.
       *
       * @return Return code
       */
      virtual long mainLoop();

      /**
       * \brief This virtual function is where the execution of a program written with cxWidgets starts. The default implementation just enters the main loop and starts handling the events until it terminates.
       *
       * @return Return code
       */
      virtual long onRun();

      /**
       * \brief Synonym for onRun().
       *
       * @return Return value of onRun()
       */
      virtual long run();

      /**
       * \brief Application initialization & startup commands will go here.
       *
       * @return Return code
       */
      virtual long onInit();

      /**
       * \brief Things to be done upon applicatoin exit will go here.
       *
       * @return
       */
      virtual long onExit();

      /**
       * \brief Returns the name of the cxWidgets class.  This is overridden
       * \brief in cxObject-based classes.  This can be used to determine
       * \brief the type of cxWidgets object that deriving classes derive from
       * \brief in applications.
       *
       * @return The name of the cxWidgets class.
       */
      virtual std::string cxTypeStr() const override;

   private:
      std::shared_ptr<cxFrame> mAppFrame; // The application's frame
};

#endif
