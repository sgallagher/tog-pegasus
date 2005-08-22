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
// Author: Karl Schopmeyer (k.schopmeyer@opengrooup.org)
//
// Modified By:
//
//%////////////////////////////////////////////////////////////////////////////


#ifndef InteropProvider_h
#define InteropProvider_h

///////////////////////////////////////////////////////////////////////////////
//  Interop Provider
///////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/ControlProviders/InteropProvider/Linkage.h>

#include <Pegasus/Common/String.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/ArrayInternal.h>
#include <Pegasus/Common/CIMType.h>
#include <Pegasus/Common/CIMInstance.h>
#include <Pegasus/Common/CIMObjectPath.h>
#include <Pegasus/Common/InternalException.h>
#include <Pegasus/Common/CIMStatusCode.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Config/ConfigManager.h>

#include <Pegasus/Repository/CIMRepository.h>
#include <Pegasus/Provider/CIMInstanceProvider.h>
#include <Pegasus/Provider/CIMAssociationProvider.h>

PEGASUS_NAMESPACE_BEGIN

/**
    The InteropProvider provides information and manipulates the
    following classes from the DMTF CIM_Interop schema:
    CIM_Namespace - Creation and deletion of namespaces
    Note: Effective Pegasus 2.4, it processes PG_Namespace
    __Namespace - Creation and deletion of namespaces (deprecated)
    CIM_ObjectManager - Reports on the status of the object manager
    CIM_ObjectManagerCommunicationMechanism - Reports on CIMOM communications
    COM_CIMXMLCommunicationMechanism - Reports on CIMXML communications
    CIM_ProtocolAdapter
    
    Associations
    NamespaceinManager
    
    creation and deletion of namespaces using the __namespace class
    Note however, that the DMTF specification model does not define a class for
    __namespace so that it is, a "false" class.  We assume that it has the
    same characteristics as the CIM_namespace class defined in CIM 2.6.

    This provider implements the following functions:
    - createInstance		( adds a new namespace to the repository)
    - getInstance		( Gets one instance of any supported object)
    - modifyInstance		( Limited Support - selected fields in CIM_Namespace)
    - enumerateInstances	( Lists all namespaces of all supported classes)
    - enumerateInstanceNames	( Lists all namespace names of all supported classes )
    - reference and associations 
    TBD
*/

class PEGASUS_INTEROPPROVIDER_LINKAGE InteropProvider :
	public CIMInstanceProvider, public CIMAssociationProvider
{
public:

    InteropProvider(CIMRepository* repository);
    virtual ~InteropProvider()
    {
	PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,"InteropProvider::~InteropProvider");

	PEG_METHOD_EXIT();
    }

    virtual void createInstance(
    	const OperationContext & context,
    	const CIMObjectPath & instanceReference,
        const CIMInstance& myInstance,
    	ObjectPathResponseHandler & handler);

    virtual void deleteInstance(
    	const OperationContext & context,
        const CIMObjectPath& instanceName,
    	ResponseHandler & handler);

    virtual void getInstance(
    	const OperationContext & context,
        const CIMObjectPath& instanceName,
    	const Boolean includeQualifiers,
    	const Boolean includeClassOrigin,
        const CIMPropertyList& propertyList,
    	InstanceResponseHandler & handler);

    void modifyInstance(
    	const OperationContext & context,
    	const CIMObjectPath & instanceReference,
        const CIMInstance& modifiedIns,
    	const Boolean includeQualifiers,
        const CIMPropertyList& propertyList,
	ResponseHandler & handler);

    virtual void enumerateInstances(
    	const OperationContext & context,
    	const CIMObjectPath & ref,
    	const Boolean includeQualifiers,
    	const Boolean includeClassOrigin,
        const CIMPropertyList& propertyList,
    	InstanceResponseHandler & handler);

    virtual void enumerateInstanceNames(
	const OperationContext & context,
	const CIMObjectPath & classReference,
        ObjectPathResponseHandler & handler);

    // Association Interfaces
   
	// CIMAssociationProvider interface
	virtual void associators(
		const OperationContext & context,
		const CIMObjectPath & objectName,
		const CIMName & associationClass,
		const CIMName & resultClass,
		const String & role,
		const String & resultRole,
		const Boolean includeQualifiers,
		const Boolean includeClassOrigin,
		const CIMPropertyList & propertyList,
		ObjectResponseHandler & handler);

	virtual void associatorNames(
		const OperationContext & context,
		const CIMObjectPath & objectName,
		const CIMName & associationClass,
		const CIMName & resultClass,
		const String & role,
		const String & resultRole,
		ObjectPathResponseHandler & handler);

	virtual void references(
		const OperationContext & context,
		const CIMObjectPath & objectName,
		const CIMName & resultClass,
		const String & role,
		const Boolean includeQualifiers,
		const Boolean includeClassOrigin,
		const CIMPropertyList & propertyList,
		ObjectResponseHandler & handler);

	virtual void referenceNames(
		const OperationContext & context,
		const CIMObjectPath & objectName,
		const CIMName & resultClass,
		const String & role,
		ObjectPathResponseHandler & handler);
    
    /**
    Standard initialization function for the provider.
    */
    void initialize(CIMOMHandle& cimomHandle)
    {
    }

    void terminate(void)
    {
	// delete self. this is necessary because the entry point for this object allocated it, and
    	// the module is responsible for its memory management.
	delete this;
    }

