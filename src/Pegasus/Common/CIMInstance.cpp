//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 The Open group, BMC Software, Tivoli Systems, IBM
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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include "CIMInstance.h"
#include "DeclContext.h"
#include "Indentor.h"
#include "CIMName.h"
#include "XmlWriter.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

#define PEGASUS_ARRAY_T CIMInstance
# include "ArrayImpl.h"
#undef PEGASUS_ARRAY_T

////////////////////////////////////////////////////////////////////////////////
//
// CIMInstance
//
////////////////////////////////////////////////////////////////////////////////

CIMInstance::CIMInstance(const CIMObject& x)
{
    if (!(_rep = dynamic_cast<CIMInstanceRep*>(x._rep)))
	throw DynamicCastFailed();
}

CIMInstance::CIMInstance(const CIMObject& x, NoThrow&)
{
    _rep = dynamic_cast<CIMInstanceRep*>(x._rep);
}

Boolean CIMInstance::identical(const CIMConstInstance& x) const
{
    x._checkRep();
    _checkRep();
    return _rep->identical(x._rep);
}

void CIMInstance::resolve(
    DeclContext* declContext, 
    const String& nameSpace,
    Boolean propagateQualifiers)
{
    _checkRep();
    CIMConstClass cimClass;
    _rep->resolve(declContext, nameSpace, cimClass, propagateQualifiers);
}

////////////////////////////////////////////////////////////////////////////////
//
// CIMConstInstance
//
////////////////////////////////////////////////////////////////////////////////

CIMConstInstance::CIMConstInstance(const CIMObject& x)
{
    if (!(_rep = dynamic_cast<CIMInstanceRep*>(x._rep)))
	throw DynamicCastFailed();
}

CIMConstInstance::CIMConstInstance(const CIMConstObject& x)
{
    if (!(_rep = dynamic_cast<CIMInstanceRep*>(x._rep)))
	throw DynamicCastFailed();
}

CIMConstInstance::CIMConstInstance(const CIMObject& x, NoThrow&)
{
    _rep = dynamic_cast<CIMInstanceRep*>(x._rep);
}

CIMConstInstance::CIMConstInstance(const CIMConstObject& x, NoThrow&)
{
    _rep = dynamic_cast<CIMInstanceRep*>(x._rep);
}

PEGASUS_NAMESPACE_END
