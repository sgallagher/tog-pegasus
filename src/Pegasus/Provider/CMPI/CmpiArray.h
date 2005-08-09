//%2005////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
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

#ifndef _CmpiArray_h_
#define _CmpiArray_h_

#include "cmpidt.h"
#include "CmpiString.h"

#include "CmpiObjectPath.h"
#include "CmpiInstance.h"
#include "CmpiBaseMI.h"
#include "CmpiData.h"
#include "Linkage.h"

class CmpiArray;
class CmpiInstance;
class CmpiData;

class PEGASUS_CMPI_PROVIDER_LINKAGE CmpiArrayIdx {
   friend class CmpiArray;
   const CmpiArray &ar;
   CMPICount idx;
   CmpiArrayIdx(const CmpiArray &a, CMPICount i);
public:
   CmpiArrayIdx& operator=(const CmpiData&);
   CmpiData getData() const;

   /** Extracting String.
   */
   operator CmpiString() const;
   /** Extracting const char *
   */
   operator const char* () const;
   /** Extracting CmpiDataTime.
   */
   operator CmpiDateTime() const;
   /** Extracting CmpiInstance.
   */
   operator CmpiInstance() const;
   /** Extracting CmpiObjectPath.
   */
   operator CmpiObjectPath() const;
   /** Extracting CMPISint8.
   */
   operator CMPISint8() const;
   /** Extracting signed 16 bit.
   */
   operator CMPISint16() const;
   /** Extracting signed 32 bit.
   */
   operator CMPISint32() const;
   /** Extracting signed 64 bit.
   */
   operator CMPISint64() const;

   /** Extracting unsigned 8 bit or boolean.
   */
   operator unsigned char() const;
   /** Extracting unsigned 16 bit or char16.
   */
   operator unsigned short() const;
   /** Extracting unsigned 32 bit.
   */
   operator CMPIUint32() const;
   /** Extracting unsigned 64 bit.
   */
   operator CMPIUint64() const;
   /** Extracting float 32 bit.
   */
   operator CMPIReal32() const;
   /** Extracting float 64 bit.
   */
   operator CMPIReal64() const;

};


/** This class wraps a CMPIData value array.
      Index operations use the [] operator.
      Data extraction uses type conversion operators.
      Extraction operations can be appended to an array indexing operation
      like this:

        CmpiString state;

 CmpiArray states;

        states=ci.getProperty("States");

 state=states[3];

 Assignment statements use array indexing operations as well:

 states[5]="offline";

       Type mismatches will be signalled by exceptions.
*/

class CmpiArray : public CmpiObject {
   friend class CmpiArrayIdx;
   friend class CmpiData;
   friend class CmpiEnumeration;
   void operator=(int x);

   /** Gets the encapsulated CMPIArray.
   */
   CMPIArray *getEnc() const;
   void *makeArray(CMPIBroker *mb,CMPICount max, CMPIType type);
   CmpiArray(CMPIArray *arr);
public:
   CmpiArray(CMPICount max, CMPIType type);
   CmpiArray();
   CMPICount size() const;
   CmpiArrayIdx operator[](int idx) const;
};

#endif



