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
class PEGASUS_PROVIDER_LINKAGE CIMOMHandle
{

   public:

      /** */
      CIMOMHandle(void);
      CIMOMHandle(const CIMOMHandle &);

      /** */
      virtual ~CIMOMHandle(void);

      CIMOMHandle & operator=(const CIMOMHandle & handle);

      virtual CIMClass getClass(
	 const OperationContext & context,
	 const CIMNamespaceName& nameSpace,
	 const CIMName& className,
	 Boolean localOnly,
	 Boolean includeQualifiers,
	 Boolean includeClassOrigin,
	 const CIMPropertyList& propertyList);

      virtual void getClassAsync(
	 const OperationContext & context,
	 const CIMNamespaceName& nameSpace,
	 const CIMName& className,
	 Boolean localOnly,
	 Boolean includeQualifiers,
	 Boolean includeClassOrigin,
	 const CIMPropertyList& propertyList,
	 ClassResponseHandler & handler);

      virtual Array<CIMClass> enumerateClasses(
	 const OperationContext & context,
	 const CIMNamespaceName& nameSpace,
	 const CIMName& className,
	 Boolean deepInheritance,
	 Boolean localOnly,
	 Boolean includeQualifiers,
	 Boolean includeClassOrigin);

      virtual void enumerateClassesAsync(
	 const OperationContext & context,
	 const CIMNamespaceName& nameSpace,
	 const CIMName& className,
	 Boolean deepInheritance,
	 Boolean localOnly,
	 Boolean includeQualifiers,
	 Boolean includeClassOrigin,
	 ClassResponseHandler & handler);

      virtual Array<CIMName> enumerateClassNames(
	 const OperationContext & context,
	 const CIMNamespaceName& nameSpace,
	 const CIMName& className,
	 Boolean deepInheritance);

      virtual void enumerateClassNamesAsync(
	 const OperationContext & context,
	 const CIMNamespaceName& nameSpace,
	 const CIMName& className,
	 Boolean deepInheritance,
	 ObjectPathResponseHandler & handler);

      virtual void createClass(
	 const OperationContext & context,
	 const CIMNamespaceName& nameSpace,
	 const CIMClass& newClass);

      virtual void createClassAsync(
	 const OperationContext & context,
	 const CIMNamespaceName& nameSpace,
	 const CIMClass& newClass,
	 ResponseHandler & handler);

      virtual void modifyClass(
	 const OperationContext & context,
	 const CIMNamespaceName& nameSpace,
	 const CIMClass& modifiedClass);

      virtual void modifyClassAsync(
	 const OperationContext & context,
	 const CIMNamespaceName& nameSpace,
	 const CIMClass& modifiedClass,
	 ResponseHandler& handler);

      virtual void deleteClass(
	 const OperationContext & context,
	 const CIMNamespaceName& nameSpace,
	 const CIMName& className);

      virtual void deleteClassAsync(
	 const OperationContext & context,
	 const CIMNamespaceName& nameSpace,
	 const CIMName& className,
	 ResponseHandler & handler);

      virtual CIMInstance getInstance(
	 const OperationContext & context,
	 const CIMNamespaceName& nameSpace,
	 const CIMObjectPath& instanceName,
	 Boolean localOnly,
	 Boolean includeQualifiers,
	 Boolean includeClassOrigin,
	 const CIMPropertyList& propertyList);

      virtual void getInstanceAsync(
	 const OperationContext & context,
	 const CIMNamespaceName& nameSpace,
	 const CIMObjectPath& instanceName,
	 Boolean localOnly,
	 Boolean includeQualifiers,
	 Boolean includeClassOrigin,
	 const CIMPropertyList& propertyList,
	 InstanceResponseHandler & handler);

      virtual Array<CIMInstance> enumerateInstances(
	 const OperationContext & context,
	 const CIMNamespaceName& nameSpace,
	 const CIMName& className,
	 Boolean deepInheritance,
	 Boolean localOnly,
	 Boolean includeQualifiers,
	 Boolean includeClassOrigin,
	 const CIMPropertyList& propertyList);

      virtual void enumerateInstancesAsync(
	 const OperationContext & context,
	 const CIMNamespaceName& nameSpace,
	 const CIMName& className,
	 Boolean deepInheritance,
	 Boolean localOnly,
	 Boolean includeQualifiers,
	 Boolean includeClassOrigin,
	 const CIMPropertyList& propertyList,
	 InstanceResponseHandler & handler);

      virtual Array<CIMObjectPath> enumerateInstanceNames(
	 const OperationContext & context,
	 const CIMNamespaceName& nameSpace,
	 const CIMName& className);

      virtual void enumerateInstanceNamesAsync(
	 const OperationContext & context,
	 const CIMNamespaceName& nameSpace,
	 const CIMName& className,
	 ObjectPathResponseHandler & handler);

      virtual CIMObjectPath createInstance(
	 const OperationContext & context,
	 const CIMNamespaceName& nameSpace,
	 const CIMInstance& newInstance);

      virtual void createInstanceAsync(
	 const OperationContext & context,
	 const CIMNamespaceName& nameSpace,
	 const CIMInstance& newInstance,
	 ResponseHandler& handler);

      virtual void modifyInstance(
	 const OperationContext & context,
	 const CIMNamespaceName& nameSpace,
	 const CIMInstance& modifiedInstance,
	 Boolean includeQualifiers,
	 const CIMPropertyList& propertyList);

      virtual void modifyInstanceAsync(
	 const OperationContext & context,
	 const CIMNamespaceName& nameSpace,
	 const CIMInstance& modifiedInstance,
	 Boolean includeQualifiers,
	 const CIMPropertyList& propertyList,
	 ResponseHandler& handler);

