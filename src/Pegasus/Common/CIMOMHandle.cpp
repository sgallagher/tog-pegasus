//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 The Open group, BMC Software, Tivoli Systems, IBM
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
// Author: Chip Vincent (cvincent@us.ibm.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include "CIMOMHandle.h"

PEGASUS_NAMESPACE_BEGIN

CIMOMHandle::CIMOMHandle(void) : _pCimom(0)
{
}

CIMOMHandle::CIMOMHandle(CIMOperations * pCimom) : _pCimom(0)
{
   if(pCimom == 0) {
      throw(UnitializedHandle());
   }

   _pCimom = pCimom;
}

CIMOMHandle::~CIMOMHandle(void)
{
}

CIMClass CIMOMHandle::getClass(
   const String & nameSpace,
   const String & className,
   Boolean localOnly,
   Boolean includeQualifiers,
   Boolean includeClassOrigin,
   const Array<String> & propertyList) const
{
   if(_pCimom == 0) {
      throw(UnitializedHandle());
   }

   return(_pCimom->getClass(nameSpace, className, localOnly, includeQualifiers, includeClassOrigin, propertyList));
}

CIMInstance CIMOMHandle::getInstance(
   const String & nameSpace,
   const CIMReference & instanceName,
   Boolean localOnly,
   Boolean includeQualifiers,
   Boolean includeClassOrigin,
   const Array<String> & propertyList) const
{
   if(_pCimom == 0) {
      throw(UnitializedHandle());
   }

   return(_pCimom->getInstance(nameSpace, instanceName, localOnly, includeQualifiers, includeClassOrigin, propertyList));
}

void CIMOMHandle::deleteClass(
   const String & nameSpace,
   const String & className) const
{
   if(_pCimom == 0) {
      throw(UnitializedHandle());
   }

   _pCimom->deleteClass(nameSpace, className);
}

void CIMOMHandle::deleteInstance(
   const String & nameSpace,
   const CIMReference & instanceName) const
{
   if(_pCimom == 0) {
      throw(UnitializedHandle());
   }

   _pCimom->deleteInstance(nameSpace, instanceName);
}

void CIMOMHandle::createClass(
   const String & nameSpace,
   const CIMClass & newClass) const
{
   if(_pCimom == 0) {
      throw(UnitializedHandle());
   }

   _pCimom->createClass(nameSpace, newClass);
}

void CIMOMHandle::createInstance(
   const String & nameSpace,
   const CIMInstance & newInstance) const
{
   if(_pCimom == 0) {
      throw(UnitializedHandle());
   }

   _pCimom->createInstance(nameSpace, newInstance);
}

void CIMOMHandle::modifyClass(
   const String & nameSpace,
   const CIMClass & modifiedClass) const
{
   if(_pCimom == 0) {
      throw(UnitializedHandle());
   }

   _pCimom->modifyClass(nameSpace, modifiedClass);
}

void CIMOMHandle::modifyInstance(
   const String & nameSpace,
   const CIMInstance & modifiedInstance) const
{
   if(_pCimom == 0) {
      throw(UnitializedHandle());
   }

   _pCimom->modifyInstance(nameSpace, modifiedInstance);
}

Array<CIMClass> CIMOMHandle::enumerateClasses(
   const String & nameSpace,
   const String & className,
   Boolean deepInheritance,
   Boolean localOnly,
   Boolean includeQualifiers,
   Boolean includeClassOrigin) const
{
   if(_pCimom == 0) {
      throw(UnitializedHandle());
   }

   return(_pCimom->enumerateClasses(nameSpace, className, deepInheritance, localOnly, includeQualifiers, includeClassOrigin));
}

Array<String> CIMOMHandle::enumerateClassNames(
   const String & nameSpace,
   const String & className,
   Boolean deepInheritance) const
{
   if(_pCimom == 0) {
      throw(UnitializedHandle());
   }

   return(_pCimom->enumerateClassNames(nameSpace, className, deepInheritance));
}

Array<CIMInstance> CIMOMHandle::enumerateInstances(
   const String & nameSpace,
   const String & className,
   Boolean deepInheritance,
   Boolean localOnly,
   Boolean includeQualifiers,
   Boolean includeClassOrigin,
   const Array<String> & propertyList) const
{
   if(_pCimom == 0) {
      throw(UnitializedHandle());
   }

   return(_pCimom->enumerateInstances(nameSpace, className, deepInheritance, localOnly, includeQualifiers, includeClassOrigin, propertyList));
}

