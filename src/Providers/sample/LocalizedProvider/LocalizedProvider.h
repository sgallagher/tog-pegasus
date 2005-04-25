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
// Author: Chuck Carmack (carmack@us.ibm.com)
//
// Modified By: Vijay Eli (vijay.eli@ini.ibm.com) for bug #2330
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef LocalizedProvider_h
#define LocalizedProvider_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Provider/CIMInstanceProvider.h>
//#include <Pegasus/Provider/CIMAssociationProvider.h>
#include <Pegasus/Provider/CIMMethodProvider.h>
//#include <Pegasus/Provider/CIMQueryProvider.h>
#include <Pegasus/Provider/CIMIndicationProvider.h>
#include <Pegasus/Provider/CIMIndicationConsumerProvider.h>

#include <Pegasus/Common/MessageLoader.h>
#include <Pegasus/Common/ContentLanguages.h>

class LocalizedProvider :
      public PEGASUS_NAMESPACE(CIMInstanceProvider),
//    public CIMAssociationProvider,
      public PEGASUS_NAMESPACE(CIMMethodProvider),
//    public CIMQueryProvider,
      public PEGASUS_NAMESPACE(CIMIndicationProvider),
      public PEGASUS_NAMESPACE(CIMIndicationConsumerProvider)
{
public:
    LocalizedProvider();
    virtual ~LocalizedProvider(void);

    // CIMProvider interface
    virtual void initialize(PEGASUS_NAMESPACE(CIMOMHandle) & cimom);
    virtual void terminate(void);

    // CIMInstanceProvider interface
    virtual void getInstance(
        const PEGASUS_NAMESPACE(OperationContext) & context,
        const PEGASUS_NAMESPACE(CIMObjectPath) & instanceReference,
        const PEGASUS_NAMESPACE(Boolean) includeQualifiers,
        const PEGASUS_NAMESPACE(Boolean) includeClassOrigin,
        const PEGASUS_NAMESPACE(CIMPropertyList) & propertyList,
        PEGASUS_NAMESPACE(InstanceResponseHandler) & handler);

    virtual void enumerateInstances(
        const PEGASUS_NAMESPACE(OperationContext) & context,
        const PEGASUS_NAMESPACE(CIMObjectPath) & classxReference,
        const PEGASUS_NAMESPACE(Boolean) includeQualifiers,
        const PEGASUS_NAMESPACE(Boolean) includeClassOrigin,
        const PEGASUS_NAMESPACE(CIMPropertyList) & propertyList,
        PEGASUS_NAMESPACE(InstanceResponseHandler) & handler);

    virtual void enumerateInstanceNames(
        const PEGASUS_NAMESPACE(OperationContext) & context,
        const PEGASUS_NAMESPACE(CIMObjectPath) & classReference,
        PEGASUS_NAMESPACE(ObjectPathResponseHandler) & handler);

    virtual void modifyInstance(
        const PEGASUS_NAMESPACE(OperationContext) & context,
        const PEGASUS_NAMESPACE(CIMObjectPath) & instanceReference,
        const PEGASUS_NAMESPACE(CIMInstance) & instanceObject,
        const PEGASUS_NAMESPACE(Boolean) includeQualifiers,
        const PEGASUS_NAMESPACE(CIMPropertyList) & propertyList,
        PEGASUS_NAMESPACE(ResponseHandler) & handler);

    virtual void createInstance(
        const PEGASUS_NAMESPACE(OperationContext) & context,
        const PEGASUS_NAMESPACE(CIMObjectPath) & instanceReference,
        const PEGASUS_NAMESPACE(CIMInstance) & instanceObject,
        PEGASUS_NAMESPACE(ObjectPathResponseHandler) & handler);

    virtual void deleteInstance(
        const PEGASUS_NAMESPACE(OperationContext) & context,
        const PEGASUS_NAMESPACE(CIMObjectPath) & instanceReference,
        PEGASUS_NAMESPACE(ResponseHandler) & handler);

/*
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
*/

    // CIMMethodProviderFacade
    virtual void invokeMethod(
        const PEGASUS_NAMESPACE(OperationContext) & context,
        const PEGASUS_NAMESPACE(CIMObjectPath) & objectReference,
        const PEGASUS_NAMESPACE(CIMName) & methodName,
        const PEGASUS_NAMESPACE(Array)<PEGASUS_NAMESPACE(CIMParamValue)> & inParameters,
        PEGASUS_NAMESPACE(MethodResultResponseHandler) & handler);
   

    // CIMQueryProvider interface
/*    
    virtual void executeQuery(
        const OperationContext & context,
        const CIMNamespaceName & nameSpace,
        const String & queryLanguage,
        const String & query,
        ObjectResponseHandler & handler);
*/        

    // CIMIndicationProvider interface
    virtual void enableIndications(
        PEGASUS_NAMESPACE(IndicationResponseHandler) & handler);

    virtual void disableIndications(void);

    virtual void createSubscription(
        const PEGASUS_NAMESPACE(OperationContext) & context,
        const PEGASUS_NAMESPACE(CIMObjectPath) & subscriptionName,
        const PEGASUS_NAMESPACE(Array)<PEGASUS_NAMESPACE(CIMObjectPath)> & classNames,
        const PEGASUS_NAMESPACE(CIMPropertyList) & propertyList,
        const PEGASUS_NAMESPACE(Uint16) repeatNotificationPolicy);

    virtual void modifySubscription(
        const PEGASUS_NAMESPACE(OperationContext) & context,
        const PEGASUS_NAMESPACE(CIMObjectPath) & subscriptionName,
        const PEGASUS_NAMESPACE(Array)<PEGASUS_NAMESPACE(CIMObjectPath)> & classNames,
        const PEGASUS_NAMESPACE(CIMPropertyList) & propertyList,
        const PEGASUS_NAMESPACE(Uint16) repeatNotificationPolicy);

    virtual void deleteSubscription(
        const PEGASUS_NAMESPACE(OperationContext) & context,
        const PEGASUS_NAMESPACE(CIMObjectPath) & subscriptionName,
        const PEGASUS_NAMESPACE(Array)<PEGASUS_NAMESPACE(CIMObjectPath)> & classNames);

    virtual void consumeIndication(const PEGASUS_NAMESPACE(OperationContext)& context,
				   const PEGASUS_NAMESPACE(String) & url, 
				   const PEGASUS_NAMESPACE(CIMInstance)& indicationInstance);

private:
    PEGASUS_NAMESPACE(Array)<PEGASUS_NAMESPACE(CIMObjectPath)> _instanceNames;
    PEGASUS_NAMESPACE(Array)<PEGASUS_NAMESPACE(CIMInstance)> _instances;
    PEGASUS_NAMESPACE(Array)<PEGASUS_NAMESPACE(ContentLanguages)> _instanceLangs;
	
    PEGASUS_NAMESPACE(MessageLoaderParms) msgParms;
    PEGASUS_NAMESPACE(MessageLoaderParms) notSupportedErrorParms;	
    PEGASUS_NAMESPACE(MessageLoaderParms) contentLangParms;	
    PEGASUS_NAMESPACE(MessageLoaderParms) roundTripErrorParms;
	
    PEGASUS_NAMESPACE(String) roundTripString;
	
    void _checkRoundTripString(const PEGASUS_NAMESPACE(OperationContext) & context,
				   const PEGASUS_NAMESPACE(CIMInstance)& instanceObject);	

    PEGASUS_NAMESPACE(AcceptLanguages) getRequestAcceptLanguages(const PEGASUS_NAMESPACE(OperationContext) &context);

    PEGASUS_NAMESPACE(ContentLanguages) getRequestContentLanguages(const PEGASUS_NAMESPACE(OperationContext) &context);

    PEGASUS_NAMESPACE(CIMObjectPath) buildRefFromInstance(const PEGASUS_NAMESPACE(CIMInstance)&instanceObject);
    	
    PEGASUS_NAMESPACE(ContentLanguages) _loadLocalizedProps(PEGASUS_NAMESPACE(AcceptLanguages) &acceptLangs,
					 PEGASUS_NAMESPACE(ContentLanguages) & contentLangs,
					 PEGASUS_NAMESPACE(CIMInstance) & instance);		
	
    PEGASUS_NAMESPACE(ContentLanguages) _addResourceBundleProp(PEGASUS_NAMESPACE(AcceptLanguages) &acceptLangs,
					    PEGASUS_NAMESPACE(CIMInstance) & instance);
	
    PEGASUS_NAMESPACE(ContentLanguages) _addResourceBundleProp(PEGASUS_NAMESPACE(CIMInstance) &instance);	
	
    void _replaceRBProperty(PEGASUS_NAMESPACE(CIMInstance) & instance,
                           PEGASUS_NAMESPACE(String) newProp);											
    PEGASUS_NAMESPACE(ContentLanguages) _addContentLanguagesProp(PEGASUS_NAMESPACE(CIMInstance) & instance);

    void _setHandlerLanguages(PEGASUS_NAMESPACE(ResponseHandler) & handler, 
                              PEGASUS_NAMESPACE(ContentLanguages) & langs);

    void _testCIMOMHandle();

    void _validateCIMOMHandleResponse(PEGASUS_NAMESPACE(String) expectedLang);

    void _generateIndication();
};

#endif
