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

#include "cimmofRepositoryInterface.h"
#include "cimmofRepository.h"
#include "cimmofClient.h"
#include "cimmofParser.h"
#include "cimmofMessages.h"
#include <Pegasus/Common/CIMClass.h>
#include <Pegasus/Common/CIMQualifierDecl.h>
#include <Pegasus/Common/CIMInstance.h>

PEGASUS_USING_PEGASUS;

cimmofRepositoryInterface::cimmofRepositoryInterface() :
  _repository(0),
  _client(0),
  _ot(compilerCommonDefs::USE_REPOSITORY)
{
}

cimmofRepositoryInterface::~cimmofRepositoryInterface() {
  if (_repository)
    delete _repository;
  if (_client)
    delete _client;
}

void
cimmofRepositoryInterface::init(_repositoryType type, String location,
				compilerCommonDefs::operationType ot)
{
  String message;
  cimmofMessages::arglist arglist;
  _ot = ot;
  if (type == REPOSITORY_INTERFACE_LOCAL) {
    // create a cimmofRepository object and put it in _repository
    cimmofParser *p = cimmofParser::Instance();
    const String NameSpace = p->getDefaultNamespacePath();
    if (location != "") {
      try {
      _repository = new cimmofRepository(location, _ot);
      } catch(Exception &e) {
        arglist.append(location);
        arglist.append(e.getMessage());
        cimmofMessages::getMessage(message,
                                  cimmofMessages::REPOSITORY_CREATE_ERROR,
                                  arglist);
        p->elog(message);
        delete _repository;
        _repository = 0;
      }
    }
  } else if (type == REPOSITORY_INTERFACE_CLIENT) {
    // create a CIMClient object and put it in _client
    _client = new cimmofClient();
    try {
      _client->init(location, ot);
    } catch(CIMClientException &e) {
      arglist.append(location);
      arglist.append(e.getMessage());
      cimmofMessages::getMessage(message,
				 cimmofMessages::REPOSITORY_CREATE_ERROR,
				 arglist);
      cimmofParser *p = cimmofParser::Instance();
      p->elog(message);
      delete _client;
      _client = 0;
    }
  } else {
    // throw an exception
  }
}

void
cimmofRepositoryInterface::addClass(const String &nameSpace, CIMClass &Class)
  const {
  if (_repository)
    _repository->addClass(nameSpace, &Class);
  if (_client)
      _client->addClass(nameSpace, Class);
}

void
cimmofRepositoryInterface::addQualifier(const String &nameSpace,
					CIMQualifierDecl &qualifier) const
{
  if (_repository)
    _repository->addQualifier(nameSpace, &qualifier);
  if (_client)
    // Convert CIMClientException to Exception so the caller catches it
    try
    {
      _client->addQualifier(nameSpace, qualifier);
    }
    catch (CIMClientException& e)
    {
      throw Exception(e.getMessage());
    }
}

void
cimmofRepositoryInterface::addInstance(const String &nameSpace,
				       CIMInstance &instance) const
{
  if (_repository)
    _repository->addInstance(nameSpace, &instance);
  if (_client)
    // Convert CIMClientException to Exception so the caller catches it
    try
    {
      _client->addInstance(nameSpace, instance);
    }
    catch (CIMClientException& e)
    {
      throw Exception(e.getMessage());
    }
}

CIMQualifierDecl
cimmofRepositoryInterface::getQualifierDecl(const String &nameSpace,
					    const String &qualifierName) const
{
  if (_repository)
    return (_repository->getQualifierDecl(nameSpace, qualifierName));
  else if (_client)
    // Convert CIMClientException to Exception so the caller catches it
    try
    {
      return (_client->getQualifierDecl(nameSpace, qualifierName));
    }
    catch (CIMClientException& e)
    {
      throw Exception(e.getMessage());
    }
  else
    return CIMQualifierDecl();
}

CIMClass
cimmofRepositoryInterface::getClass(const String &nameSpace,
				    const String &className) const
{
  if (_repository)
    return (_repository->getClass(nameSpace, className));
  else if (_client)
    // Convert CIMClientException to Exception so the caller catches it
    try
    {
      return (_client->getClass(nameSpace, className));
    }
    catch (CIMClientException& e)
    {
      throw Exception(e.getMessage());
    }
  else
    return CIMClass();
}

void
cimmofRepositoryInterface::createNameSpace(const String &nameSpace) const
{
  if (_repository)
    _repository->createNameSpace(nameSpace);
  else if (_client)
  {
      _client->createNameSpace(nameSpace);
  }
}
