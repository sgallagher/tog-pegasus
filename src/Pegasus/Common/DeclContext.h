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

#ifndef Pegasus_DeclContext_h
#define Pegasus_DeclContext_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/CIMClass.h>
#include <Pegasus/Common/CIMQualifierDecl.h>

PEGASUS_NAMESPACE_BEGIN

/**
    This class is used internally as a source of CIM declarations.
    It is passed to the _resolve() method of several classes (CIMClass,
    CIMMethod, CIMProperty, CIMReference, CIMQualifier). Methods are provided 
    for looking up class, qualifier, and instance declarations. This is
    an abstract base class: implementations must be provided by derived 
    classes.
*/
class PEGASUS_COMMON_LINKAGE DeclContext
{
public:

    virtual ~DeclContext();

    virtual CIMQualifierDecl lookupQualifierDecl(
	const String& nameSpace,
	const String& name) const = 0;

    virtual CIMClass lookupClass(
	const String& nameSpace,
	const String& name) const = 0;
};

typedef Pair<String, CIMClass> CPair;
typedef Pair<String, CIMQualifierDecl> QPair;

inline int operator==(const QPair& x, const QPair& y)
{
    return 0;
}

inline int operator==(const CPair& x, const CPair& y)
{
    return 0;
}

#define PEGASUS_ARRAY_T QPair
# include "ArrayInter.h"
#undef PEGASUS_ARRAY_T

#define PEGASUS_ARRAY_T CPair
# include "ArrayInter.h"
#undef PEGASUS_ARRAY_T

class PEGASUS_COMMON_LINKAGE SimpleDeclContext : public DeclContext
{
public:

    virtual ~SimpleDeclContext();

    void addQualifierDecl(
	const String& nameSpace,
	const CIMQualifierDecl& x);

    void addClass(
	const String& nameSpace,
	const CIMClass& x);

    virtual CIMQualifierDecl lookupQualifierDecl(
	const String& nameSpace,
	const String& name) const;

    virtual CIMClass lookupClass(
	const String& nameSpace,
	const String& name) const;

private:

    Array<CPair> _classDeclarations;
    Array<QPair> _qualifierDeclarations;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_DeclContext_h */
