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

#ifndef _CMPI_Object_H_
#define _CMPI_Object_H_

#include <Pegasus/Provider/CMPI/cmpidt.h>

#include <Pegasus/Common/String.h>
#include <Pegasus/Common/CIMInstance.h>
#include <Pegasus/Common/CIMProperty.h>
#include <Pegasus/Common/CIMObjectPath.h>
#include <Pegasus/Common/CIMParamValue.h>
#include <Pegasus/Common/CIMDateTime.h>
#include <Pegasus/Common/OperationContext.h>

#include "CMPI_ThreadContext.h"
#include "CMPI_Enumeration.h"

PEGASUS_NAMESPACE_BEGIN

class CMPI_Object {
   friend class CMPI_ThreadContext;
   void *hdl;
   void *ftab;
   CMPI_Object *next,*prev;
 public:
   void *priv;    // CMPI type specific usage
   void *getHdl()  { return hdl; }
   void *getFtab() { return ftab; }
   CMPI_Object(CIMInstance*);
   CMPI_Object(CIMObjectPath*);
   CMPI_Object(CIMDateTime*);
   CMPI_Object(OperationContext*);
   CMPI_Object(const String&);
   CMPI_Object(const char*);
   CMPI_Object(Array<CIMParamValue>*);
   CMPI_Object(CMPIData*);
   CMPI_Object(CMPI_Object*);
   CMPI_Object(CMPISelectCond*);
   CMPI_Object(CMPISubCond*);
   CMPI_Object(CMPIPredicate*);
   CMPI_Object(struct CMPI_InstEnumeration*);
   CMPI_Object(struct CMPI_ObjEnumeration*);
   CMPI_Object(struct CMPI_OpEnumeration*);
   ~CMPI_Object();
   void unlinkAndDelete();
   void unlink();
};

PEGASUS_NAMESPACE_END

#endif
