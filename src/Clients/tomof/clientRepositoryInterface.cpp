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
// Author: Karl Schopmeyer (k.schopmeyer@opengroup.org)
//
// Modified By:  Carol Ann Krug Graves, Hewlett-Packard Company
//               (carolann_graves@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Repository/CIMRepository.h>
#include <Pegasus/Common/CIMClass.h>
#include <Pegasus/Common/CIMInstance.h>
#include <Pegasus/Client/CIMClient.h>
#include <Pegasus/Common/Exception.h>
#include "clientRepositoryInterface.h"

PEGASUS_USING_STD;
PEGASUS_NAMESPACE_BEGIN

clientRepositoryInterface::clientRepositoryInterface() :
  _repository(0),
  _client(0)
{
}

clientRepositoryInterface::~clientRepositoryInterface() 
{
  if (_repository)
    delete _repository;
  if (_client)
    delete _client;
}

void
clientRepositoryInterface::init(_repositoryType type, 
                                const String &location)
{
  String message;
  // _ot = ot;
  if (type == REPOSITORY_INTERFACE_LOCAL)
  {
      _repository = new CIMRepository(location);
      // test to find if repository exists.
  }
  else if (type == REPOSITORY_INTERFACE_CLIENT) 
  {
    // create a CIMClient object and put it in _client
    try
    {
        Uint32 index = location.find (':');
        String host = location.subString (0, index);
        Uint32 portNumber = 0;
        if (index != PEG_NOT_FOUND)
        {
            String portStr = location.subString (index + 1, location.size ());
            sscanf (portStr.getCString (), "%u", &portNumber);
        }
        cout << "open " << host << " port " << portNumber << endl;
        _client = new CIMClient();
        _client->connect (host, portNumber, String::EMPTY, String::EMPTY);
    } 
    
    catch(Exception &e) 
    {
	  cerr << "Internal Error:" << e.getMessage() << endl;
      delete _client;
      _client = 0;
    }
  }
  else 
  {
	  throw IndexOutOfBoundsException();
  }
}


Array<CIMQualifierDecl> clientRepositoryInterface::enumerateQualifiers(
    const CIMNamespaceName& nameSpace) const 
{
  if (_repository)
    return _repository->enumerateQualifiers(nameSpace);
  if (_client)
      return _client->enumerateQualifiers(nameSpace);
	throw IndexOutOfBoundsException();
}

CIMClass clientRepositoryInterface::getClass(
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    const Boolean localOnly,
    const Boolean includeQualifiers,
    const Boolean includeClassOrigin) const
{
    if (_repository)
        return _repository->getClass(nameSpace, className,
                        localOnly, includeQualifiers, includeClassOrigin);
    if (_client)
        return _client->getClass(nameSpace, className,
                        localOnly, includeQualifiers, includeClassOrigin);
	throw IndexOutOfBoundsException();
};


Array<CIMClass> clientRepositoryInterface::enumerateClasses(
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    const Boolean deepInheritance,
    const Boolean localOnly,
    const Boolean includeQualifiers,
    const Boolean includeClassOrigin) const
{
    if (_repository)
        return _repository->enumerateClasses(
                                    nameSpace,
                                    className,
                                    deepInheritance,
                                    localOnly,
                                    includeQualifiers,
                                    includeClassOrigin);

     if (_client)

       return _client->enumerateClasses(
                                    nameSpace,
                                    className,
                                    deepInheritance,
                                    localOnly,
                                    includeQualifiers,
                                    includeClassOrigin);
	 throw IndexOutOfBoundsException();
};
Array<CIMName> clientRepositoryInterface::enumerateClassNames(
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    const Boolean deepInheritance)

{
    if (_repository)
    {
       Array<CIMName> classNameArray = _repository->enumerateClassNames
           (nameSpace, className, deepInheritance);
       // ATTN: Temporary code until the repository uses CIMName
       Array<CIMName> cimNameArray;
       for (Uint32 i = 0; i < classNameArray.size(); i++)
       {
           cimNameArray.append(classNameArray[i]);
       }
       return cimNameArray;
    }

    if (_client)
       return _client->enumerateClassNames(nameSpace, className, deepInheritance);
	throw IndexOutOfBoundsException();
};

Array<CIMObjectPath> clientRepositoryInterface::enumerateInstanceNames(
	const CIMNamespaceName& nameSpace,
	const CIMName& className)
{
    if (_repository)
       return _repository->enumerateInstanceNames(nameSpace, className);

    if (_client)
       return _client->enumerateInstanceNames(nameSpace, className);
	throw IndexOutOfBoundsException();
};

Array<CIMInstance> clientRepositoryInterface::enumerateInstances(
	const CIMNamespaceName& nameSpace,
	const CIMName& className,
	Boolean deepInheritance,
	Boolean localOnly,
	Boolean includeQualifiers,
	Boolean includeClassOrigin,
	const CIMPropertyList& propertyList)
	
{
    if (_repository)
       return _repository->enumerateInstances(nameSpace, className,
			deepInheritance, localOnly,includeQualifiers,includeClassOrigin);

    if (_client)
       return _client->enumerateInstances(nameSpace, className,
		   deepInheritance, localOnly,includeQualifiers,includeClassOrigin);
	throw IndexOutOfBoundsException();
};


PEGASUS_NAMESPACE_END

