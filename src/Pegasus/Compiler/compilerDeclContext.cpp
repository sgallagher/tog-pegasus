//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000 The Open Group, BMC Software, Tivoli Systems, IBM
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
//==============================================================================
//
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By:  Bob Blair (bblair@bmc.com)
//
//%/////////////////////////////////////////////////////////////////////////////


#include "compilerDeclContext.h"

compilerDeclContext::compilerDeclContext(cimmofRepository *repository,
					 cimmofRepository::operationType ot) :
  RepositoryDeclContext(repository), _cimmofRepository(repository), _ot(ot)
{
}

compilerDeclContext::~compilerDeclContext() {}

CIMQualifierDecl
compilerDeclContext::lookupQualifierDecl(const String &nameSpace,
					 const String &qualifierName) const
{
  const CIMQualifierDecl *pTheQualifier = 0;
  if (_ot != cimmofRepository::USE_REPOSITORY) {
    if ( (pTheQualifier = 
	  _findQualifierInMemory(qualifierName)) )
      return *pTheQualifier;
  }
  if (_repository && (_ot != cimmofRepository::IGNORE_REPOSITORY)) {
    try {
      return _repository->getQualifier(nameSpace, qualifierName);
    } catch (Exception &) {
      // FIXME:  catch the NOT_FOUND condition.  Don't catch others.
      // ignore.  It either isn't there or something's broken.
    }
  }
  return CIMQualifierDecl();
}

CIMClass
compilerDeclContext::lookupClass(const String &nameSpace,
				 const String &className) const
{
  const CIMClass *pTheClass;
  if (_ot != cimmofRepository::USE_REPOSITORY) {
    if ( (pTheClass =_findClassInMemory(className)) )
      return *pTheClass;
  }
  if (_repository && _ot != cimmofRepository::IGNORE_REPOSITORY) {
    try {
      return _repository->getClass(nameSpace, className);
    } catch(Exception &) {
      // ignore failure to find.  FIXME:  pass others on.
    }
  }
  return CIMClass();
}

void
compilerDeclContext::addQualifierDecl(const String &nameSpace,
				      const CIMQualifierDecl &x)
{
  if (_ot != cimmofRepository::USE_REPOSITORY)
    _qualifiers.append(x);
  else
    _repository->setQualifier(nameSpace, x);
}

void
compilerDeclContext::addClass(const String &nameSpace, CIMClass &x)
{
  if (_ot != cimmofRepository::USE_REPOSITORY)
    _classes.append(x);
  else
    _repository->createClass(nameSpace, x);
}

const CIMClass *
compilerDeclContext::_findClassInMemory(const String &classname) const
{
  for (unsigned int i = 0; i < _classes.size(); i++) {
    if (String::equal(classname, (_classes[i]).getClassName()))
	return &(_classes[i]);
  }
  return 0;
}

const CIMQualifierDecl *
compilerDeclContext::_findQualifierInMemory(const String &classname) const
{
  for (unsigned int i = 0; i < _qualifiers.size(); i++) {
    if (String::equal(classname, (_qualifiers[i]).getName()))
	return &(_qualifiers[i]);
  }
  return 0;
}
