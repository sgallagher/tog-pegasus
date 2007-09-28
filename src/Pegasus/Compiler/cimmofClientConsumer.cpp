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

#include "cimmofClientConsumer.h"
#include <Pegasus/Common/CIMClass.h>
#include <Pegasus/Common/CIMQualifierDecl.h>
#include <Pegasus/Common/CIMInstance.h>

PEGASUS_USING_PEGASUS;

static const char __NAMESPACE_NAMESPACE [] = "root";
static const char NAMESPACE_CLASS [] = "__NameSpace";
static const char NAMESPACE_PROPERTY_NAME [] = "Name";

cimmofClientConsumer::cimmofClientConsumer() :
    _ot(compilerCommonDefs::USE_REPOSITORY),
    _client(0)
{
}

cimmofClientConsumer::~cimmofClientConsumer()
{
}

void cimmofClientConsumer::init(
    String &location, compilerCommonDefs::operationType ot)
{
    _ot = ot;
    // ATTN:  We will want to make failure to connect a more satisfying
    // experience by catching the error here and analyzing it.  For now,
    // though, it should be OK to just jump all the way out and let
    // our caller handle it.

    _client = new CIMClient();
    _client->setTimeout(CIMMOFCLIENT_TIMEOUT_DEFAULT);
    _client->setRequestDefaultLanguages();  //l10n
    _client->connectLocal();
}

void cimmofClientConsumer::addClass(const CIMNamespaceName &nameSpace,
                            CIMClass &Class)
{
    _client->createClass(nameSpace, Class);
}

void cimmofClientConsumer::addQualifier(const CIMNamespaceName &nameSpace,
                                CIMQualifierDecl &qualifier)
{
    _client->setQualifier(nameSpace, qualifier);
}

void cimmofClientConsumer::addInstance(const CIMNamespaceName &nameSpace,
                               CIMInstance &instance)
{
    _client->createInstance(nameSpace, instance);
}

CIMQualifierDecl cimmofClientConsumer::getQualifierDecl(
        const CIMNamespaceName &nameSpace,
        const CIMName &qualifierName)
{
    return (_client->getQualifier(nameSpace, qualifierName));
}

CIMClass cimmofClientConsumer::getClass(const CIMNamespaceName &nameSpace,
                                const CIMName &className)
{
    return (_client->getClass(nameSpace, className, false, true, true));
}

void cimmofClientConsumer::modifyClass(const CIMNamespaceName &nameSpace,
                               CIMClass &Class)
{
    _client->modifyClass(nameSpace, Class);
}

void cimmofClientConsumer::createNameSpace(const CIMNamespaceName &nameSpace)
{
    // The new namespace name will be computed by
    // concatenating the target Namespace name with the
    // value of the Name property. By setting the
    // value of the Name property to String::EMPTY,
    // the name of the created namespace will be the
    // target namespace.
    CIMInstance newInstance = CIMName (NAMESPACE_CLASS);
    newInstance.addProperty(CIMProperty(CIMName (NAMESPACE_PROPERTY_NAME),
                                        String::EMPTY));
    _client->createInstance(nameSpace, newInstance);
}

void cimmofClientConsumer::start()
{
}

void cimmofClientConsumer::finish()
{
}
