//%///-*-c++-*-/////////////////////////////////////////////////////////////////
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
// Author: Chip Vincent (cvincent@us.ibm.com)
//
// Modified By: Mike Brasher (mbrasher@bmc.com)
//              Yi Zhou, Hewlett-Packard Company (yi_zhou@hp.com)
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//              Mike Day, IBM (mdday@us.ibm.com)
//
//%/////////////////////////////////////////////////////////////////////////////


#ifndef Pegasus_CIMOMHandle_h
#define Pegasus_CIMOMHandle_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/ModuleController.h>
#include <Pegasus/Common/OperationContext.h>
#include <Pegasus/Common/CIMObject.h>
#include <Pegasus/Common/CIMObjectPath.h>
#include <Pegasus/Common/CIMClass.h>
#include <Pegasus/Common/CIMInstance.h>
#include <Pegasus/Common/CIMPropertyList.h>
#include <Pegasus/Common/ResponseHandler.h>
#include "Linkage.h"

PEGASUS_NAMESPACE_BEGIN

class ProviderManager;
class Provider;

class cimom_handle_op_semaphore;

class PEGASUS_PROVIDER_LINKAGE TimeoutContainer : public OperationContext::Container
{
   public:
      static const String NAME;
      
      TimeoutContainer(const OperationContext::Container & container);
      TimeoutContainer(Uint32 timeout);
      virtual String getName(void) const;
      virtual OperationContext::Container * clone(void) const;
      virtual void destroy(void);
      
      Uint32 getTimeOut(void) const;
   protected:
      Uint32 _value;
   private:
      TimeoutContainer(void);
};


class PEGASUS_PROVIDER_LINKAGE CIMOMHandle
{

   public:

      /** */
      CIMOMHandle(void);
      CIMOMHandle(const CIMOMHandle &);

      /** */
      virtual ~CIMOMHandle(void);

      CIMOMHandle & operator=(const CIMOMHandle & handle);
      
      CIMClass getClass(
	 const OperationContext & context,
	 const CIMNamespaceName& nameSpace,
	 const CIMName& className,
	 Boolean localOnly,
	 Boolean includeQualifiers,
	 Boolean includeClassOrigin,
	 const CIMPropertyList& propertyList);

      Array<CIMClass> enumerateClasses(
	 const OperationContext & context,
	 const CIMNamespaceName& nameSpace,
	 const CIMName& className,
	 Boolean deepInheritance,
	 Boolean localOnly,
	 Boolean includeQualifiers,
	 Boolean includeClassOrigin);

      Array<CIMName> enumerateClassNames(
	 const OperationContext & context,
	 const CIMNamespaceName& nameSpace,
	 const CIMName& className,
	 Boolean deepInheritance);

      void createClass(
	 const OperationContext & context,
	 const CIMNamespaceName& nameSpace,
	 const CIMClass& newClass);

      void modifyClass(
	 const OperationContext & context,
	 const CIMNamespaceName& nameSpace,
	 const CIMClass& modifiedClass);

      void deleteClass(
	 const OperationContext & context,
	 const CIMNamespaceName& nameSpace,
	 const CIMName& className);

      CIMInstance getInstance(
	 const OperationContext & context,
	 const CIMNamespaceName& nameSpace,
	 const CIMObjectPath& instanceName,
	 Boolean localOnly,
	 Boolean includeQualifiers,
	 Boolean includeClassOrigin,
	 const CIMPropertyList& propertyList);

      Array<CIMInstance> enumerateInstances(
	 const OperationContext & context,
	 const CIMNamespaceName& nameSpace,
	 const CIMName& className,
	 Boolean deepInheritance,
	 Boolean localOnly,
	 Boolean includeQualifiers,
	 Boolean includeClassOrigin,
	 const CIMPropertyList& propertyList);

      Array<CIMObjectPath> enumerateInstanceNames(
	 const OperationContext & context,
	 const CIMNamespaceName& nameSpace,
	 const CIMName& className);

      CIMObjectPath createInstance(
	 const OperationContext & context,
	 const CIMNamespaceName& nameSpace,
	 const CIMInstance& newInstance);

      void modifyInstance(
	 const OperationContext & context,
	 const CIMNamespaceName& nameSpace,
	 const CIMInstance& modifiedInstance,
	 Boolean includeQualifiers,
	 const CIMPropertyList& propertyList);

      void deleteInstance(
	 const OperationContext & context,
	 const CIMNamespaceName& nameSpace,
	 const CIMObjectPath& instanceName);

      Array<CIMObject> execQuery(
	 const OperationContext & context,
	 const CIMNamespaceName& nameSpace,
	 const String& queryLanguage,
	 const String& query);

      Array<CIMObject> associators(
	 const OperationContext & context,
	 const CIMNamespaceName& nameSpace,
	 const CIMObjectPath& objectName,
	 const CIMName& assocClass,
	 const CIMName& resultClass,
	 const String& role,
	 const String& resultRole,
	 Boolean includeQualifiers,
	 Boolean includeClassOrigin,
	 const CIMPropertyList& propertyList);

      Array<CIMObjectPath> associatorNames(
	 const OperationContext & context,
	 const CIMNamespaceName& nameSpace,
	 const CIMObjectPath& objectName,
	 const CIMName& assocClass,
	 const CIMName& resultClass,
	 const String& role,
	 const String& resultRole);

      Array<CIMObject> references(
	 const OperationContext & context,
	 const CIMNamespaceName& nameSpace,
	 const CIMObjectPath& objectName,
	 const CIMName& resultClass,
	 const String& role,
	 Boolean includeQualifiers,
	 Boolean includeClassOrigin,
	 const CIMPropertyList& propertyList);

      Array<CIMObjectPath> referenceNames(
	 const OperationContext & context,
	 const CIMNamespaceName& nameSpace,
	 const CIMObjectPath& objectName,
	 const CIMName& resultClass,
	 const String& role);

      // property operations
      CIMValue getProperty(
	 const OperationContext & context,
	 const CIMNamespaceName& nameSpace,
	 const CIMObjectPath& instanceName,
	 const CIMName& propertyName);

      void setProperty(
	 const OperationContext & context,
	 const CIMNamespaceName& nameSpace,
	 const CIMObjectPath& instanceName,
	 const CIMName& propertyName,
	 const CIMValue& newValue);

      CIMValue invokeMethod(
	 const OperationContext & context,
	 const CIMNamespaceName& nameSpace,
	 const CIMObjectPath& instanceName,
	 const CIMName& methodName,
	 const Array<CIMParamValue>& inParameters,
	 Array<CIMParamValue>& outParameters);



//   force provider manager to keep in memory
      virtual void protect(void);
// allow provider manager to unload when idle 
      virtual void unprotect(void);
      
      class _cimom_handle_rep;
      _cimom_handle_rep *_rep;
   private:
      friend class Provider;
      friend class cimom_handle_op_semaphore;
      virtual void get_idle_timer(struct timeval *);
      virtual void update_idle_timer(void);
      virtual Boolean pending_operation(void);
      virtual Boolean unload_ok(void);
};





PEGASUS_NAMESPACE_END

#endif