private:

    CIMInstance _buildInstanceSkeleton(
        const CIMObjectPath & objectPath,
        const CIMName& className,
        CIMClass& returnedClass);

    CIMObjectPath _buildObjectPath(
        const CIMObjectPath& objectPath,
        const CIMName& className, 
        const CIMInstance& instance);
    CIMObjectPath _buildInstancePath(
        const CIMObjectPath& objectPath,
        const CIMName& className, 
        const CIMInstance& instance);
    
    CIMClass _getClass(const CIMObjectPath& objectPath,
        const CIMName& className);

    Array<CIMNamespaceName> _enumerateNameSpaces();
    
    CIMInstance _buildInstancePGCIMXMLCommunicationMechanism(
        const CIMObjectPath& objectPath,
        const String& namespaceType,
        const Uint16& accessProtocol,
        const String& IPAddress);

    Array<CIMInstance> _buildInstancesPGCIMXMLCommunicationMechanism(
        const CIMObjectPath& objectPath);

    CIMInstance _getInstanceCIMObjectManager(
        const CIMObjectPath& objectPath,
        const Boolean includeQualifiers,
        const Boolean includeClassOrigin,
        const CIMPropertyList& propertyList);

    String _getObjectManagerName();

    Array<CIMInstance> _getInstancesCIMNamespace(
        const CIMObjectPath& objectPath);

    CIMInstance _getInstanceCIMNamespace(
        const CIMObjectPath& objectPath);
    //
    CIMInstance _buildInstancePGNamespace(
        const CIMObjectPath& objectPath,
                CIMNamespaceName& nameSpace);

    Array<CIMInstance> _buildInstancesNamespaceInManager(
        const CIMObjectPath& objectPath);

    Array<CIMInstance> _buildInstancesCommMechanismForManager(
        const CIMObjectPath& objectPath);

    Boolean _getInstanceFromRepositoryCIMObjectManager(
        const CIMObjectPath& objectPath,
        CIMInstance& rtnInstance,
        const Boolean includeQualifiers,
        const Boolean includeClassOrigin,
        const CIMPropertyList& propertyList);

    Boolean isModifyAllowed(const OperationContext & context,
        const CIMObjectPath & instanceReference,
        const CIMInstance& modifiedIns,
        const Boolean includeQualifiers,
        const CIMPropertyList& propertyList,
        const CIMPropertyList& allowedModifyProperties);

    void modifyObjectManagerInstance(const OperationContext & context,
        const CIMObjectPath & instanceReference,
        const CIMInstance& modifiedIns,
        const Boolean includeQualifiers,
        const CIMPropertyList& propertyList);

    // The following are internal equivalents of the operations
    // allowing the operations to call one another internally within
    // the provider.
    Array<CIMInstance> localEnumerateInstances(
        const OperationContext & context,
        const CIMObjectPath & ref,
        const Boolean includeQualifiers,
        const Boolean includeClassOrigin,
        const CIMPropertyList& propertyList);

    Array<CIMObject> localReferences(
    	const OperationContext & context,
    	const CIMObjectPath & objectName,
    	const CIMName & resultClass,
    	const String & role,
    	const Boolean includeQualifiers,
    	const Boolean includeClassOrigin,
    	const CIMPropertyList & propertyList);

    CIMInstance localGetInstance(
        const OperationContext & context,
        const CIMObjectPath & instanceName,
        const Boolean includeQualifiers,
        const Boolean includeClassOrigin,
        const CIMPropertyList & propertyList);

        // Repository Instance variable
       CIMRepository*   _repository;
       // local save for name of object manager
       String _objectManagerName;
};

PEGASUS_NAMESPACE_END

#endif // InteropProvider_h
