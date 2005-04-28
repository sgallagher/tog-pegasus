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
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include "CMPI_Version.h"

#include "CMPI_Object.h"
#include "CMPI_ThreadContext.h"
#include "CMPI_Ftabs.h"

#include <string.h>

PEGASUS_USING_STD;
PEGASUS_NAMESPACE_BEGIN

CMPI_Object::CMPI_Object(CMPI_Object *obj) {
   hdl=obj->hdl;
   ftab=obj->ftab;
   priv=obj->priv;
}
// Add a flag here?
CMPI_Object::CMPI_Object(CIMInstance* ci) {
   CMPI_ThreadContext::addObject(this);
   hdl=(void*)ci;
   ftab=CMPI_Instance_Ftab;
   priv=NULL;
}

CMPI_Object::CMPI_Object(CIMObjectPath* cop) {
   CMPI_ThreadContext::addObject(this);
   hdl=(void*)cop;
   ftab=CMPI_ObjectPath_Ftab;
}

CMPI_Object::CMPI_Object(CIMDateTime* cdt) {
   CMPI_ThreadContext::addObject(this);
   hdl=(void*)cdt;
   ftab=CMPI_DateTime_Ftab;
}

CMPI_Object::CMPI_Object(const String& str) {
   CMPI_ThreadContext::addObject(this);
   const CString st=str.getCString();
   hdl=(void*)strdup((const char*)st);
   ftab=CMPI_String_Ftab;
}

CMPI_Object::CMPI_Object(const char *str) {
   CMPI_ThreadContext::addObject(this);
   hdl=(void*)strdup(str);
   ftab=CMPI_String_Ftab;
}

CMPI_Object::CMPI_Object(Array<CIMParamValue> *args) {
   CMPI_ThreadContext::addObject(this);
   hdl=(void*)args;
   ftab=CMPI_Args_Ftab;
}

CMPI_Object::CMPI_Object(CMPIData *dta) {
   CMPI_ThreadContext::addObject(this);
   hdl=(void*)dta;
   ftab=CMPI_Array_Ftab;
}

CMPI_Object::CMPI_Object(CMPISelectCond *dta) {
   CMPI_ThreadContext::addObject(this);
   hdl=(void*)dta;
   ftab=CMPI_SelectCond_Ftab;
}

CMPI_Object::CMPI_Object(CMPISubCond *dta) {
    CMPI_ThreadContext::addObject(this);
    hdl=(void*)dta;
    ftab=CMPI_SubCond_Ftab;
}

CMPI_Object::CMPI_Object(CMPIPredicate *dta) {
   CMPI_ThreadContext::addObject(this);
   hdl=(void*)dta;
   ftab=CMPI_Predicate_Ftab;
}

CMPI_Object::CMPI_Object(CMPI_ObjEnumeration *dta) {
   CMPI_ThreadContext::addObject(this);
   hdl=(void*)dta;
   ftab=CMPI_ObjEnumeration_Ftab;
}

CMPI_Object::CMPI_Object(CMPI_InstEnumeration *dta) {
   CMPI_ThreadContext::addObject(this);
   hdl=(void*)dta;
   ftab=CMPI_InstEnumeration_Ftab;
}

CMPI_Object::CMPI_Object(CMPI_OpEnumeration *dta) {
   CMPI_ThreadContext::addObject(this);
   hdl=(void*)dta;
   ftab=CMPI_OpEnumeration_Ftab;
}

CMPI_Object::~CMPI_Object() {
   if (ftab==CMPI_Instance_Ftab) {
      char **list=(char**)priv;
      if (priv) {
         while (*list) {
            free (*list);
            list++;
         }
         free(priv);
      }
      priv=NULL;
   }
}

void CMPI_Object::unlinkAndDelete() {
   CMPI_ThreadContext::remObject(this);
   delete this;
}

void CMPI_Object::unlink() {
   CMPI_ThreadContext::remObject(this);
}

PEGASUS_NAMESPACE_END




