//%2003////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002  BMC Software, Hewlett-Packard Development
// Company, L. P., IBM Corp., The Open Group, Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L. P.;
// IBM Corp.; EMC Corporation, The Open Group.
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
#include <Pegasus/Common/OperationContext.h>
#include <Pegasus/Common/CIMObject.h>
#include <Pegasus/Common/CIMObjectPath.h>
#include <Pegasus/Common/CIMClass.h>
#include <Pegasus/Common/CIMInstance.h>
#include <Pegasus/Common/CIMPropertyList.h>
#include <Pegasus/Common/ResponseHandler.h>
#include <Pegasus/Provider/Linkage.h>

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
      CIMOMHandle(const CIMOMHandle* );
      

      /** */
#ifdef PEGASUS_OS_OS400
virtual
#endif
      ~CIMOMHandle(void);

      CIMOMHandle & operator =(const CIMOMHandle & handle);
      
#ifdef PEGASUS_OS_OS400
virtual
#endif
      CIMClass getClass(
	 const OperationContext & context,
	 const CIMNamespaceName& nameSpace,
	 const CIMName& className,
	 Boolean localOnly,
	 Boolean includeQualifiers,
	 Boolean includeClassOrigin,
	 const CIMPropertyList& propertyList);

#ifdef PEGASUS_OS_OS400
virtual
#endif
      Array<CIMClass> enumerateClasses(
	 const OperationContext & context,
	 const CIMNamespaceName& nameSpace,
	 const CIMName& className,
	 Boolean deepInheritance,
	 Boolean localOnly,
	 Boolean includeQualifiers,
	 Boolean includeClassOrigin);

#ifdef PEGASUS_OS_OS400
virtual
#endif
      Array<CIMName> enumerateClassNames(
	 const OperationContext & context,
	 const CIMNamespaceName& nameSpace,
	 const CIMName& className,
	 Boolean deepInheritance);

#ifdef PEGASUS_OS_OS400
virtual
#endif
      void createClass(
	 const OperationContext & context,
	 const CIMNamespaceName& nameSpace,
	 const CIMClass& newClass);

#ifdef PEGASUS_OS_OS400
virtual
#endif
      void modifyClass(
	 const OperationContext & context,
	 const CIMNamespaceName& nameSpace,
	 const CIMClass& modifiedClass);

#ifdef PEGASUS_OS_OS400
virtual
#endif
      void deleteClass(
	 const OperationContext & context,
	 const CIMNamespaceName& nameSpace,
	 const CIMName& className);

#ifdef PEGASUS_OS_OS400
virtual
#endif
      CIMInstance getInstance(
	 const OperationContext & context,
	 const CIMNamespaceName& nameSpace,
	 const CIMObjectPath& instanceName,
	 Boolean localOnly,
	 Boolean includeQualifiers,
	 Boolean includeClassOrigin,
	 const CIMPropertyList& propertyList);

#ifdef PEGASUS_OS_OS400
virtual
#endif
      Array<CIMInstance> enumerateInstances(
	 const OperationContext & context,
	 const CIMNamespaceName& nameSpace,
	 const CIMName& className,
	 Boolean deepInheritance,
	 Boolean localOnly,
	 Boolean includeQualifiers,
	 Boolean includeClassOrigin,
	 const CIMPropertyList& propertyList);

#ifdef PEGASUS_OS_OS400
virtual
#endif
      Array<CIMObjectPath> enumerateInstanceNames(
	 const OperationContext & context,
	 const CIMNamespaceName& nameSpace,
	 const CIMName& className);

#ifdef PEGASUS_OS_OS400
virtual
#endif
      CIMObjectPath createInstance(
	 const OperationContext & context,
	 const CIMNamespaceName& nameSpace,
	 const CIMInstance& newInstance);

#ifdef PEGASUS_OS_OS400
virtual
#endif
      void modifyInstance(
	 const OperationContext & context,
	 const CIMNamespaceName& nameSpace,
	 const CIMInstance& modifiedInstance,
	 Boolean includeQualifiers,
	 const CIMPropertyList& propertyList);

