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
//              Terry Martin, Hewlett-Packard Company (terry.martin@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMQualifierDecl.h>
#include <Pegasus/Common/CIMClass.h>
#include <Pegasus/Common/CIMInstance.h>
#include "cimmofRepository.h"
#include "cimmofClient.h"
#include "cimmofParser.h"
#include "cimmofMessages.h"
#include "cimmofRepositoryInterface.h"

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
				const CIMRepository_Mode Mode,
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
	_repository = new cimmofRepository(location, Mode,  _ot);
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
    } catch(Exception &e) {
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
cimmofRepositoryInterface::addClass(const CIMNamespaceName &nameSpace, 
    CIMClass &Class)
  const {
  if (_repository)
    _repository->addClass(nameSpace, &Class);
  if (_client)
      _client->addClass(nameSpace, Class);
}

void
cimmofRepositoryInterface::addQualifier(const CIMNamespaceName &nameSpace,
					CIMQualifierDecl &qualifier) const
{
  if (_repository)
    _repository->addQualifier(nameSpace, &qualifier);
  if (_client)
    _client->addQualifier(nameSpace, qualifier);
}

void
cimmofRepositoryInterface::addInstance(const CIMNamespaceName &nameSpace,
				       CIMInstance &instance) const
{
  if (_repository)
    _repository->addInstance(nameSpace, &instance);
  if (_client)
    _client->addInstance(nameSpace, instance);
}

CIMQualifierDecl
cimmofRepositoryInterface::getQualifierDecl(const CIMNamespaceName &nameSpace,
					    const CIMName &qualifierName) const
{
  if (_repository)
    return (_repository->getQualifierDecl(nameSpace, qualifierName));
  else if (_client)
    return (_client->getQualifierDecl(nameSpace, qualifierName));
  else
    return CIMQualifierDecl();
}

CIMClass
cimmofRepositoryInterface::getClass(const CIMNamespaceName &nameSpace,
				    const CIMName &className) const
{
  if (_repository)
    return (_repository->getClass(nameSpace, className));
  else if (_client)
    return (_client->getClass(nameSpace, className));
  else
    return CIMClass();
}

void
cimmofRepositoryInterface::modifyClass(const CIMNamespaceName &nameSpace, 
    CIMClass &Class)
  const {
  if (_repository)
  {
    _repository->modifyClass(nameSpace, &Class);
  }
  if (_client)
  {
    _client->modifyClass(nameSpace, Class);
  }
}

void
cimmofRepositoryInterface::createNameSpace(
    const CIMNamespaceName &nameSpace) const
{
  if (_repository)
    _repository->createNameSpace(nameSpace);
  else if (_client)
  {
      _client->createNameSpace(nameSpace);
  }
}
