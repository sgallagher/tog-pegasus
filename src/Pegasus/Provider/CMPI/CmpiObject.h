//%2003////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002  BMC Software, Hewlett-Packard Development
// Company, L. P., IBM Corp., The Open Group, Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L. P.;
// IBM Corp.; EMC Corporation, The Open Group.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// THE ABOVE COPYRIGHT NOTICE AND THIS PERMISSION NOTICE SHALL BE INCLUDED IN
// ALL COPIES OR SUBSTANTIAL PORTIONS OF THE SOFTWARE. THE SOFTWARE IS PROVIDED
// "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
// LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//==============================================================================
//
// Author:      Adrian Schuur, schuur@de.ibm.com
//
// Modified By: Heidi Neuman, heidineu@de.ibm.com
//              Angel Nunez Mencias, anunez@de.ibm.com
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef _CmpiObject_h_
#define _CmpiObject_h_

#include "cmpidt.h"
#include "cmpift.h"

#ifndef CmpiBoolean
#define CmpiBoolean CMPIBoolean
#define CmpiRc      CMPIrc
#endif

//#include "CmpiBaseMI.h"
#include "CmpiString.h"

class CmpiBroker;

/** Abstract base class for all Cmpi classes.
*/

class CmpiObject {
   friend class CmpiBroker;
   friend class CmpiObjectPath;
   friend class CmpiInstance;
  protected:
   /** Protected pointer to encapsulated CMPI instance
   */
    void *enc;
   /** Constructor - Do nothing
   */
   CmpiObject() : enc(0) {}
   /** Constructor - Normal base class constructor
   */
   inline CmpiObject(const void* enc)
      { this->enc=(void*)enc; }
   CmpiString doToString(CMPIBroker *mb);
   CmpiBoolean doIsA(CMPIBroker *mb, const char *typeName) const;
  private:
  public:
   /** isNull - Test for valid encapsualtion pointer
   */
   inline CmpiBoolean isNull() const
      { return (enc==NULL); }
   /** toString - Produc;s CIMOM specific string representation of object
   */
   inline CmpiString toString() {
      return doToString(CmpiProviderBase::getBroker());
   }
   /** isA - checks this objects type   ;
   */
   inline CmpiBoolean isA(const char *typeName) const  {
      return doIsA(CmpiProviderBase::getBroker(),typeName);
   }
};

#endif

