//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software, Hewlett-Packard Company, IBM,
// The Open Group, Tivoli Systems
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
// Modified By: Carol Ann Krug Graves, Hewlett-Packard Company
//                  (carolann_graves@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////
//
// This class is the interface to allow a common call to serve both the repository
// and the client interface.  Since these two interfaces were not designed with a
// common hiearchy, it substitutes the individual calls with any special characteristics
// for each call. Note that Bob Blair in the Compiler created much the same structure but
// with different operations.   
//
// This class supports only the operations that tomof needs, which
// are
//     enumerateQualifiers()
//     enumerateClassNames()
//     enumerateClasses()
//     enumerateInstances()
//
//  ACTION: In the future we should regularalize this so we have one class that can serve
//  multiple programs. However, since the compiler extended the repository definitions
//  and uses different functions, it was easier just to create a new class
//

#ifndef CLIENT_REPOSITORY_INTERFACE_H_
#define CLIENT_REPOSITORY_INTERFACE_H_

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/CIMPropertyList.h>
#include <Pegasus/Common/CIMObjectPath.h>


PEGASUS_NAMESPACE_BEGIN

// Forward declarations
class CIMRepository;
class CIMClient;
class CIMClass;
class CIMQualifierDecl;
class CIMInstance;


class clientRepositoryInterface
{
 private:
  CIMRepository *_repository;
  CIMClient     *_client;

 public:
  enum _repositoryType { REPOSITORY_INTERFACE_LOCAL = 0,
                          REPOSITORY_INTERFACE_CLIENT
  };
  clientRepositoryInterface();

  ~clientRepositoryInterface();

  void init(_repositoryType type, String location);

  Boolean ok() const { return _repository || _client; }

  virtual Array<CIMQualifierDecl> enumerateQualifiers(const String &nameSpace) const;

  virtual CIMClass getClass(
      const String& nameSpace,
      const String& className,
      const Boolean localOnly,
      const Boolean includeQualifiers,
      const Boolean includeClassOrigin) const;

  virtual Array<CIMClass> enumerateClasses(
      const String& nameSpace,
      const String& className,
      const Boolean deepInheritance,
      const Boolean localOnly,
      const Boolean includeQualifiers,
      const Boolean includeClassOrigin) const;

  virtual Array<CIMName> enumerateClassNames(
      const String& nameSpace,
      const String& className,
      const Boolean deepInheritance);

  virtual Array<CIMObjectPath> enumerateInstanceNames(
	  const String& nameSpace,
	  const String& className);

  virtual Array<CIMInstance> enumerateInstances(
	const String& nameSpace,
	const String& className,
	Boolean deepInheritance = true,
	Boolean localOnly = true,
	Boolean includeQualifiers = false,
	Boolean includeClassOrigin = false,
	const CIMPropertyList& propertyList = CIMPropertyList());

};

PEGASUS_NAMESPACE_END

#endif /* CLIENT_REPOSITORY_INTERFACE_H_ */


