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

#ifndef _CmpiString_h_
#define _CmpiString_h_

#include "cmpidt.h"
#include "cmpift.h"
#include <string.h>

#ifndef CmpiBoolean
#define CmpiBoolean CMPIBoolean
#define CmpiRc      CMPIrc
#endif

//#include "CmpiObject.h"
//#include "CmpiObjectPath.h"
#include "CmpiStatus.h"
#include "Linkage.h"

#ifdef PEGASUS_PLATFORM_WIN32_IX86_MSVC
#define strcasecmp _stricmp
#endif

/** This class wraps a CIMOM specific string representation.
*/

class PEGASUS_CMPI_PROVIDER_LINKAGE CmpiString { //: public CmpiObject {
   friend class CmpiBroker;
   friend class CmpiData;
   friend class CmpiObjectPath;
   friend class CmpiInstance;
   friend class CmpiObject;
   friend class CmpiArgs;
   friend class CmpiArrayIdx;
private:
protected:
   void *enc;
   /** Constructor - Internal use only
   */
   inline CmpiString(CMPIString* c) {
      enc=c;
   }

   /** Gets the encapsulated CMPIString.
   */
   inline CMPIString *getEnc() const
   {
      return (CMPIString*)enc;
   }
public:

   /** Constructor - Empty string
   */
   CmpiString()
   {
      enc=NULL;
   }

   /** Constructor - Copy constructor
   */
   CmpiString(const CmpiString& s);

   /** charPtr - get pointer to char* representation
   */
   inline const char* charPtr() const
   {
      if (getEnc())
         return (const char*)getEnc()->hdl;
      else
         return NULL;
   }

   /** charPtr - Case sensitive compare
   */
   inline CmpiBoolean equals(const char *str) const
   {
      return (strcmp(charPtr(),str)==0);
   }
   inline CmpiBoolean equals(const CmpiString& str) const
   {
      return (strcmp(charPtr(),str.charPtr())==0);
   }

   /** charPtr - Case insensitive compare
   */
   inline CmpiBoolean equalsIgnoreCase(const char *str) const
   {
      return (strcasecmp(charPtr(),str)==0);
   }
   inline CmpiBoolean equalsIgnoreCase(const CmpiString& str) const
   {
      return (strcasecmp(charPtr(),str.charPtr())==0);
   }
};

#endif


