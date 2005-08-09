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

#ifndef _CmpiData_h_
#define _CmpiData_h_

#include "cmpidt.h"
#include "cmpift.h"
#include "CmpiString.h"
#include "CmpiDateTime.h"

class CmpiInstance;
class CmpiObjectPath;

#include "CmpiObjectPath.h"
#include "CmpiInstance.h"
#include "CmpiArray.h"
#include "Linkage.h"

class CmpiBooleanData;
class CmpiCharData;


/** This class wraps a CMPIData value item. Data extraction uses type conversion operators.
      Extraction operations can be appended to a property type retrieval statement
      like this:
 
        CmpiString name = cop.getKey("DeviceID");
 
       Type mismatches will be signalled by exceptions.
*/
class PEGASUS_CMPI_PROVIDER_LINKAGE CmpiData {
   friend class CmpiInstance;
   friend class CmpiObjectPath;
   friend class CmpiResult;
   friend class CmpiArgs;
   friend class CmpiArrayIdx;
   friend class CmpiContext;
   friend class CmpiPropertyMI;
   friend class CmpiEnumeration;
protected:

   /** CmpiData actually is a CMPIData struct.
   */
   CMPIData _data;

   /** Constructor - Empty constructor.
   */
   CmpiData(CMPIData& data);

public:

   /** Constructor - Empty constructor.
   */
   CmpiData();

   /** Constructor - signed 8 bit as input.
   */
   CmpiData(CMPISint8 d);

   /** Constructor - signed 16 bit as input.
   */
   CmpiData(CMPISint16 d);

   /** Constructor - signed 32 bit as input.
   */
   CmpiData(CMPISint32 d);

   /** Constructor - signed 64 bit as input.
   */
   CmpiData(CMPISint64 d);

   /** Constructor - unsigned 8 bit as input.
   */
   CmpiData(CMPIUint8 d);

   /** Constructor - unsigned 16 bit as input.
   */
   CmpiData(CMPIUint16 d);

   /** Constructor - unsigned 32 bit as input.
   */
   CmpiData(CMPIUint32 d);

   /** Constructor - unsigned 64 bit as input.
   */
   CmpiData(CMPIUint64 d);

   /** Constructor - 32 bit float as input.
   */
   CmpiData(CMPIReal32 d);

   /** Constructor - 64 bit float as input.
   */
   CmpiData(CMPIReal64 d);

   /** Constructor - String as input.
   */
   CmpiData(const CmpiString& d);

   /** Constructor - char* as input.
   */
   CmpiData(const char* d);

   /** Constructor - ObjectPath as input.
   */
   CmpiData(const CmpiObjectPath& d);


   /** Constructor - DateTime as input.
   */
   CmpiData(const CmpiDateTime& d);

   /** Constructor - Array as input.
   */
   CmpiData(const CmpiArray& d);

   /** Extracting String.
   */
    operator CmpiString() const;

   /** Extracting const char *.
   */
    operator const char* () const;

   /** Extracting CmpiDataTime.
   */
   operator CmpiDateTime() const;

   /** Extracting signed 8 bit.
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

   /** Extracting Array.
   */
   operator CmpiArray() const;

   /** Extracting Instance.
   */
   operator CmpiInstance() const;

   /** Extracting ObjectPath.
   */
   operator CmpiObjectPath() const;

   /** test for null value
   */
   int isNullValue() const;

   /** test for not found value
   */
   int isNotFound() const;
};

#endif



