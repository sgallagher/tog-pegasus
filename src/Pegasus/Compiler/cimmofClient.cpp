//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2002 The Open group, BMC Software, Tivoli Systems, IBM
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
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include "cimmofClient.h"
#include <Pegasus/Common/Monitor.h>
#include <Pegasus/Common/HTTPConnector.h>
#include <Pegasus/Common/CIMClass.h>
#include <Pegasus/Common/CIMQualifierDecl.h>
#include <Pegasus/Common/CIMInstance.h>

PEGASUS_USING_PEGASUS;

cimmofClient::cimmofClient() :
  _client(0),
  _ot(compilerCommonDefs::USE_REPOSITORY)
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

  String hostname = "localhost";  //defaults
  String portno = "5988";
  // ATTN:
  //   get host name and port from command line data via parser object
  String connectString;
  if (location != "")
    connectString = hostname + ":" + portno;
  else
    connectString = location;
  _client->connect(connectString);

}

void
cimmofClient::addClass(const String &nameSpace, CIMClass &Class)
  const {
    _client->createClass(nameSpace, Class);
}

void
cimmofClient::addQualifier(const String &nameSpace,
					CIMQualifierDecl &qualifier) const
{
    _client->setQualifier(nameSpace, qualifier);
}

void
cimmofClient::addInstance(const String &nameSpace,
				       CIMInstance &instance) const
{
    _client->createInstance(nameSpace, instance);
}

CIMQualifierDecl
cimmofClient::getQualifierDecl(const String &nameSpace,
					    const String &qualifierName) const
{
    return (_client->getQualifier(nameSpace, qualifierName));
}

CIMClass
cimmofClient::getClass(const String &nameSpace,
				    const String &className) const
{
    return (_client->getClass(nameSpace, className));
}

void
cimmofClient::createNameSpace(const String &nameSpace) const
{
  //_client->createNameSpace(nameSpace);
}
