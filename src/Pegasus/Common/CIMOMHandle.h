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

#ifndef Pegasus_CIMOMHandle_h
#define Pegasus_CIMOMHandle_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Repository/CIMRepository.h>

PEGASUS_NAMESPACE_BEGIN

class PEGASUS_COMMON_LINKAGE CIMOMHandle
{
public:
   CIMOMHandle(void);
   CIMOMHandle(CIMOperations * pCimom);
   virtual ~CIMOMHandle(void);

   virtual CIMClass getClass(
      const String& nameSpace,
      const String& className,
      Boolean localOnly = true,
      Boolean includeQualifiers = true,
      Boolean includeClassOrigin = false,
      const Array<String>& propertyList = Array<String>()) const;

   virtual CIMInstance getInstance(
      const String& nameSpace,
      const CIMReference& instanceName,
      Boolean localOnly = true,
      Boolean includeQualifiers = false,
      Boolean includeClassOrigin = false,
      const Array<String>& propertyList = Array<String>()) const;

   virtual void deleteClass(
      const String& nameSpace,
      const String& className) const;

   virtual void deleteInstance(
      const String& nameSpace,
      const CIMReference& instanceName) const;

   virtual void createClass(
      const String& nameSpace,
      const CIMClass& newClass) const;

   virtual void createInstance(
      const String& nameSpace,
      const CIMInstance& newInstance) const;

   virtual void modifyClass(
      const String& nameSpace,
      const CIMClass& modifiedClass) const;

   virtual void modifyInstance(
      const String& nameSpace,
      const CIMInstance& modifiedInstance) const;

   virtual Array<CIMClass> enumerateClasses(
      const String& nameSpace,
      const String& className = String::EMPTY,
      Boolean deepInheritance = false,
      Boolean localOnly = true,
      Boolean includeQualifiers = true,
      Boolean includeClassOrigin = false) const;

   virtual Array<String> enumerateClassNames(
      const String& nameSpace,
      const String& className = String::EMPTY,
      Boolean deepInheritance = false) const;

   virtual Array<CIMInstance> enumerateInstances(
      const String& nameSpace,
      const String& className,
      Boolean deepInheritance = true,
      Boolean localOnly = true,
      Boolean includeQualifiers = false,
      Boolean includeClassOrigin = false,
      const Array<String>& propertyList = Array<String>()) const;

   virtual Array<CIMReference> enumerateInstanceNames(
      const String& nameSpace,
      const String& className) const;

   virtual Array<CIMInstance> execQuery(
      const String& queryLanguage,
      const String& query) const;

   virtual Array<CIMObjectWithPath> associators(
      const String& nameSpace,
      const CIMReference& objectName,
      const String& assocClass = String::EMPTY,
      const String& resultClass = String::EMPTY,
      const String& role = String::EMPTY,
      const String& resultRole = String::EMPTY,
      Boolean includeQualifiers = false,
      Boolean includeClassOrigin = false,
      const Array<String>& propertyList = Array<String>()) const;

   virtual Array<CIMReference> associatorNames(
      const String& nameSpace,
      const CIMReference& objectName,
      const String& assocClass = String::EMPTY,
      const String& resultClass = String::EMPTY,
      const String& role = String::EMPTY,
      const String& resultRole = String::EMPTY) const;

   virtual Array<CIMObjectWithPath> references(
      const String& nameSpace,
      const CIMReference& objectName,
      const String& resultClass = String::EMPTY,
      const String& role = String::EMPTY,
      Boolean includeQualifiers = false,
      Boolean includeClassOrigin = false,
      const Array<String>& propertyList = Array<String>()) const;

   virtual Array<CIMReference> referenceNames(
      const String& nameSpace,
      const CIMReference& objectName,
      const String& resultClass = String::EMPTY,
      const String& role = String::EMPTY) const;

   virtual CIMValue getProperty(
      const String& nameSpace,
      const CIMReference& instanceName,
      const String& propertyName) const;

   virtual void setProperty(
      const String& nameSpace,
      const CIMReference& instanceName,
      const String& propertyName,
      const CIMValue& newValue = CIMValue()) const;

   virtual CIMQualifierDecl getQualifier(
      const String& nameSpace,
      const String& qualifierName) const;

   virtual void setQualifier(
      const String& nameSpace,
      const CIMQualifierDecl& qualifierDecl) const;

   virtual void deleteQualifier(
      const String& nameSpace,
      const String& qualifierName) const;

   virtual Array<CIMQualifierDecl> enumerateQualifiers(
      const String& nameSpace) const;

   virtual CIMValue invokeMethod(
      const String& nameSpace,
      const CIMReference& instanceName,
      const String& methodName,
      const Array<CIMValue>& inParameters,
      Array<CIMValue>& outParameters) const;

protected:
   mutable CIMOperations * _pCimom;

};

PEGASUS_NAMESPACE_END

#endif
