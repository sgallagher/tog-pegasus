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
//==============================================================================
//

//%/////////////////////////////////////////////////////////////////////////////
//
// This class is the interface between the cimmof compiler, in its various
// forms, and the various Pegasus repository interfaces which as the
// time this class was created were CIMRepository and CIMClient.
//
// This class supports only the operations that the compiler needs, which
// are
//     addClass()
//     addInstance()
//     addQualifier()
//     createNameSpace()
//
// If we create compiler-like tools to do mass changes to the repository,
// then I expect that we will add methods to deal with the modification.
// This class is intended to be very light, basically making it easy
// to choose what repository and what repository interface to use.
// It includes both, since there's nothing to be saved by splitting them.
// Anything that the client or repository interface throws gets passed
// to the cimmofParser level, which is equipped to handle the exceptions
//

#ifndef cimmofConsumer_h
#define cimmofConsumer_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMName.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Compiler/Linkage.h>
#include "mofCompilerOptions.h"
#include <Pegasus/Repository/CIMRepository.h>

PEGASUS_NAMESPACE_BEGIN

// Forward declarations
class cimmofRepositoryConsumer;
class cimmofClientConsumer;
class CIMClass;
class CIMQualifierDecl;
class CIMInstance;

/** This class consumer parsed entities. It is the base class for other 
    consumers.
*/
class PEGASUS_COMPILER_LINKAGE cimmofConsumer
{
public:

    enum ConsumerType 
    {
        REPOSITORY_CONSUMER = 1,
        CLIENT_CONSUMER = 2,
        META_CONSUMER = 3,
    };

    cimmofConsumer();

    virtual ~cimmofConsumer();

    virtual void addClass(
        const CIMNamespaceName& nameSpace,
        CIMClass& Class) = 0;

    virtual void addQualifier(
        const CIMNamespaceName& nameSpace,
        CIMQualifierDecl& qual) = 0;

    virtual void addInstance(
        const CIMNamespaceName& nameSpace,
        CIMInstance& instance) = 0;

    virtual CIMQualifierDecl getQualifierDecl(
        const CIMNamespaceName& nameSpace,
        const CIMName& qualifierName) = 0;

    virtual CIMClass getClass(
        const CIMNamespaceName& nameSpace,
        const CIMName& className) = 0;

    virtual void modifyClass(
        const CIMNamespaceName& nameSpace,
        CIMClass& Class) = 0;

    virtual void createNameSpace(
        const CIMNamespaceName& nameSpace) = 0;

    virtual void start() = 0;

    virtual void finish() = 0;

    static cimmofConsumer* createConsumer(
        ConsumerType type, 
        String location, 
        Uint32 mode,
        compilerCommonDefs::operationType ot, 
        bool discard);

private:

    PEGASUS_FORMAT(3, 4) 
    void _out1(FILE* os, const char* format, ...);

    PEGASUS_FORMAT(3, 4) 
    void _out2(FILE* os, const char* format, ...);

    cimmofConsumer(const cimmofConsumer&);
    cimmofConsumer& operator=(const cimmofConsumer&);
};

PEGASUS_NAMESPACE_END

#endif /* cimmofConsumer_h */
