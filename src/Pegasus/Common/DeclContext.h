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
// $Log: DeclContext.h,v $
// Revision 1.1  2001/01/14 19:51:13  mike
// Initial revision
//
//
//END_HISTORY

////////////////////////////////////////////////////////////////////////////////
//
// DeclContext.h
//
//	This class is used internally as a source of CIM declarations.
//	It is passed to the _resolve() method of several classes (ClassDecl,
//	Method, Property, Reference, Qualifier). Methods are provided for
//	looking up class, qualifier, and instance declarations. This is
//	an abstract base class: implementations must be provided by derived
//	classes.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_DeclContext_h
#define Pegasus_DeclContext_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/ClassDecl.h>
#include <Pegasus/Common/QualifierDecl.h>

PEGASUS_NAMESPACE_BEGIN

class PEGASUS_COMMON_LINKAGE DeclContext
{
public:

    virtual ~DeclContext();

    virtual QualifierDecl lookupQualifierDecl(
	const String& nameSpace,
	const String& name) const = 0;

    virtual ClassDecl lookupClassDecl(
	const String& nameSpace,
	const String& name) const = 0;
};

class PEGASUS_COMMON_LINKAGE SimpleDeclContext : public DeclContext
{
public:

    virtual ~SimpleDeclContext();

    void addQualifierDecl(
	const String& nameSpace,
	const QualifierDecl& x);

    void addClassDecl(
	const String& nameSpace,
	const ClassDecl& x);

    virtual QualifierDecl lookupQualifierDecl(
	const String& nameSpace,
	const String& name) const;

    virtual ClassDecl lookupClassDecl(
	const String& nameSpace,
	const String& name) const;

private:

    typedef Pair<String, ClassDecl> CPair;
    typedef Pair<String, QualifierDecl> QPair;

    Array<CPair> _classDeclarations;
    Array<QPair> _qualifierDeclarations;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_DeclContext_h */
