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

#include "DeclContext.h"
#include "CIMName.h"

PEGASUS_NAMESPACE_BEGIN

#define PEGASUS_ARRAY_T QPair
# include "ArrayImpl.h"
#undef PEGASUS_ARRAY_T

#define PEGASUS_ARRAY_T CPair
# include "ArrayImpl.h"
#undef PEGASUS_ARRAY_T

DeclContext::~DeclContext()
{

}

SimpleDeclContext::~SimpleDeclContext()
{

}

void SimpleDeclContext::addQualifierDecl(
    const String& nameSpace,
    const CIMQualifierDecl& x)
{
    if (!lookupQualifierDecl(nameSpace, x.getName()).isNull())
	throw AlreadyExists();

    _qualifierDeclarations.append(QPair(nameSpace, x));
}

void SimpleDeclContext::addClass(
    const String& nameSpace,
    const CIMClass& x)
{
    if (!lookupClass(nameSpace, x.getClassName()).isNull())
	throw AlreadyExists();

    _classDeclarations.append(CPair(nameSpace, x));
}

CIMQualifierDecl SimpleDeclContext::lookupQualifierDecl(
    const String& nameSpace,
    const String& name) const
{
    for (Uint32 i = 0, n = _qualifierDeclarations.size(); i < n; i++)
    {
	const String& first = _qualifierDeclarations[i].first;
	const CIMQualifierDecl& second = _qualifierDeclarations[i].second;

	if (CIMName::equal(first, nameSpace) && 
	    CIMName::equal(second.getName(), name))
	{
	    return second;
	}
    }

    // Not found:
    return CIMQualifierDecl();
}

CIMClass SimpleDeclContext::lookupClass(
    const String& nameSpace,
    const String& name) const
{
    for (Uint32 i = 0, n = _classDeclarations.size(); i < n; i++)
    {
	const String& first = _classDeclarations[i].first;
	const CIMClass& second = _classDeclarations[i].second;

	if (CIMName::equal(first, nameSpace) && 
	    CIMName::equal(second.getClassName(), name))
	{
	    return second;
	}
    }

    // Not found:
    return CIMClass();
}

PEGASUS_NAMESPACE_END
