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
// Author: Chip Vincent (cvincent@us.ibm.com)
//
// Modified By:	Barbara Packard  (barbara_packard@hp.com)
//				Paulo Sehn (paulo_sehn@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_WMIType_h
#define Pegasus_WMIType_h

//#include <WMIAdapter/Common/Config.h>
#include <Pegasus/Common/CIMType.h>

//#include <wbemcli.h>

PEGASUS_NAMESPACE_BEGIN

PEGASUS_WMIPROVIDER_LINKAGE CIMType WMITypeToCIMType(const CIMTYPE type);

PEGASUS_WMIPROVIDER_LINKAGE CIMType variantToCIMType(const CComVariant & vValue);

PEGASUS_WMIPROVIDER_LINKAGE CIMType vartypeToCIMType(const VARTYPE vt);

PEGASUS_WMIPROVIDER_LINKAGE CIMTYPE_ENUMERATION CIMTypeToWMIType(const CIMTYPE type);

PEGASUS_NAMESPACE_END

#endif