#ifdef PEGASUS_OS_OS400
virtual
#endif
      void deleteInstance(
	 const OperationContext & context,
	 const CIMNamespaceName& nameSpace,
	 const CIMObjectPath& instanceName);

#ifdef PEGASUS_OS_OS400
virtual
#endif
      Array<CIMObject> execQuery(
	 const OperationContext & context,
	 const CIMNamespaceName& nameSpace,
	 const String& queryLanguage,
	 const String& query);

#ifdef PEGASUS_OS_OS400
virtual
#endif
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

#ifdef PEGASUS_OS_OS400
virtual
#endif
      Array<CIMObjectPath> associatorNames(
	 const OperationContext & context,
	 const CIMNamespaceName& nameSpace,
	 const CIMObjectPath& objectName,
	 const CIMName& assocClass,
	 const CIMName& resultClass,
	 const String& role,
	 const String& resultRole);

#ifdef PEGASUS_OS_OS400
virtual
#endif
      Array<CIMObject> references(
	 const OperationContext & context,
	 const CIMNamespaceName& nameSpace,
	 const CIMObjectPath& objectName,
	 const CIMName& resultClass,
	 const String& role,
	 Boolean includeQualifiers,
	 Boolean includeClassOrigin,
	 const CIMPropertyList& propertyList);

#ifdef PEGASUS_OS_OS400
virtual
#endif
      Array<CIMObjectPath> referenceNames(
	 const OperationContext & context,
	 const CIMNamespaceName& nameSpace,
	 const CIMObjectPath& objectName,
	 const CIMName& resultClass,
	 const String& role);

      // property operations
#ifdef PEGASUS_OS_OS400
virtual
#endif
      CIMValue getProperty(
	 const OperationContext & context,
	 const CIMNamespaceName& nameSpace,
	 const CIMObjectPath& instanceName,
	 const CIMName& propertyName);

#ifdef PEGASUS_OS_OS400
virtual
#endif
      void setProperty(
	 const OperationContext & context,
	 const CIMNamespaceName& nameSpace,
	 const CIMObjectPath& instanceName,
	 const CIMName& propertyName,
	 const CIMValue& newValue);

#ifdef PEGASUS_OS_OS400
virtual
#endif
      CIMValue invokeMethod(
	 const OperationContext & context,
	 const CIMNamespaceName& nameSpace,
	 const CIMObjectPath& instanceName,
	 const CIMName& methodName,
	 const Array<CIMParamValue>& inParameters,
	 Array<CIMParamValue>& outParameters);

      /**
          Provides a hint to the CIM Server that the provider calling this
          method prefers not to be unloaded.  This hint applies in
          situations where a provider unload is not necessary, such as
          when the CIM Server unloads idle providers for efficiency reasons.
          A provider may rescind this hint by using the allowProviderUnload
          method.  Note that disallowProviderUnload is cumulative, such that
          each call to disallowProviderUnload must be matched with a call to
          allowProviderUnload.
       */
#ifdef PEGASUS_OS_OS400
virtual
#endif
      void disallowProviderUnload();

      /**
          Provides a hint to the CIM Server that the provider calling this
          method no longer prefers not to be unloaded.  This hint applies in
          situations where a provider unload is not necessary, such as
          when the CIM Server unloads idle providers for efficiency reasons.
          This method is used to rescind a hint that was given using the
          disallowProviderUnload method.  Note that each allowProviderUnload
          call should be preceded by a disallowProviderUnload call.
       */
#ifdef PEGASUS_OS_OS400
virtual
#endif
      void allowProviderUnload();

   private:      
      class _cimom_handle_rep;
      _cimom_handle_rep *_rep;

      friend class Provider;
      friend class cimom_handle_op_semaphore;
      void get_idle_timer(void *);
      void update_idle_timer(void);
      Boolean pending_operation(void);
      Boolean unload_ok(void);
};

PEGASUS_NAMESPACE_END

#endif
