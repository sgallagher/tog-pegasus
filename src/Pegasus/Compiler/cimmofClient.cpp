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
// Author: Bob Blair (bblair@bmc.com)
//
// Modified By: Carol Ann Krug Graves, Hewlett-Packard Company
//                (carolann_graves@hp.com)
//              Gerarda Marquez (gmarquez@us.ibm.com)
//              -- PEP 43 changes
//
//%/////////////////////////////////////////////////////////////////////////////

#include "cimmofClient.h"
#include <Pegasus/Common/CIMClass.h>
#include <Pegasus/Common/CIMQualifierDecl.h>
#include <Pegasus/Common/CIMInstance.h>

PEGASUS_USING_PEGASUS;

static const char __NAMESPACE_NAMESPACE [] = "root";
static const char NAMESPACE_CLASS [] = "__NameSpace";
static const char NAMESPACE_PROPERTY_NAME [] = "Name";

cimmofClient::cimmofClient() :
  _ot(compilerCommonDefs::USE_REPOSITORY),
  _client(0)
{
}

cimmofClient::~cimmofClient() {
}

void
cimmofClient::init(String &location, compilerCommonDefs::operationType ot)
{
  _ot = ot;
  // ATTN:  We will want to make failure to connect a more satisfying
  // experience by catching the error here and analyzing it.  For now,
  // though, it should be OK to just jump all the way out and let
  // our caller handle it.

  _client = new CIMClient();
  _client->setRequestDefaultLanguages();  //l10n

#if defined(PEGASUS_OS_HPUX) || defined(PEGASUS_PLATFORM_LINUX_GENERIC_GNU)
  _client->connectLocal();
#else
  String hostname = "localhost";  //defaults
  Uint32 portno = 5988;
  // ATTN:
  //   get host name and port from command line data via parser object
  _client->connect(hostname, portno, String::EMPTY, String::EMPTY);
#endif
}

void
cimmofClient::addClass(const CIMNamespaceName &nameSpace, CIMClass &Class) const
{
    _client->createClass(nameSpace, Class);
}

void
cimmofClient::addQualifier(const CIMNamespaceName &nameSpace,
					CIMQualifierDecl &qualifier) const
{
    _client->setQualifier(nameSpace, qualifier);
}

void
cimmofClient::addInstance(const CIMNamespaceName &nameSpace,
				       CIMInstance &instance) const
{
    _client->createInstance(nameSpace, instance);
}

CIMQualifierDecl
cimmofClient::getQualifierDecl(const CIMNamespaceName &nameSpace,
					    const CIMName &qualifierName) const
{
    return (_client->getQualifier(nameSpace, qualifierName));
}

CIMClass
cimmofClient::getClass(const CIMNamespaceName &nameSpace,
				    const CIMName &className) const
{
    return (_client->getClass(nameSpace, className, false, true, true));
}

void
cimmofClient::modifyClass(const CIMNamespaceName &nameSpace, CIMClass &Class) const
{
    _client->modifyClass(nameSpace, Class);
}

void
cimmofClient::createNameSpace(const CIMNamespaceName &nameSpace) const
{
    // The new namespace name will be computed by
    // concatenating the target Namespace name with the
    // value of the Name property. By setting the
    // value of the Name property to String::EMPTY,
    // the name of the created namespace will be the 
    // target namespace.
    CIMInstance newInstance = CIMName (NAMESPACE_CLASS);
    newInstance.addProperty(
              CIMProperty(CIMName (NAMESPACE_PROPERTY_NAME), String::EMPTY));
    _client->createInstance(nameSpace, newInstance);
}