      virtual void deleteInstance(
	 const OperationContext & context,
	 const CIMNamespaceName& nameSpace,
	 const CIMObjectPath& instanceName);

      virtual void deleteInstanceAsync(
	 const OperationContext & context,
	 const CIMNamespaceName& nameSpace,
	 const CIMObjectPath& instanceName,
	 ResponseHandler& handler);

      virtual Array<CIMObject> execQuery(
	 const OperationContext & context,
	 const CIMNamespaceName& nameSpace,
	 const String& queryLanguage,
	 const String& query);

      virtual void execQueryAsync(
	 const OperationContext & context,
	 const CIMNamespaceName& nameSpace,
	 const String& queryLanguage,
	 const String& query,
	 ObjectResponseHandler & handler);

      virtual Array<CIMObject> associators(
	 const OperationContext & context,
	 const CIMNamespaceName& nameSpace,
	 const CIMObjectPath& objectName,
	 const String& assocClass,
	 const String& resultClass,
	 const String& role,
	 const String& resultRole,
	 Boolean includeQualifiers,
	 Boolean includeClassOrigin,
	 const CIMPropertyList& propertyList);

      virtual void associatorsAsync(
	 const OperationContext & context,
	 const CIMNamespaceName& nameSpace,
	 const CIMObjectPath& objectName,
	 const String& assocClass,
	 const String& resultClass,
	 const String& role,
	 const String& resultRole,
	 Boolean includeQualifiers,
	 Boolean includeClassOrigin,
	 const CIMPropertyList& propertyList,
	 ObjectResponseHandler & handler);

      virtual Array<CIMObjectPath> associatorNames(
	 const OperationContext & context,
	 const CIMNamespaceName& nameSpace,
	 const CIMObjectPath& objectName,
	 const String& assocClass,
	 const String& resultClass,
	 const String& role,
	 const String& resultRole);

      virtual void associatorNamesAsync(
	 const OperationContext & context,
	 const CIMNamespaceName& nameSpace,
	 const CIMObjectPath& objectName,
	 const String& assocClass,
	 const String& resultClass,
	 const String& role,
	 const String& resultRole,
	 ObjectPathResponseHandler & handler);

      virtual Array<CIMObject> references(
	 const OperationContext & context,
	 const CIMNamespaceName& nameSpace,
	 const CIMObjectPath& objectName,
	 const String& resultClass,
	 const String& role,
	 Boolean includeQualifiers,
	 Boolean includeClassOrigin,
	 const CIMPropertyList& propertyList);

      virtual void referencesAsync(
	 const OperationContext & context,
	 const CIMNamespaceName& nameSpace,
	 const CIMObjectPath& objectName,
	 const String& resultClass,
	 const String& role,
	 Boolean includeQualifiers,
	 Boolean includeClassOrigin,
	 const CIMPropertyList& propertyList,
	 ObjectResponseHandler & handler);

      virtual Array<CIMObjectPath> referenceNames(
	 const OperationContext & context,
	 const CIMNamespaceName& nameSpace,
	 const CIMObjectPath& objectName,
	 const String& resultClass,
	 const String& role);

      virtual void referenceNamesAsync(
	 const OperationContext & context,
	 const CIMNamespaceName& nameSpace,
	 const CIMObjectPath& objectName,
	 const String& resultClass,
	 const String& role,
	 ObjectPathResponseHandler& handler);

      // property operations
      virtual CIMValue getProperty(
	 const OperationContext & context,
	 const CIMNamespaceName& nameSpace,
	 const CIMObjectPath& instanceName,
	 const String& propertyName);

      virtual void getPropertyAsync(
	 const OperationContext & context,
	 const CIMNamespaceName& nameSpace,
	 const CIMObjectPath& instanceName,
	 const String& propertyName,
	 ValueResponseHandler & handler);

      virtual void setProperty(
	 const OperationContext & context,
	 const CIMNamespaceName& nameSpace,
	 const CIMObjectPath& instanceName,
	 const String& propertyName,
	 const CIMValue& newValue);

      virtual void setPropertyAsync(
	 const OperationContext & context,
	 const CIMNamespaceName& nameSpace,
	 const CIMObjectPath& instanceName,
	 const String& propertyName,
	 const CIMValue& newValue,
	 ValueResponseHandler& handler);

      virtual void get_idle_timer(struct timeval *);
      virtual void update_idle_timer(void);
      virtual Uint32 get_operation_timeout(void);
      virtual void set_operation_timeout(Uint32);
      virtual Boolean pending_operation(void);
      virtual Boolean unload_ok(void);

      virtual Uint32 get_output_qid(void);
      virtual void set_output_qid(Uint32);
      virtual Uint32 get_return_qid(void);
      virtual void set_return_qid(Uint32);

//   force provider manager to keep in memory
      virtual void protect(void);
// allow provider manager to unload when idle 
      virtual void unprotect(void);

//     virtual CIMValue invokeMethod(
//     const OperationContext & context,
//     const CIMNamespaceName& nameSpace,
//     const CIMObjectPath& instanceName,
//     const String& methodName,
//     const Array<CIMParamValue>& inParameters,
//     Array<CIMParamValue>& outParameters);

//     virtual void invokeMethodAsync(
//     const OperationContext & context,
//     const CIMNamespaceName& nameSpace,
//     const CIMObjectPath& instanceName,
//     const String& methodName,
//     const Array<CIMParamValue>& inParameters,
//     Array<CIMParamValue>& outParameters,
//     ResponseHandler<CIMValue> & handler);
    

//      static void async_callback(Uint32 user_data, Message *reply, void *parm);


      class _cimom_handle_rep;
      _cimom_handle_rep *_rep;
   private:
      friend class Provider;
      friend class cimom_handle_op_semaphore;
      
};





PEGASUS_NAMESPACE_END

#endif
