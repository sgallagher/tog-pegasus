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

#ifndef _cimmofMRR_h
#define _cimmofMRR_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMClass.h>
#include <Pegasus/Common/CIMQualifierDecl.h>
#include <cstdio>
#include "Linkage.h"

PEGASUS_NAMESPACE_BEGIN

#define PEGASUS_ARRAY_T CIMConstQualifier
# include <Pegasus/Common/ArrayInter.h>
#undef PEGASUS_ARRAY_T

/** The MRR (Memory Resident Repository) .
*/
class PEGASUS_COMPILER_LINKAGE cimmofMRR
{
public:

    cimmofMRR(bool descriptions);

    ~cimmofMRR();

    void addClass(
        const CIMNamespaceName& nameSpace,
        CIMClass& Class);

    void addQualifier(
        const CIMNamespaceName& nameSpace,
        CIMQualifierDecl& qual);

    void addInstance(
        const CIMNamespaceName& nameSpace,
        CIMInstance& instance);

    CIMQualifierDecl getQualifierDecl(
        const CIMNamespaceName& nameSpace,
        const CIMName& qualifierName);

    CIMClass getClass(
        const CIMNamespaceName& nameSpace,
        const CIMName& className);

    void modifyClass(
        const CIMNamespaceName& nameSpace,
        CIMClass& Class);

    void createNameSpace(
        const CIMNamespaceName& nameSpace);

    void start();

    void finish();

private:

    PEGASUS_FORMAT(2, 3)
    void _out(const char* format, ...);

    PEGASUS_FORMAT(2, 3)
    void _outn(const char* format, ...);

    void _nl();

    Uint32 _findClass(const CIMName& className) const;

    Uint32 _findQualifier(const CIMName& qualifierName) const;

    void _writeMetaPrologue();

    void _writeMetaEpilogue();

    void _writeQualifier(
        const Array<CIMQualifierDecl>& qualifierDecls,
        const CIMConstQualifier& qualifier);

    void _writeQualifierDecl(const CIMConstQualifierDecl& cq);

    void _writeNameSpace(const CIMNamespaceName& nameSpace);

    void _writeQualifierArray(
        const String& root,
        const Array<CIMConstQualifier>& qualifiers);

    void _writeProperty(
        const CIMNamespaceName& nameSpace,
        const CIMName& className,
        const CIMConstProperty& cp);

    void _writeParameter(
        const CIMNamespaceName& nameSpace,
        const CIMName& cn,
        const CIMName& mn,
        const CIMConstParameter& cp);

    void _writeMethod(
        const CIMNamespaceName& nameSpace,
        const CIMName& cn,
        const CIMConstMethod& cm);

    void _writeClass(
        const CIMNamespaceName& nameSpace,
        const CIMClass& cimClass);

    void _loadClassFile(
        Array<CIMName>& classes, const String& path);

    bool _includeClass(const CIMName& cn);

    bool _discard;
    FILE* _os;
    CIMNamespaceName _nameSpace;
    Array<CIMClass> _classes;
    Array<CIMQualifierDecl> _qualifiers;
    Array<CIMInstance> _instances;
    Array<CIMName> _closure;
};

PEGASUS_NAMESPACE_END

#endif /* _cimmofMRR_h */
