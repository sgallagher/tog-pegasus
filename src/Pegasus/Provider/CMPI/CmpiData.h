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

class CmpiBooleanData;
class CmpiCharData;


/** This class wraps a CMPIData value item. Data extraction uses type conversion operators.
      Extraction operations can be appended to a property type retrieval statement
      like this:

        CmpiString name = cop.getKey("DeviceID");

       Type mismatches will be signalled by exceptions.
*/
class CmpiData {
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
   CMPIData data;

   /** Constructor - Empty constructor.
   */
   CmpiData(CMPIData& data)
      { this->data=data; }

  public:

   /** Constructor - Empty constructor.
   */
   CmpiData() {}

   /** Constructor - singed 8 bit as input.
   */
   inline CmpiData(CMPISint8 d)
      { data.value.sint8=d;  data.type=CMPI_sint8; }
   /** Constructor - singed 16 bit as input.
   */
   inline CmpiData(CMPISint16 d)
      { data.value.sint16=d; data.type=CMPI_sint16;}
   /** Constructor - singed 32 bit as input.
   */
   inline CmpiData(CMPISint32 d)
      { data.value.sint32=d; data.type=CMPI_sint32;}
   /** Constructor - singed 32 bit as input.
   */
   inline CmpiData(int d)
      { data.value.sint32=d; data.type=CMPI_sint32;}
   /** Constructor - singed 64 bit as input.
   */
   inline CmpiData(CMPISint64 d)
      { data.value.sint64=d; data.type=CMPI_sint64;}

   /** Constructor - unsinged 8 bit as input.
   */
   inline CmpiData(CMPIUint8 d)
      { data.value.sint8=d;  data.type=CMPI_uint8; }
   /** Constructor - unsinged 16 bit as input.
   */
   inline CmpiData(CMPIUint16 d)
      { data.value.sint16=d; data.type=CMPI_uint16;}
   /** Constructor - unsinged 32 bit as input.
   */
   inline CmpiData(CMPIUint32 d)
      { data.value.sint32=d; data.type=CMPI_uint32;}
   /** Constructor - unsinged 32 bit as input.
   */
   inline CmpiData(unsigned int d)
      { data.value.sint32=d; data.type=CMPI_uint32;}
   /** Constructor - unsinged 64 bit as input.
   */
   inline CmpiData(CMPIUint64 d)
      { data.value.sint64=d; data.type=CMPI_uint64;}    

   /** Constructor - 32 bit float as input.
   */
   inline CmpiData(CMPIReal32 d)
      { data.value.real32=d; data.type=CMPI_real32;}    

   /** Constructor - 64 bit float as input.
   */
   inline CmpiData(CMPIReal64 d)
      { data.value.real64=d; data.type=CMPI_real64;}    


   /** Constructor - String as input.
   */
   inline CmpiData(const CmpiString& d)
      { data.value.chars=(char*)d.charPtr(); data.type=CMPI_chars;}

   /** Constructor - char* as input.
   */
   inline CmpiData(const char* d)
      { if (d) 
	data.value.chars=(char*)d;  
      else 
	data.value.chars="";  
      data.type=CMPI_chars; }

   /** Constructor - ObjectPath as input.
   */
    CmpiData(const CmpiObjectPath& d);
    

   /** Constructor - DateTime as input.
   */
   inline CmpiData(const CmpiDateTime& d)
      { data.value.dateTime=d.getEnc(); data.type=CMPI_dateTime;}


   /** Constructor - Array as input.
   */
   inline CmpiData(const CmpiArray& d)
      { data.value.array=d.getEnc(); data.type=((CMPIArrayFT*)d.getEnc()->ft)->getSimpleType(d.getEnc(),0) | CMPI_ARRAY;}

   /** Extracting String.
   */
   inline  operator CmpiString() const
      { if (data.type!=CMPI_string) 
	throw CmpiStatus(CMPI_RC_ERR_TYPE_MISMATCH);
        else return CmpiString(data.value.string);
      }

