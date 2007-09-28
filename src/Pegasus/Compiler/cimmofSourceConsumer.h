//%2006////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
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
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef _cimmofSourceConsumer_h
#define _cimmofSourceConsumer_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMClass.h>
#include <cstdio>
#include "cimmofConsumer.h"

PEGASUS_NAMESPACE_BEGIN

class PEGASUS_COMPILER_LINKAGE cimmofSourceConsumer : public cimmofConsumer
{
public:

    cimmofSourceConsumer();

    virtual ~cimmofSourceConsumer();

    virtual void addClass(
        const CIMNamespaceName& nameSpace,
        CIMClass& Class);

    virtual void addQualifier(
        const CIMNamespaceName& nameSpace,
        CIMQualifierDecl& qual);

    virtual void addInstance(
        const CIMNamespaceName& nameSpace,
        CIMInstance& instance);

    virtual CIMQualifierDecl getQualifierDecl(
        const CIMNamespaceName& nameSpace,
        const CIMName& qualifierName);

    virtual CIMClass getClass(
        const CIMNamespaceName& nameSpace,
        const CIMName& className);

    virtual void modifyClass(
        const CIMNamespaceName& nameSpace,
        CIMClass& Class);

    virtual void createNameSpace(
        const CIMNamespaceName& nameSpace);

    virtual void start();

    virtual void finish();

private:

    PEGASUS_FORMAT(2, 3)
    void _out(const char* format, ...);

    PEGASUS_FORMAT(2, 3)
    void _outn(const char* format, ...);

    void _nl();

    Uint32 _findNameSpace(const CIMNamespaceName& nameSpace) const;

    Uint32 _findClass(
        const CIMNamespaceName& nameSpace, 
        const CIMName& className) const;

    Uint32 _findQualifier(
        const CIMNamespaceName& nameSpace, 
        const CIMName& qualifierName) const;

    void _writeSourcePrologue();

    void _writeSourceEpilogue();

    void _writeNameSpace(const CIMNamespaceName& nameSpace);

    void _writeProperty(
        const CIMNamespaceName& nameSpace,
        const CIMName& className,
        const CIMConstProperty& cp);

    void _writeClass(
        const CIMNamespaceName& nameSpace,
        const CIMClass& cimClass);

    typedef Pair<CIMNamespaceName, CIMClass> Class;
    typedef Pair<CIMNamespaceName, CIMQualifierDecl> Qualifier;

    Array<CIMNamespaceName> _nameSpaces;
    Array<Class> _classes;
    Array<Qualifier> _qualifiers;
    FILE* _os;
};

PEGASUS_NAMESPACE_END

#endif /* _cimmofSourceConsumer_h */
