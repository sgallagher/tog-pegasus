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
// Author: Chuck Carmack (carmack@us.ibm.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef LocalizedProvider_h
#define LocalizedProvider_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Provider/CIMInstanceProvider.h>
//#include <Pegasus/Provider/CIMAssociationProvider.h>
//#include <Pegasus/Provider/CIMPropertyProvider.h>
#include <Pegasus/Provider/CIMMethodProvider.h>
//#include <Pegasus/Provider/CIMQueryProvider.h>
//#include <Pegasus/Provider/CIMIndicationProvider.h>
//#include <Pegasus/Provider/CIMIndicationConsumer.h>

#include <Pegasus/Common/MessageLoader.h>
#include <Pegasus/Common/ContentLanguages.h>

PEGASUS_NAMESPACE_BEGIN

class LocalizedProvider :
    public CIMInstanceProvider,
//    public CIMAssociationProvider,
//    public CIMPropertyProvider,
      public CIMMethodProvider
//    public CIMQueryProvider,
//    public CIMIndicationProvider,
//    public CIMIndicationConsumer
{
public:
    LocalizedProvider();
    virtual ~LocalizedProvider(void);

    // CIMProvider interface
    virtual void initialize(CIMOMHandle & cimom);
    virtual void terminate(void);

    // CIMInstanceProvider interface
    virtual void getInstance(
        const OperationContext & context,
        const CIMObjectPath & instanceReference,
        const Boolean includeQualifiers,
        const Boolean includeClassOrigin,
        const CIMPropertyList & propertyList,
        InstanceResponseHandler & handler);

    virtual void enumerateInstances(
        const OperationContext & context,
        const CIMObjectPath & classxReference,
        const Boolean includeQualifiers,
        const Boolean includeClassOrigin,
        const CIMPropertyList & propertyList,
        InstanceResponseHandler & handler);

    virtual void enumerateInstanceNames(
        const OperationContext & context,
        const CIMObjectPath & classReference,
        ObjectPathResponseHandler & handler);

    virtual void modifyInstance(
        const OperationContext & context,
        const CIMObjectPath & instanceReference,
        const CIMInstance & instanceObject,
        const Boolean includeQualifiers,
        const CIMPropertyList & propertyList,
        ResponseHandler & handler);

    virtual void createInstance(
        const OperationContext & context,
        const CIMObjectPath & instanceReference,
        const CIMInstance & instanceObject,
        ObjectPathResponseHandler & handler);

    virtual void deleteInstance(
        const OperationContext & context,
        const CIMObjectPath & instanceReference,
        ResponseHandler & handler);

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

    // CIMPropertyProvider interface
    virtual void getProperty(
        const OperationContext & context,
        const CIMObjectPath & instanceReference,
        const CIMName & propertyName,
        ValueResponseHandler & handler);

    virtual void setProperty(
        const OperationContext & context,
        const CIMObjectPath & instanceReference,
        const CIMName & propertyName,
        const CIMValue & newValue,
        ResponseHandler & handler);
*/

    // CIMMethodProviderFacade
    virtual void invokeMethod(
        const OperationContext & context,
        const CIMObjectPath & objectReference,
        const CIMName & methodName,
        const Array<CIMParamValue> & inParameters,
        MethodResultResponseHandler & handler);
   

    // CIMQueryProvider interface
/*    
    virtual void executeQuery(
        const OperationContext & context,
        const CIMNamespaceName & nameSpace,
        const String & queryLanguage,
        const String & query,
        ObjectResponseHandler & handler);
*/        
/*
    // CIMIndicationProvider interface
    virtual void enableIndications(
        IndicationResponseHandler & handler);

    virtual void disableIndications(void);

    virtual void createSubscription(
        const OperationContext & context,
        const CIMObjectPath & subscriptionName,
        const Array<CIMObjectPath> & classNames,
        const CIMPropertyList & propertyList,
        const Uint16 repeatNotificationPolicy);

    virtual void modifySubscription(
        const OperationContext & context,
        const CIMObjectPath & subscriptionName,
        const Array<CIMObjectPath> & classNames,
        const CIMPropertyList & propertyList,
        const Uint16 repeatNotificationPolicy);

    virtual void deleteSubscription(
        const OperationContext & context,
        const CIMObjectPath & subscriptionName,
        const Array<CIMObjectPath> & classNames);
*/        

/*
    // CIMIndicationConsumer interface
    virtual void handleIndication(
        const OperationContext & context,
        const CIMInstance & indication,
        IndicationResponseHandler & handler);
*/        

private:
	Array<CIMObjectPath> _instanceNames;
	Array<CIMInstance> _instances;
	Array<ContentLanguages> _instanceLangs;
	
	MessageLoaderParms msgParms;
	MessageLoaderParms notSupportedErrorParms;	
	MessageLoaderParms contentLangParms;	
	MessageLoaderParms roundTripErrorParms;
	
	String roundTripString;
	
	void _checkRoundTripString(const OperationContext & context,
								const CIMInstance& instanceObject);	

    AcceptLanguages getRequestAcceptLanguages(const OperationContext & context);

    ContentLanguages getRequestContentLanguages(const OperationContext & context);

	CIMObjectPath buildRefFromInstance(const CIMInstance& instanceObject);
    	
	ContentLanguages _loadLocalizedProps(AcceptLanguages & acceptLangs,
										ContentLanguages & contentLangs,
										CIMInstance & instance);		
	
	ContentLanguages _addResourceBundleProp(AcceptLanguages & acceptLangs,
										CIMInstance & instance);
	
	ContentLanguages _addResourceBundleProp(CIMInstance & instance);	
	
	void _replaceRBProperty(CIMInstance & instance, String newProp);	
										
	ContentLanguages _addContentLanguagesProp(CIMInstance & instance);

        void _setHandlerLanguages(ResponseHandler & handler, ContentLanguages & langs);
};

PEGASUS_NAMESPACE_END

#endif
