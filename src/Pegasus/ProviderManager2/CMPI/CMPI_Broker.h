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

#ifndef _CMPI_Broker_H_
#define _CMPI_Broker_H_

#include <Pegasus/Provider/CMPI/cmpidt.h>
#include <Pegasus/Provider/CMPI/cmpift.h>

#include <Pegasus/Common/String.h>
#include <Pegasus/Common/CIMClass.h>
#include <Pegasus/Common/HashTable.h>
#include <Pegasus/Common/IPC.h>

PEGASUS_NAMESPACE_BEGIN

#define CM_BROKER (CMPI_ThreadContext::getBroker())
#define CM_CIMOM(mb) ((CIMOMHandle*)mb->hdl)

#define CM_Context(ctx) (((CMPI_Context*)ctx)->ctx)
#define CM_Instance(ci) ((CIMInstance*)ci->hdl)
#define CM_ObjectPath(cop) ((CIMObjectPath*)cop->hdl)

#define CM_LocalOnly(flgs) (((flgs) & CMPI_FLAG_LocalOnly)!=0)
#define CM_ClassOrigin(flgs) (((flgs) & CMPI_FLAG_IncludeClassOrigin)!=0)
#define CM_IncludeQualifiers(flgs) (((flgs) & CMPI_FLAG_IncludeQualifiers)!=0)
#define CM_DeepInheritance(flgs) (((flgs) & CMPI_FLAG_DeepInheritance)!=0)

CIMClass *mbGetClass(const CMPIBroker *mb, const CIMObjectPath &cop);

typedef HashTable<String, CIMClass *,
      EqualFunc<String>,  HashFunc<String> > ClassCache;

class CMPIProvider;

struct CMPI_Broker : CMPIBroker {
   ClassCache *clsCache;
   Mutex mtx;
   String name;
   CMPIProvider *provider;
};

PEGASUS_NAMESPACE_END

#endif
