//BEGIN_LICENSE
//
// Copyright (c) 2000 The Open Group, BMC Software, Tivoli Systems, IBM
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
//END_LICENSE
//BEGIN_HISTORY
//
// Author:
//
// $Log: DeclContext.cpp,v $
// Revision 1.2  2001/02/16 02:06:06  mike
// Renamed many classes and headers.
//
// Revision 1.1.1.1  2001/01/14 19:51:13  mike
// Pegasus import
//
//
//END_HISTORY

#include "DeclContext.h"
#include "CIMName.h"

PEGASUS_NAMESPACE_BEGIN

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
    if (lookupQualifierDecl(nameSpace, x.getName()))
	throw AlreadyExists();

    _qualifierDeclarations.append(QPair(nameSpace, x));
}

void SimpleDeclContext::addClassDecl(
    const String& nameSpace,
    const CIMClass& x)
{
    if (lookupClassDecl(nameSpace, x.getClassName()))
	throw AlreadyExists();

    _classDeclarations.append(CPair(nameSpace, x));
}

CIMQualifierDecl SimpleDeclContext::lookupQualifierDecl(
    const String& nameSpace,
    const String& name) const
{
    for (Uint32 i = 0, n = _qualifierDeclarations.getSize(); i < n; i++)
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

CIMClass SimpleDeclContext::lookupClassDecl(
    const String& nameSpace,
    const String& name) const
{
    for (Uint32 i = 0, n = _classDeclarations.getSize(); i < n; i++)
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