   /** Extracting const char *.
   */
   inline  operator const char* () const
      { if (data.type!=CMPI_string) 
	throw CmpiStatus(CMPI_RC_ERR_TYPE_MISMATCH);
        else return CmpiString(data.value.string).charPtr();
      }

   /** Extracting CmpiDataTime.
   */
   inline operator CmpiDateTime() const
      { if (data.type!=CMPI_dateTime) 
	throw CmpiStatus(CMPI_RC_ERR_TYPE_MISMATCH);
      else return CmpiDateTime(data.value.dateTime);
      }
     
   /** Extracting signed 8 bit.
   */
   inline operator CMPISint8() const
      { if (data.type!=CMPI_sint8) 
	throw CmpiStatus(CMPI_RC_ERR_TYPE_MISMATCH);
        else return data.value.sint8;
	}
   /** Extracting signed 16 bit.
   */
   inline operator CMPISint16() const
      { if (data.type!=CMPI_sint16) 
	throw CmpiStatus(CMPI_RC_ERR_TYPE_MISMATCH);
        else return data.value.sint16;
      }
   /** Extracting signed 32 bit.
   */
   inline operator CMPISint32() const
      { if (data.type!=CMPI_sint32)
	throw CmpiStatus(CMPI_RC_ERR_TYPE_MISMATCH);
        else return data.value.sint32;
      }
   /** Extracting signed 32 bit.
   */
   inline operator int() const
      { if (data.type!=CMPI_sint32) 
	throw CmpiStatus(CMPI_RC_ERR_TYPE_MISMATCH);
        else return data.value.sint32;
      }
   /** Extracting signed 64 bit.
   */
   inline operator CMPISint64() const
      { if (data.type!=CMPI_sint64) 
	throw CmpiStatus(CMPI_RC_ERR_TYPE_MISMATCH);
        else return data.value.sint64;
      }

   /** Extracting unsigned 8 bit or boolean.
   */
   inline operator unsigned char() const
      { 
	if (data.type!=CMPI_uint8 && data.type!=CMPI_boolean) 
	  throw CmpiStatus(CMPI_RC_ERR_TYPE_MISMATCH);
        if (data.type==CMPI_uint8) 
	  return data.value.uint8;
	else
	  return data.value.boolean;
      }
   /** Extracting unsigned 16 bit or char16.
   */
   inline operator unsigned short() const
      { 
	if (data.type!=CMPI_uint16 && data.type!=CMPI_char16) 
	  throw CmpiStatus(CMPI_RC_ERR_TYPE_MISMATCH);
	if (data.type==CMPI_uint16)
	  return data.value.uint16;
        else 
	  return data.value.char16;
      }
   /** Extracting unsigned 32 bit.
   */
   inline operator CMPIUint32() const
      { if (data.type!=CMPI_uint32) 
	throw CmpiStatus(CMPI_RC_ERR_TYPE_MISMATCH);
        else return data.value.uint32;
      }
   /** Extracting unsigned 32 bit.
   */
   inline operator unsigned int() const
      { if (data.type!=CMPI_uint32) 
	throw CmpiStatus(CMPI_RC_ERR_TYPE_MISMATCH);
        else return data.value.uint32;
      }
   /** Extracting unsigned 64 bit.
   */
   inline operator CMPIUint64() const
      { if (data.type!=CMPI_uint64) 
	throw CmpiStatus(CMPI_RC_ERR_TYPE_MISMATCH);
        else return data.value.uint64;
      }

   /** Extracting float 32 bit.
   */
   inline operator CMPIReal32() const
      { if (data.type!=CMPI_real32) 
	throw CmpiStatus(CMPI_RC_ERR_TYPE_MISMATCH);
        else return data.value.real32;
      }

   /** Extracting float 64 bit.
   */
   inline operator CMPIReal64() const
      { if (data.type!=CMPI_real64) 
	throw CmpiStatus(CMPI_RC_ERR_TYPE_MISMATCH);
        else return data.value.real64;
      }

   /** Extracting Array.
   */
   operator CmpiArray() const;

   /** Extracting Instance.
   */
   operator CmpiInstance() const;

   /** Extracting ObjectPath.
   */
   operator CmpiObjectPath() const;
};

#endif



