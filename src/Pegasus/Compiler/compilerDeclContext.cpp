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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By:  Bob Blair (bblair@bmc.com)
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                (carolann_graves@hp.com)
//              Gerarda Marquez (gmarquez@us.ibm.com)
//              -- PEP 43 changes
//
//%////////////////////////////////////////////////////////////////////////////


#include "compilerDeclContext.h"

PEGASUS_NAMESPACE_BEGIN

compilerDeclContext::compilerDeclContext(CIMRepository *repository,
					compilerCommonDefs::operationType ot) :
  RepositoryDeclContext(repository), _cimRepository(repository), _ot(ot)
{
  if (!repository && ot != compilerCommonDefs::IGNORE_REPOSITORY)
    throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
				"attempt to initialize repository with "
				"invalid data");
}

compilerDeclContext::~compilerDeclContext() {}

CIMQualifierDecl
compilerDeclContext::lookupQualifierDecl(const CIMNamespaceName &nameSpace,
					 const CIMName &qualifierName) const
{
  const CIMQualifierDecl *pTheQualifier = 0;
  if (_ot != compilerCommonDefs::USE_REPOSITORY) {
    if ( (pTheQualifier = 
	  _findQualifierInMemory(qualifierName)) )
      return *pTheQualifier;
  }
  if (_repository && (_ot != compilerCommonDefs::IGNORE_REPOSITORY)) {
      return _repository->_getQualifier(nameSpace, qualifierName);
  }
  return CIMQualifierDecl();
}

CIMClass
compilerDeclContext::lookupClass(const CIMNamespaceName &nameSpace,
				 const CIMName &className) const
{
  const CIMClass *pTheClass;

  if (_ot != compilerCommonDefs::USE_REPOSITORY) {
    if ( (pTheClass =_findClassInMemory(className)) )
      return *pTheClass;
  }
  if (_repository && _ot != compilerCommonDefs::IGNORE_REPOSITORY) {
      return _repository->_getClass(
          nameSpace, className, false, true, true, CIMPropertyList());
  }
  return CIMClass();
}

void
compilerDeclContext::addQualifierDecl(const CIMNamespaceName &nameSpace,
				      const CIMQualifierDecl &x)
{
  if (_ot != compilerCommonDefs::USE_REPOSITORY)
    _qualifiers.append(x);
  else
    _repository->_setQualifier(nameSpace, x);
}

void
compilerDeclContext::addClass(const CIMNamespaceName &nameSpace, CIMClass &x)
{
  if (_ot != compilerCommonDefs::USE_REPOSITORY)
    _classes.append(x);
  else
    _repository->_createClass(nameSpace, x);
}

void
compilerDeclContext::addInstance(const CIMNamespaceName &nameSpace, 
                                 CIMInstance &x)
{
  if (_ot == compilerCommonDefs::USE_REPOSITORY)
    _repository->_createInstance(nameSpace, x);
}

void
compilerDeclContext::modifyClass(const CIMNamespaceName &nameSpace, CIMClass &x)
{
  if (_ot != compilerCommonDefs::USE_REPOSITORY)
  {
    _classes.append(x);
  }
  else
  {
    _repository->_modifyClass(nameSpace, x);
  }
}

const CIMClass *
compilerDeclContext::_findClassInMemory(const CIMName &classname) const
{
  for (unsigned int i = 0; i < _classes.size(); i++) {
    if (classname.equal (_classes[i].getClassName()))
	return &(_classes[i]);
  }
  return 0;
}

const CIMQualifierDecl *
compilerDeclContext::_findQualifierInMemory(const CIMName &classname) const
{
  for (unsigned int i = 0; i < _qualifiers.size(); i++) {
    if (classname.equal (_qualifiers[i].getName()))
	return &(_qualifiers[i]);
  }
  return 0;
}

PEGASUS_NAMESPACE_END