Array<CIMReference> CIMOMHandle::enumerateInstanceNames(
   const String & nameSpace,
   const String & className) const
{
   if(_pCimom == 0) {
      throw(UnitializedHandle());
   }

   return(_pCimom->enumerateInstanceNames(nameSpace, className));
}

Array<CIMInstance> CIMOMHandle::execQuery(
   const String & queryLanguage,
   const String & query) const
{
   if(_pCimom == 0) {
      throw(UnitializedHandle());
   }

   return(_pCimom->execQuery(queryLanguage, query));
}

Array<CIMObjectWithPath> CIMOMHandle::associators(
   const String & nameSpace,
   const CIMReference& objectName,
   const String & assocClass,
   const String & resultClass,
   const String & role,
   const String & resultRole,
   Boolean includeQualifiers,
   Boolean includeClassOrigin,
   const Array<String> & propertyList) const
{
   if(_pCimom == 0) {
      throw(UnitializedHandle());
   }

   return(_pCimom->associators(nameSpace, objectName, assocClass, resultClass, role, resultRole, includeQualifiers, includeClassOrigin, propertyList));
}

Array<CIMReference> CIMOMHandle::associatorNames(
   const String & nameSpace,
   const CIMReference & objectName,
   const String & assocClass,
   const String & resultClass,
   const String & role,
   const String & resultRole) const
{
   if(_pCimom == 0) {
      throw(UnitializedHandle());
   }

   return(_pCimom->associatorNames(nameSpace, objectName, assocClass, resultClass, role, resultRole));
}

Array<CIMObjectWithPath> CIMOMHandle::references(
   const String & nameSpace,
   const CIMReference & objectName,
   const String & resultClass,
   const String & role,
   Boolean includeQualifiers,
   Boolean includeClassOrigin,
   const Array<String> & propertyList) const
{
   if(_pCimom == 0) {
      throw(UnitializedHandle());
   }

   return(_pCimom->references(nameSpace, objectName, resultClass, role, includeQualifiers, includeClassOrigin, propertyList));
}

Array<CIMReference> CIMOMHandle::referenceNames(
   const String & nameSpace,
   const CIMReference & objectName,
   const String & resultClass,
   const String & role) const
{
   if(_pCimom == 0) {
      throw(UnitializedHandle());
   }

   return(_pCimom->referenceNames(nameSpace, objectName, resultClass, role));
}

CIMValue CIMOMHandle::getProperty(
   const String & nameSpace,
   const CIMReference & instanceName,
   const String & propertyName) const
{
   if(_pCimom == 0) {
      throw(UnitializedHandle());
   }

   return(_pCimom->getProperty(nameSpace, instanceName, propertyName));
}

void CIMOMHandle::setProperty(
   const String & nameSpace,
   const CIMReference & instanceName,
   const String & propertyName,
   const CIMValue & newValue) const
{
   if(_pCimom == 0) {
      throw(UnitializedHandle());
   }

   _pCimom->setProperty(nameSpace, instanceName, propertyName, newValue);
}

CIMQualifierDecl CIMOMHandle::getQualifier(
   const String & nameSpace,
   const String & qualifierName) const
{
   if(_pCimom == 0) {
      throw(UnitializedHandle());
   }

   return(_pCimom->getQualifier(nameSpace, qualifierName));
}

void CIMOMHandle::setQualifier(
   const String & nameSpace,
   const CIMQualifierDecl & qualifierDecl) const
{
   if(_pCimom == 0) {
      throw(UnitializedHandle());
   }

   _pCimom->setQualifier(nameSpace, qualifierDecl);
}

void CIMOMHandle::deleteQualifier(
   const String & nameSpace,
   const String & qualifierName) const
{
   if(_pCimom == 0) {
      throw(UnitializedHandle());
   }

   _pCimom->deleteQualifier(nameSpace, qualifierName);
}

Array<CIMQualifierDecl> CIMOMHandle::enumerateQualifiers(
   const String & nameSpace) const
{
   if(_pCimom == 0) {
      throw(UnitializedHandle());
   }

   return(_pCimom->enumerateQualifiers(nameSpace));
}

CIMValue CIMOMHandle::invokeMethod(
   const String & nameSpace,
   const CIMReference & instanceName,
   const String & methodName,
   const Array<CIMValue> & inParameters,
   Array<CIMValue> & outParameters) const
{
   if(_pCimom == 0) {
      throw(UnitializedHandle());
   }

   return(_pCimom->invokeMethod(nameSpace, instanceName, methodName, inParameters, outParameters));
}

PEGASUS_NAMESPACE_END
