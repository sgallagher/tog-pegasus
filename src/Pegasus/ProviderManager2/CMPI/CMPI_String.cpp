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
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////


#define CMPI_VER_86 1

#include "CMPI_String.h"
#include "CMPI_Ftabs.h"

#include <string.h>

PEGASUS_USING_STD;
PEGASUS_NAMESPACE_BEGIN

CMPI_String* string2CMPIString(const String &s) {
  const CString st=s.getCString();
  char *cp=strdup((const char*)st);
  CMPI_Object *obj= new CMPI_Object((void*)cp,CMPI_String_Ftab);
//  CMPIRefs::localRefs().addRef(obj,CMPIRefs::TypeString);
  return (CMPI_String*)obj;
}

static CMPIStatus stringRelease(CMPIString *eStr) {
//   cout<<"--- stringRelease()"<<endl;
   CMReturn(CMPI_RC_OK);
}

static CMPIString* stringClone(CMPIString *eStr, CMPIStatus* rc) {
   char* str=(char*)eStr->hdl;
   char* newstr=::strdup(str);
   CMPI_Object * obj=new CMPI_Object(newstr,CMPI_String_Ftab);
//   CMPIRefs::localRefs().addRef(obj,CMPIRefs::TypeString);
   if (rc) CMSetStatus(rc,CMPI_RC_OK);
   return (CMPIString*)obj;
}

static char * stringGetCharPtr(CMPIString *eStr, CMPIStatus* rc) {
   char* ptr=(char*)eStr->hdl;
   if (rc) CMSetStatus(rc,CMPI_RC_OK);
   return ptr;
}

static CMPIStringFT string_FT={
     CMPICurrentVersion,
     stringRelease,
     stringClone,
     stringGetCharPtr,
};

CMPIStringFT *CMPI_String_Ftab=&string_FT;


PEGASUS_NAMESPACE_END

