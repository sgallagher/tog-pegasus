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

static const char INTEROP_NAMESPACE [] = "root/PG_InterOp";
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
  try
  {
    _client->createClass(nameSpace, Class);
  }
  catch (CIMClientCIMException &eCIMClient)
  {
     CIMException e = CIMException(eCIMClient.getCode(), 
                                   eCIMClient.getMessage());
     throw e;
  }
  catch (CIMClientException &eCIMClient)
  {
     Exception e  = Exception(eCIMClient.getMessage());
     throw e;
  }
}

void
cimmofClient::addQualifier(const String &nameSpace,
					CIMQualifierDecl &qualifier) const
{
  try
  {
    _client->setQualifier(nameSpace, qualifier);
  }
  catch (CIMClientCIMException &eCIMClient)
  {
     CIMException e = CIMException(eCIMClient.getCode(), 
                                   eCIMClient.getMessage());
     throw e;
  }
  catch (CIMClientException &eCIMClient)
  {
     Exception e  = Exception(eCIMClient.getMessage());
     throw e;
  }
}

void
cimmofClient::addInstance(const String &nameSpace,
				       CIMInstance &instance) const
{
  try
  {
    _client->createInstance(nameSpace, instance);
  }
  catch (CIMClientCIMException &eCIMClient)
  {
     CIMException e = CIMException(eCIMClient.getCode(), 
                                   eCIMClient.getMessage());
     throw e;
  }
  catch (CIMClientException &eCIMClient)
  {
     Exception e  = Exception(eCIMClient.getMessage());
     throw e;
  }
}

CIMQualifierDecl
cimmofClient::getQualifierDecl(const String &nameSpace,
					    const String &qualifierName) const
{
  try
  {
    return (_client->getQualifier(nameSpace, qualifierName));
  }
  catch (CIMClientCIMException &eCIMClient)
  {
     CIMException e = CIMException(eCIMClient.getCode(), 
                                   eCIMClient.getMessage());
     throw e;
  }
  catch (CIMClientException &eCIMClient)
  {
     Exception e  = Exception(eCIMClient.getMessage());
     throw e;
  }
}

CIMClass
cimmofClient::getClass(const String &nameSpace,
				    const String &className) const
{
  try
  {
    return (_client->getClass(nameSpace, className));
  }
  catch (CIMClientCIMException &eCIMClient)
  {
     CIMException e = CIMException(eCIMClient.getCode(), 
                                   eCIMClient.getMessage());
     throw e;
  }
  catch (CIMClientException &eCIMClient)
  {
     Exception e  = Exception(eCIMClient.getMessage());
     throw e;
  }
}

void
cimmofClient::createNameSpace(const String &nameSpace) const
{
  try
  {
    CIMInstance newInstance(NAMESPACE_CLASS);
    newInstance.addProperty(
              CIMProperty(NAMESPACE_PROPERTY_NAME, nameSpace));
    _client->createInstance(INTEROP_NAMESPACE, newInstance);
  }
  catch (CIMClientCIMException &eCIMClient)
  {
     CIMException e = CIMException(eCIMClient.getCode(), 
                                   eCIMClient.getMessage());
     throw e;
  }
  catch (CIMClientException &eCIMClient)
  {
     Exception e  = Exception(eCIMClient.getMessage());
     throw e;
  }
}
