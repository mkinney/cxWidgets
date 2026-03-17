#ifndef __CXOBJECT_H__
#define __CXOBJECT_H__

// Copyright (c) 2005-2007 Michael H. Kinney
//
// This is the base class for everything in
//  the cxWidgets UI framework and contains
//  defaults for some things.

#include <ncurses.h>
#include <panel.h>

#include <cstdio>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>

#include "cxColors.h"
#include "cxKeyDefines.h"
#include "cxWidgetsException.h" // Included here so that
                                //  everything has access to it

typedef std::vector<std::string> stringContainer;
typedef std::vector<std::string>::iterator stringContainerIterator;

// Un-comment this line to include some test-only
//  helper functionality in classes where it might
//  be useful.
#define TESTS

/** \class cxObject
 * \brief The base class for windowing classes in the cxWidgets
 * \brief UI framework; contains defaults for some things.
 *
 * \author $Author: erico $
 * \version $Revision: 1.9 $
 * \date $Date: 2007/12/04 00:23:46 $
 */
class cxObject
{
   public:
      /**
       * Default constructor for cxObject
       */
      cxObject();

      /**
       * Destructor
       */
      virtual ~cxObject();

      /**
       * Returns whether or not colors are used
       * @return Boolean--true if colors used, false otherwise
       */
      static bool UseColors() { return useColors; }

      /**
       * Mutator--set to true to use colors
       * @param useIt Boolean--true to use colors
       */
      static void UseColors(bool useIt);

      /**
       * Returns whether or not beeps are used
       * @return Boolean--true means beeps used
       */
      static bool UseBeeps() { return useBeeps; }

      /**
       * Mutator--set to true to use beeps
       * @param useIt Boolean--set to true to use beeps
       */
      static void UseBeeps(bool useIt) { useBeeps = useIt; }

      /**
       * Outputs a beep character
       */
      virtual void beep();

      /**
       * \brief Returns the name of the cxWidgets class.  This is overridden
       * \brief in cxObject-based classes.  This can be used to determine
       * \brief the type of cxWidgets object that deriving classes derive from
       * \brief in applications.
       *
       * @return The name of the cxWidgets class.
       */
      virtual std::string cxTypeStr() const;

   protected:
      /**
       * \brief Whether or not to use colors
       */
      static bool useColors;
      /**
       * \brief Whether or not to enable beeping
       */
      static bool useBeeps;
};

#endif
