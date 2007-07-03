//%2006////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
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
//%/////////////////////////////////////////////////////////////////////////////

#include "CIMClientRep.h"

#include <Pegasus/Common/MessageLoader.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/LanguageParser.h>
#include <Pegasus/Common/Stopwatch.h>

#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sys/types.h>
#include <Pegasus/Common/Network.h>

#ifdef PEGASUS_USE_DIRECTACCESS_FOR_LOCAL_DEPEND
#endif

int numdacim=0, numnondacim=0,numconnect=0,numredirok=0,numredirnok=0;
PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

static Stopwatch temptimer;  // ebbfix



// _directaccess_redirect flag should only be set via this macro
#if PEGASUS_DIRECTACCESS_BUILDTYPE == dacimINTEGRATED
#define ck_dacim_redirect() \
    if (_directaccesslocalproviders) { \
        disconnect();\
        _directaccess_redirect = true; \
        try { connectLocal();\
++numredirok; \
        } \
		catch( Exception& e ) { \
++numredirnok; \
            _directaccess_redirect = false; \
            connectLocal(); \
            } \
        }
#else
#define ck_dacim_redirect()  ;
#endif
///////////////////////////////////////////////////////////////////////////////
//
// CIMClientRep
//
///////////////////////////////////////////////////////////////////////////////

CIMClientRep::CIMClientRep(Uint32 timeoutMilliseconds)
    :
    MessageQueue(PEGASUS_QUEUENAME_CLIENT),
    _timeoutMilliseconds(timeoutMilliseconds),
    _connected(false),
    _doReconnect(false),
#ifdef PEGASUS_USE_DIRECTACCESS_FOR_LOCAL_DEPEND
    _allowdirectaccesslocalproviders(true),
    _directaccess_redirect(false),	
    _localizer(  //new CIMDirectAccessRep() ) 
                 CIMDirectAccessRep::get() )
#else
    _allowdirectaccesslocalproviders(false),
    _directaccess_redirect(false),	
    _localizer(NULL)
#endif
{
#ifdef PEGASUS_USE_DIRECTACCESS_FOR_LOCAL_DEPEND
#endif
temptimer.start();    

gid_t groupid = getegid();
    //
    // Create Monitor and HTTPConnector
    //
    _monitor.reset(new Monitor());
    _httpConnector.reset(new HTTPConnector(_monitor.get()));

    requestAcceptLanguages.clear();
    requestContentLanguages.clear();
}

//----------------------------------------------------------
CIMClientRep::~CIMClientRep()
{
   disconnect();
#ifdef PEGASUS_USE_DIRECTACCESS_FOR_LOCAL_DEPEND
   _localizer->release();
   //delete _localizer;
#endif
temptimer.stop();   
}

void CIMClientRep::handleEnqueue()
{
}

Uint32 _getShowType(String& s)
{
    String log = "log";
    String con = "con";
    String both = "both";
    if (s == log)
        return 2;
    if (s == con)
        return 1;
    if (s == both)
        return 3;
    return 0;
}

//---------------------------------------------
void CIMClientRep::_connect()
{
    //
    // Test for Display optons of the form
    // Use Env variable PEGASUS_CLIENT_TRACE= <intrace> : <outtrace
    // intrace = "con" | "log" | "both"
    // outtrace = intrace
    // ex set PEGASUS_CLIENT_TRACE=BOTH:BOTH traces input and output
    // to console and log
    // Keywords are case insensitive.
    // PEP 90
    //
++numconnect;
    Uint32 showOutput = 0;
    Uint32 showInput = 0;
#ifdef PEGASUS_CLIENT_TRACE_ENABLE
    String input;
    if (char * envVar = getenv("PEGASUS_CLIENT_TRACE"))
    {
        input = envVar;
        input.toLower();
        String io;
        Uint32 pos = input.find(':');
        if (pos == PEG_NOT_FOUND)
            pos = 0;
        else
            io = input.subString(0,pos);

        // some compilers do not allow temporaries to be passed to a
        // reference argument - so break into 2 lines
        String out = input.subString(pos + 1);
        showOutput = _getShowType(out);

        showInput = _getShowType(io);
    }
#endif
    char *ckdacim = getenv("PEGASUS_USE_DIRECTACCESS_FOR_LOCAL_RT");
    if (ckdacim) 
    {
        _allowdirectaccesslocalproviders = (strcmp(ckdacim,"false") != 0);
    }
    else 
    {
        //If the runtime environment is not set, use cimserver
        _allowdirectaccesslocalproviders = 0; 
    }
    if (_directaccesslocalproviders =     // (assign is intentional)
             _allowdirectaccesslocalproviders 
			 && !_directaccess_redirect && _isLocalHost() ) {
        _connected = true;
        _connectHost = "localhost";
        }

    else { // do the usual connection functions ...


        _allowdirectaccesslocalproviders = true; // reset each connect

    //
    // Create response decoder:
    //
    AutoPtr<CIMOperationResponseDecoder> responseDecoder(
        new CIMOperationResponseDecoder(
            this, _requestEncoder.get(), &_authenticator, showInput));

    //
    // Attempt to establish a connection:
    //
    AutoPtr<HTTPConnection> httpConnection(_httpConnector->connect(
        _connectHost,
        _connectPortNumber,
        _connectSSLContext.get(),
        responseDecoder.get()));

    //
    // Create request encoder:
    //
    String connectHost = _connectHost;
    if (connectHost.size())
    {
        char portStr[32];
        sprintf(portStr, ":%u", _connectPortNumber);
        connectHost.append(portStr);
    }

    AutoPtr<CIMOperationRequestEncoder> requestEncoder(
        new CIMOperationRequestEncoder(
            httpConnection.get(), connectHost, &_authenticator, showOutput));

    _responseDecoder.reset(responseDecoder.release());
    _httpConnection = httpConnection.release();
    _requestEncoder.reset(requestEncoder.release());
    _responseDecoder->setEncoderQueue(_requestEncoder.get());

    //pass encoder and decoder a pointer to CIMClientRep::perfDataStore
    _requestEncoder->setDataStorePointer(&perfDataStore);
    _responseDecoder->setDataStorePointer(&perfDataStore);

    _doReconnect = false;
    _connected = true;
    _httpConnection->setSocketWriteTimeout(_timeoutMilliseconds/1000+1);
    }
}

//------------------------------------------
void CIMClientRep::_disconnect()
{
    if (_connected)
    {
        if (!_directaccesslocalproviders) {
        //
        // destroy response decoder
        //
        _responseDecoder.reset();


        //
        // Close the connection
        //
        if (_httpConnector.get())
        {
            _httpConnector->disconnect(_httpConnection);
            _httpConnection = 0;
        }

        //
        // destroy request encoder
        //
        _requestEncoder.reset();
        }

        _connected = false;
#ifdef PEGASUS_USE_DIRECTACCESS_FOR_LOCAL_DEPEND
        _allowdirectaccesslocalproviders = true;
#else
        _allowdirectaccesslocalproviders = false;
#endif
    }

    // Reconnect no longer applies
    _doReconnect = false;

    // Let go of the cached request message if we have one
    _authenticator.setRequestMessage(0);
}

//-----------------------------------------------
Boolean CIMClientRep::_isLocalHost() 
{
    if ( _connectHost == String::EMPTY  || 
         String::equalNoCase(_connectHost,"localhost") ) return true;
    return System::sameHost( _connectHost );
}
//-------------------------------------------
void CIMClientRep::connect(
    const String& host,
    const Uint32 portNumber,
    const String& userName,
    const String& password)
{
    //
    // If already connected, bail out!
    //
    if (_connected)
        throw AlreadyConnectedException();

    //
    // If the host is empty, set hostName to "localhost"
    //
    String hostName = host;
    if (host == String::EMPTY)
    {
        hostName = "localhost";
    }

    //
    // Set authentication information
    //
    _authenticator.clear();

    if (userName.size())
    {
        _authenticator.setUserName(userName);
    }

    if (password.size())
    {
        _authenticator.setPassword(password);
    }

    _connectSSLContext.reset();
    _connectHost = hostName;
    _connectPortNumber = portNumber;

    _connect();
}


//------------------------------------------
void CIMClientRep::connect(
    const String& host,
    const Uint32 portNumber,
    const SSLContext& sslContext,
    const String& userName,
    const String& password)
{
    //
    // If already connected, bail out!
    //
    if (_connected)
        throw AlreadyConnectedException();

    //
    // If the host is empty, set hostName to "localhost"
    //
    String hostName = host;
    if (host == String::EMPTY)
    {
        hostName = "localhost";
    }

    //
    // Set authentication information
    //
    _authenticator.clear();

    if (userName.size())
    {
        _authenticator.setUserName(userName);
    }

    if (password.size())
    {
        _authenticator.setPassword(password);
    }

    _connectHost = hostName;
    _connectPortNumber = portNumber;

    _connectSSLContext.reset(new SSLContext(sslContext));
    _connect();
}


//----------------------------------------------
void CIMClientRep::connectLocal()
{
    //
    // If already connected, bail out!
    //
    if (_connected)
        throw AlreadyConnectedException();
    if (_allowdirectaccesslocalproviders && !_directaccess_redirect) {
		// set up direct access CIM	
        _connectHost = String::EMPTY;
        _connect();
        return;                    // <--- note
        }

    //
    // Set authentication type
    //
    _authenticator.clear();
    _authenticator.setAuthType(ClientAuthenticator::LOCAL);

#ifndef PEGASUS_DISABLE_LOCAL_DOMAIN_SOCKET
    _connectSSLContext.reset();
    _connectHost = String::EMPTY;
    _connectPortNumber = 0;
    _connect();
#else

    try
    {
        //
        // Look up the WBEM HTTP port number for the local system
        //
        _connectPortNumber = System::lookupPort (WBEM_HTTP_SERVICE_NAME,
            WBEM_DEFAULT_HTTP_PORT);

        //
        //  Assign host
        //
        _connectHost.assign(System::getHostName());

        _connectSSLContext.reset();

        _connect();
    }
    catch (const CannotConnectException &)
    {
        //
        // Look up the WBEM HTTPS port number for the local system
        //
        _connectPortNumber = System::lookupPort (WBEM_HTTPS_SERVICE_NAME,
            WBEM_DEFAULT_HTTPS_PORT);

        //
        //  Assign host
        //
        _connectHost.assign(System::getHostName());

        //
        // Create SSLContext
        //
        const char* pegasusHome = getenv("PEGASUS_HOME");

        String randFile;

#ifdef PEGASUS_SSL_RANDOMFILE
        randFile = FileSystem::getAbsolutePath(
            pegasusHome, PEGASUS_SSLCLIENT_RANDOMFILE);
#endif

        try
        {
            _connectSSLContext.reset(
                new SSLContext(String::EMPTY, NULL, randFile));
        }
        catch (const SSLException &)
        {
            throw;
        }

        _connect();
    }
#endif
}


void CIMClientRep::disconnect()
{
    _disconnect();
    _authenticator.clear();
    _connectSSLContext.reset();
}

Boolean CIMClientRep::isConnected() const throw()
{
    return _connected;
}

AcceptLanguageList CIMClientRep::getRequestAcceptLanguages() const
{
    return requestAcceptLanguages;
}

ContentLanguageList CIMClientRep::getRequestContentLanguages() const
{
    return requestContentLanguages;
}

ContentLanguageList CIMClientRep::getResponseContentLanguages() const
{
    return responseContentLanguages;
}

void CIMClientRep::setRequestAcceptLanguages(const AcceptLanguageList& langs)
{
    requestAcceptLanguages = langs;
}

void CIMClientRep::setRequestContentLanguages(const ContentLanguageList& langs)
{
    requestContentLanguages = langs;
}

void CIMClientRep::setRequestDefaultLanguages()
{
    requestAcceptLanguages = LanguageParser::getDefaultAcceptLanguages();
}

CIMClass CIMClientRep::getClass(
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{
    AutoPtr<CIMRequestMessage> request(new CIMGetClassRequestMessage(
        String::EMPTY,
        nameSpace,
        className,
        localOnly,
        includeQualifiers,
        includeClassOrigin,
        propertyList,
        QueueIdStack()));

    Message* message = _doRequest(request, CIM_GET_CLASS_RESPONSE_MESSAGE);

    CIMGetClassResponseMessage* response =
        (CIMGetClassResponseMessage*)message;

    AutoPtr<CIMGetClassResponseMessage> destroyer(response);

    return response->cimClass;
}

CIMInstance CIMClientRep::getInstance(
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& instanceName,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{
    AutoPtr<CIMRequestMessage> request(new CIMGetInstanceRequestMessage(
        String::EMPTY,
        nameSpace,
        instanceName,
        localOnly,
        includeQualifiers,
        includeClassOrigin,
        propertyList,
        QueueIdStack()));

    Message* message = _doRequest(request, CIM_GET_INSTANCE_RESPONSE_MESSAGE);

    CIMGetInstanceResponseMessage* response =
        (CIMGetInstanceResponseMessage*)message;

    AutoPtr<CIMGetInstanceResponseMessage> destroyer(response);

    return response->cimInstance;
}

void CIMClientRep::deleteClass(
    const CIMNamespaceName& nameSpace,
    const CIMName& className)
{
    AutoPtr<CIMRequestMessage> request(new CIMDeleteClassRequestMessage(
        String::EMPTY,
        nameSpace,
        className,
        QueueIdStack()));
	ck_dacim_redirect()
    Message* message = _doRequest(request, CIM_DELETE_CLASS_RESPONSE_MESSAGE);
    _directaccess_redirect = false;
    CIMDeleteClassResponseMessage* response =
        (CIMDeleteClassResponseMessage*)message;

    AutoPtr<CIMDeleteClassResponseMessage> destroyer(response);
}

void CIMClientRep::deleteInstance(
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& instanceName)
{
    AutoPtr<CIMRequestMessage> request(new CIMDeleteInstanceRequestMessage(
        String::EMPTY,
        nameSpace,
        instanceName,
        QueueIdStack()));
	if (nameSpace == "root/PG_InterOp") { ck_dacim_redirect() }
    Message* message =
        _doRequest(request, CIM_DELETE_INSTANCE_RESPONSE_MESSAGE);
    _directaccess_redirect = false;
    CIMDeleteInstanceResponseMessage* response =
        (CIMDeleteInstanceResponseMessage*)message;

    AutoPtr<CIMDeleteInstanceResponseMessage> destroyer(response);
}

void CIMClientRep::createClass(
    const CIMNamespaceName& nameSpace,
    const CIMClass& newClass)
{
    AutoPtr<CIMRequestMessage> request(new CIMCreateClassRequestMessage(
        String::EMPTY,
        nameSpace,
        newClass,
        QueueIdStack()));

    ck_dacim_redirect()
    Message* message = _doRequest(request, CIM_CREATE_CLASS_RESPONSE_MESSAGE);
    _directaccess_redirect = false;
    CIMCreateClassResponseMessage* response =
        (CIMCreateClassResponseMessage*)message;

    AutoPtr<CIMCreateClassResponseMessage> destroyer(response);
}

CIMObjectPath CIMClientRep::createInstance(
    const CIMNamespaceName& nameSpace,
    const CIMInstance& newInstance)
{
    AutoPtr<CIMRequestMessage> request(new CIMCreateInstanceRequestMessage(
        String::EMPTY,
        nameSpace,
        newInstance,
        QueueIdStack()));

	if (nameSpace == "root/PG_InterOp") { ck_dacim_redirect() }
    Message* message =
        _doRequest(request, CIM_CREATE_INSTANCE_RESPONSE_MESSAGE);
    _directaccess_redirect = false;

    CIMCreateInstanceResponseMessage* response =
        (CIMCreateInstanceResponseMessage*)message;

    AutoPtr<CIMCreateInstanceResponseMessage> destroyer(response);

    return response->instanceName;
}

void CIMClientRep::modifyClass(
    const CIMNamespaceName& nameSpace,
    const CIMClass& modifiedClass)
{
    AutoPtr<CIMRequestMessage> request(new CIMModifyClassRequestMessage(
        String::EMPTY,
        nameSpace,
        modifiedClass,
        QueueIdStack()));

	ck_dacim_redirect()
    Message* message = _doRequest(request, CIM_MODIFY_CLASS_RESPONSE_MESSAGE);
    _directaccess_redirect = false;
    CIMModifyClassResponseMessage* response =
        (CIMModifyClassResponseMessage*)message;

    AutoPtr<CIMModifyClassResponseMessage> destroyer(response);
}

void CIMClientRep::modifyInstance(
    const CIMNamespaceName& nameSpace,
    const CIMInstance& modifiedInstance,
    Boolean includeQualifiers,
    const CIMPropertyList& propertyList)
{
    AutoPtr<CIMRequestMessage> request(new CIMModifyInstanceRequestMessage(
        String::EMPTY,
        nameSpace,
        modifiedInstance,
        includeQualifiers,
        propertyList,
        QueueIdStack()));

	if (nameSpace == "root/PG_InterOp") { ck_dacim_redirect() }
    Message* message =
        _doRequest(request, CIM_MODIFY_INSTANCE_RESPONSE_MESSAGE);
    _directaccess_redirect = false;
    CIMModifyInstanceResponseMessage* response =
        (CIMModifyInstanceResponseMessage*)message;

    AutoPtr<CIMModifyInstanceResponseMessage> destroyer(response);
}

Array<CIMClass> CIMClientRep::enumerateClasses(
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    Boolean deepInheritance,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin)
{
    AutoPtr<CIMRequestMessage> request(new CIMEnumerateClassesRequestMessage(
        String::EMPTY,
        nameSpace,
        className,
        deepInheritance,
        localOnly,
        includeQualifiers,
        includeClassOrigin,
        QueueIdStack()));

    Message* message =
        _doRequest(request, CIM_ENUMERATE_CLASSES_RESPONSE_MESSAGE);

    CIMEnumerateClassesResponseMessage* response =
        (CIMEnumerateClassesResponseMessage*)message;

    AutoPtr<CIMEnumerateClassesResponseMessage> destroyer(response);

    return response->cimClasses;
}

Array<CIMName> CIMClientRep::enumerateClassNames(
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    Boolean deepInheritance)
{
    AutoPtr<CIMRequestMessage> request(new CIMEnumerateClassNamesRequestMessage(
        String::EMPTY,
        nameSpace,
        className,
        deepInheritance,
        QueueIdStack()));

    Message* message =
        _doRequest(request, CIM_ENUMERATE_CLASS_NAMES_RESPONSE_MESSAGE);

    CIMEnumerateClassNamesResponseMessage* response =
        (CIMEnumerateClassNamesResponseMessage*)message;

    AutoPtr<CIMEnumerateClassNamesResponseMessage> destroyer(response);

    // Temporary code until internal structures use CIMName instead of String
    Array<CIMName> classNameArray;
    classNameArray.reserveCapacity(response->classNames.size());
    for (Uint32 i=0; i<response->classNames.size(); i++)
    {
        classNameArray.append(response->classNames[i]);
    }
    return classNameArray;
}

Array<CIMInstance> CIMClientRep::enumerateInstances(
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    Boolean deepInheritance,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{
    AutoPtr<CIMRequestMessage> request(new CIMEnumerateInstancesRequestMessage(
        String::EMPTY,
        nameSpace,
        className,
        deepInheritance,
        localOnly,
        includeQualifiers,
        includeClassOrigin,
        propertyList,
        QueueIdStack()));

    Message* message =
        _doRequest(request, CIM_ENUMERATE_INSTANCES_RESPONSE_MESSAGE);

    CIMEnumerateInstancesResponseMessage* response =
        (CIMEnumerateInstancesResponseMessage*)message;

    AutoPtr<CIMEnumerateInstancesResponseMessage> destroyer(response);

    return response->cimNamedInstances;
}

Array<CIMObjectPath> CIMClientRep::enumerateInstanceNames(
    const CIMNamespaceName& nameSpace,
    const CIMName& className)
{
#ifdef PEGASUS_USE_DIRECTACCESS_FOR_LOCAL_DEPEND
#endif
    AutoPtr<CIMRequestMessage> request(
        new CIMEnumerateInstanceNamesRequestMessage(
            String::EMPTY,
            nameSpace,
            className,
            QueueIdStack()));

    Message* message =
        _doRequest(request, CIM_ENUMERATE_INSTANCE_NAMES_RESPONSE_MESSAGE);

    CIMEnumerateInstanceNamesResponseMessage* response =
        (CIMEnumerateInstanceNamesResponseMessage*)message;

    AutoPtr<CIMEnumerateInstanceNamesResponseMessage> destroyer(response);

    return response->instanceNames;
}

Array<CIMObject> CIMClientRep::execQuery(
    const CIMNamespaceName& nameSpace,
    const String& queryLanguage,
    const String& query)
{
    AutoPtr<CIMRequestMessage> request(new CIMExecQueryRequestMessage(
        String::EMPTY,
        nameSpace,
        queryLanguage,
        query,
        QueueIdStack()));

    Message* message = _doRequest(request, CIM_EXEC_QUERY_RESPONSE_MESSAGE);

    CIMExecQueryResponseMessage* response =
        (CIMExecQueryResponseMessage*)message;

    AutoPtr<CIMExecQueryResponseMessage> destroyer(response);

    return response->cimObjects;
}

Array<CIMObject> CIMClientRep::associators(
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& objectName,
    const CIMName& assocClass,
    const CIMName& resultClass,
    const String& role,
    const String& resultRole,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{
    AutoPtr<CIMRequestMessage> request(new CIMAssociatorsRequestMessage(
        String::EMPTY,
        nameSpace,
        objectName,
        assocClass,
        resultClass,
        role,
        resultRole,
        includeQualifiers,
        includeClassOrigin,
        propertyList,
        QueueIdStack()));

    Message* message = _doRequest(request, CIM_ASSOCIATORS_RESPONSE_MESSAGE);

    CIMAssociatorsResponseMessage* response =
        (CIMAssociatorsResponseMessage*)message;

    AutoPtr<CIMAssociatorsResponseMessage> destroyer(response);

    return response->cimObjects;
}

Array<CIMObjectPath> CIMClientRep::associatorNames(
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& objectName,
    const CIMName& assocClass,
    const CIMName& resultClass,
    const String& role,
    const String& resultRole)
{
    AutoPtr<CIMRequestMessage> request(new CIMAssociatorNamesRequestMessage(
        String::EMPTY,
        nameSpace,
        objectName,
        assocClass,
        resultClass,
        role,
        resultRole,
        QueueIdStack()));

    Message* message =
        _doRequest(request, CIM_ASSOCIATOR_NAMES_RESPONSE_MESSAGE);

    CIMAssociatorNamesResponseMessage* response =
        (CIMAssociatorNamesResponseMessage*)message;

    AutoPtr<CIMAssociatorNamesResponseMessage> destroyer(response);

    return response->objectNames;
}

Array<CIMObject> CIMClientRep::references(
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& objectName,
    const CIMName& resultClass,
    const String& role,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{
    AutoPtr<CIMRequestMessage> request(new CIMReferencesRequestMessage(
        String::EMPTY,
        nameSpace,
        objectName,
        resultClass,
        role,
        includeQualifiers,
        includeClassOrigin,
        propertyList,
        QueueIdStack()));

    Message* message = _doRequest(request, CIM_REFERENCES_RESPONSE_MESSAGE);

    CIMReferencesResponseMessage* response =
        (CIMReferencesResponseMessage*)message;

    AutoPtr<CIMReferencesResponseMessage> destroyer(response);

    return response->cimObjects;
}

Array<CIMObjectPath> CIMClientRep::referenceNames(
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& objectName,
    const CIMName& resultClass,
    const String& role)
{
    AutoPtr<CIMRequestMessage> request(new CIMReferenceNamesRequestMessage(
        String::EMPTY,
        nameSpace,
        objectName,
        resultClass,
        role,
        QueueIdStack()));

    Message* message =
        _doRequest(request, CIM_REFERENCE_NAMES_RESPONSE_MESSAGE);

    CIMReferenceNamesResponseMessage* response =
        (CIMReferenceNamesResponseMessage*)message;

    AutoPtr<CIMReferenceNamesResponseMessage> destroyer(response);

    return response->objectNames;
}

CIMValue CIMClientRep::getProperty(
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& instanceName,
    const CIMName& propertyName)
{
    AutoPtr<CIMRequestMessage> request(new CIMGetPropertyRequestMessage(
        String::EMPTY,
        nameSpace,
        instanceName,
        propertyName,
        QueueIdStack()));

    Message* message = _doRequest(request, CIM_GET_PROPERTY_RESPONSE_MESSAGE);

    CIMGetPropertyResponseMessage* response =
        (CIMGetPropertyResponseMessage*)message;

    AutoPtr<CIMGetPropertyResponseMessage> destroyer(response);

    return response->value;
}

void CIMClientRep::setProperty(
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& instanceName,
    const CIMName& propertyName,
    const CIMValue& newValue)
{
    AutoPtr<CIMRequestMessage> request(new CIMSetPropertyRequestMessage(
        String::EMPTY,
        nameSpace,
        instanceName,
        propertyName,
        newValue,
        QueueIdStack()));

    Message* message = _doRequest(request, CIM_SET_PROPERTY_RESPONSE_MESSAGE);

    CIMSetPropertyResponseMessage* response =
        (CIMSetPropertyResponseMessage*)message;

    AutoPtr<CIMSetPropertyResponseMessage> destroyer(response);
}

CIMQualifierDecl CIMClientRep::getQualifier(
    const CIMNamespaceName& nameSpace,
    const CIMName& qualifierName)
{
    AutoPtr<CIMRequestMessage> request(new CIMGetQualifierRequestMessage(
        String::EMPTY,
        nameSpace,
        qualifierName,
        QueueIdStack()));

    Message* message = _doRequest(request, CIM_GET_QUALIFIER_RESPONSE_MESSAGE);

    CIMGetQualifierResponseMessage* response =
        (CIMGetQualifierResponseMessage*)message;

    AutoPtr<CIMGetQualifierResponseMessage> destroyer(response);

    return response->cimQualifierDecl;
}

void CIMClientRep::setQualifier(
    const CIMNamespaceName& nameSpace,
    const CIMQualifierDecl& qualifierDeclaration)
{
    AutoPtr<CIMRequestMessage> request(new CIMSetQualifierRequestMessage(
        String::EMPTY,
        nameSpace,
        qualifierDeclaration,
        QueueIdStack()));
	ck_dacim_redirect()
    Message* message = _doRequest(request, CIM_SET_QUALIFIER_RESPONSE_MESSAGE);
    _directaccess_redirect = false;
    CIMSetQualifierResponseMessage* response =
        (CIMSetQualifierResponseMessage*)message;

    AutoPtr<CIMSetQualifierResponseMessage> destroyer(response);
}

void CIMClientRep::deleteQualifier(
    const CIMNamespaceName& nameSpace,
    const CIMName& qualifierName)
{
    AutoPtr<CIMRequestMessage> request(new CIMDeleteQualifierRequestMessage(
        String::EMPTY,
        nameSpace,
        qualifierName,
        QueueIdStack()));

	ck_dacim_redirect()
    Message* message =
        _doRequest(request, CIM_DELETE_QUALIFIER_RESPONSE_MESSAGE);
    _directaccess_redirect = false;
    CIMDeleteQualifierResponseMessage* response =
        (CIMDeleteQualifierResponseMessage*)message;

    AutoPtr<CIMDeleteQualifierResponseMessage> destroyer(response);
}

Array<CIMQualifierDecl> CIMClientRep::enumerateQualifiers(
    const CIMNamespaceName& nameSpace)
{
    AutoPtr<CIMRequestMessage> request(new CIMEnumerateQualifiersRequestMessage(
        String::EMPTY,
        nameSpace,
        QueueIdStack()));

    Message* message =
        _doRequest(request, CIM_ENUMERATE_QUALIFIERS_RESPONSE_MESSAGE);

    CIMEnumerateQualifiersResponseMessage* response =
        (CIMEnumerateQualifiersResponseMessage*)message;

    AutoPtr<CIMEnumerateQualifiersResponseMessage> destroyer(response);

    return response->qualifierDeclarations;
}

CIMValue CIMClientRep::invokeMethod(
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& instanceName,
    const CIMName& methodName,
    const Array<CIMParamValue>& inParameters,
    Array<CIMParamValue>& outParameters)
{
    // ATTN-RK-P2-20020301: Does it make sense to have a nameSpace parameter
    // when the namespace should already be included in the instanceName?
    // ATTN-RK-P3-20020301: Do we need to make sure the caller didn't specify
    // a host name in the instanceName?

    // solved with PEP#139 Stage1 as other CIMOMs contained in the object path
    // will cause a TypeMisMatchException

    AutoPtr<CIMRequestMessage> request(new CIMInvokeMethodRequestMessage(
        String::EMPTY,
        nameSpace,
        instanceName,
        methodName,
        inParameters,
        QueueIdStack()));

    Message* message = _doRequest(request, CIM_INVOKE_METHOD_RESPONSE_MESSAGE);

    CIMInvokeMethodResponseMessage* response =
        (CIMInvokeMethodResponseMessage*)message;

    AutoPtr<CIMInvokeMethodResponseMessage> destroyer(response);

    outParameters = response->outParameters;

    return response->retValue;
}

//------------------------------------------------------
Message* CIMClientRep::_doRequest(
    AutoPtr<CIMRequestMessage>& request,
    Uint32 expectedResponseMessageType)
{
    if (!_connected && !_doReconnect)
    {
        request.reset();
        throw NotConnectedException();
    }
#ifdef PEGASUS_USE_DIRECTACCESS_FOR_LOCAL_DEPEND
    if ( _directaccesslocalproviders && !_directaccess_redirect ) {
++numdacim;        
        Message *response = _localizer->dorequest(request);
        if (!response) {
            }
        if ( response->getType() == CLIENT_EXCEPTION_MESSAGE ) {
            Exception *clientexcep = ((ClientExceptionMessage*)response)->
                                     clientException;
            CIMException *cimexcep = dynamic_cast<CIMException*>(clientexcep);
            delete response;                  // fix! deletes what??
            if (cimexcep) throw cimexcep;
            throw *clientexcep;
            }
        else if ( response->getType() == DIRECTACCESSCIM_NOTSUPPORTED_TEMP ) {
			// should see these only during dev; should not occur with intg 
			// build done.
            CIMException cimexcep( CIM_ERR_NOT_SUPPORTED,
                                   "DACIM does not yet handle this." );
            delete response;
            throw cimexcep;
            }
        //else if ( response->getType() == DIRECTACCESSCIM_NOTSUPPORTED_REQUEST ) {
		//	// pass these along to the cimserver, bau, as if direct access cim didn't
		//	// exist.
		//	_directaccess_redirect = true;
		//	connectLocal();
		//	_directaccess_redirect = false;
		//	// note; don't return here.
		//    }	
        else {
            if ( response->getType() != expectedResponseMessageType ) {   
                MessageLoaderParms mlParms(
                  "Client.CIMOperationResponseDecoder.MISMATCHED_RESPONSE_TYPE",
                  "Mismatched response message type.");               // fix
                String mlString(MessageLoader::getMessage(mlParms));
                CIMClientResponseException responseexcep(mlString);             
                delete response;
                throw responseexcep;
                }
            CIMResponseMessage *crm = (CIMResponseMessage*)response;
            if (crm->cimException.getCode() != CIM_ERR_SUCCESS) {
                CIMException cimexcep( crm->cimException.getCode(),
                              (crm->cimException.getMessage().size() > 0)?TraceableCIMException(crm->cimException).getDescription():cimStatusCodeToString(crm->cimException.getCode()));
                delete response;
                throw cimexcep;
                }
            return response;                          // <-- note.
            }
    }
#endif

++numnondacim;    

    if (_doReconnect)
    {
        _connect();
        _doReconnect = false;
    }

    String messageId = XmlWriter::getNextMessageId();
    const_cast<String &>(request->messageId) = messageId;

    _authenticator.setRequestMessage(0);

    // ATTN-RK-P2-20020416: We should probably clear out the queue first.
    PEGASUS_ASSERT(getCount() == 0);  // Shouldn't be any messages in our queue

    //
    //  Set HTTP method in request to POST
    //
    //Bug 478/418 - Change this to do post call, not mpost
    request->setHttpMethod (HTTP_METHOD__POST);

    // Set the Accept-Languages and Content-Languages into
    // the request message

    request->operationContext.set(
        AcceptLanguageListContainer(requestAcceptLanguages));
    request->operationContext.set(
        ContentLanguageListContainer(requestContentLanguages));


    //gathering statistical information about client operation
    perfDataStore.reset();
    perfDataStore.setOperationType(request->getType());
    perfDataStore.setMessageID(request->messageId);

    // Sending a new request, so clear out the response Content-Languages
    responseContentLanguages.clear();

    _requestEncoder->enqueue(request.get());
    request.release();

    Uint64 startMilliseconds = TimeValue::getCurrentTime().toMilliseconds();
    Uint64 nowMilliseconds = startMilliseconds;
    Uint64 stopMilliseconds = nowMilliseconds + _timeoutMilliseconds;

    while (nowMilliseconds < stopMilliseconds)
    {
        //
        // Wait until the timeout expires or an event occurs:
        //
        _monitor->run(Uint32(stopMilliseconds - nowMilliseconds));

        //
        // Check to see if incoming queue has a message
        //

        AutoPtr<Message> response(dequeue());

        if (response.get())
        {
            // Shouldn't be any more messages in our queue
            PEGASUS_ASSERT(getCount() == 0);

            //
            // Close the connection if response contained a "Connection: Close"
            // header (e.g. at authentication challenge)
            //
            if (response->getCloseConnect() == true)
            {
                _disconnect();
                _doReconnect = true;
                response->setCloseConnect(false);
            }

            //
            //  Future:  If M-POST is used and HTTP response is 501 Not
            //  Implemented or 510 Not Extended, retry with POST method
            //

            if (response->getType() == CLIENT_EXCEPTION_MESSAGE)
            {
                Exception* clientException =
                    ((ClientExceptionMessage*)response.get())->clientException;

                AutoPtr<Exception> d(clientException);

                // Make the ContentLanguage of the exception available through
                // the CIMClient API (its also available in the exception).
                responseContentLanguages =
                    clientException->getContentLanguages();

                //
                // Determine and throw the specific class of client exception
                //

                CIMClientMalformedHTTPException* malformedHTTPException =
                    dynamic_cast<CIMClientMalformedHTTPException*>(
                        clientException);
                if (malformedHTTPException)
                {
                    throw *malformedHTTPException;
                }

                CIMClientHTTPErrorException* httpErrorException =
                    dynamic_cast<CIMClientHTTPErrorException*>(
                        clientException);
                if (httpErrorException)
                {
                    throw *httpErrorException;
                }

                CIMClientXmlException* xmlException =
                    dynamic_cast<CIMClientXmlException*>(clientException);
                if (xmlException)
                {
                    throw *xmlException;
                }

                CIMClientResponseException* responseException =
                    dynamic_cast<CIMClientResponseException*>(clientException);
                if (responseException)
                {
                    throw *responseException;
                }

                CIMException* cimException =
                    dynamic_cast<CIMException*>(clientException);
                if (cimException)
                {
                    throw *cimException;
                }

                throw *clientException;
            }
            else if (response->getType() == expectedResponseMessageType)
            {
                CIMResponseMessage* cimResponse =
                    (CIMResponseMessage*)response.get();

                if (cimResponse->messageId != messageId)
                {
                    MessageLoaderParms mlParms(
                        "Client.CIMClient.MISMATCHED_RESPONSE",
                        "Mismatched response message ID:  Got \"$0\", "
                            "expected \"$1\".",
                        cimResponse->messageId, messageId);
                    String mlString(MessageLoader::getMessage(mlParms));

                    CIMClientResponseException responseException(mlString);

                    throw responseException;
                }

                // Get the Content-Languages from the response's
                // operationContext and make available through the
                // CIMClient API
                responseContentLanguages = ((ContentLanguageListContainer)
                    cimResponse->operationContext.get(
                        ContentLanguageListContainer::NAME)).getLanguages();

                if (cimResponse->cimException.getCode() != CIM_ERR_SUCCESS)
                {
                    CIMException cimException(cimResponse->cimException);

                    cimException.setContentLanguages(responseContentLanguages);
                    throw cimException;
                }

                // if execution gets here everytihng is working correctly
                // and a proper response was generated and received.

                // Check that client side statistics are valid before
                // handing them to the client application via a call back
                Boolean re_check = perfDataStore.checkMessageIDandType(
                    cimResponse->messageId,
                    cimResponse->getType());

                if (re_check &&
                    !perfDataStore.getStatError() &&
                    perfDataStore.isClassRegistered())
                {
                   // If callback method throws an exception it will be seen
                   // by the client no try/catch block is used here
                   // intentionaly - because exceptions come from the client
                   // application so client app. should handle them
                   ClientOpPerformanceData item =
                       perfDataStore.createPerfDataStruct();
                   perfDataStore.handler_prt->handleClientOpPerformanceData(
                       item);

                }//end of if statmet that call the callback method
                return response.release();
            }
            else if (dynamic_cast<CIMRequestMessage*>(response.get()) != 0)
            {
                //
                // Respond to an authentication challenge.
                // Reconnect if the connection was closed.
                //
                if (_doReconnect)
                {
                    _connect();
                }

                _requestEncoder->enqueue(response.release());
                nowMilliseconds = TimeValue::getCurrentTime().toMilliseconds();
                stopMilliseconds = nowMilliseconds + _timeoutMilliseconds;
                continue;
            }
            else
            {
                MessageLoaderParms mlParms(
                    "Client.CIMOperationResponseDecoder."
                        "MISMATCHED_RESPONSE_TYPE",
                    "Mismatched response message type.");
                String mlString(MessageLoader::getMessage(mlParms));

                CIMClientResponseException responseException(mlString);

                throw responseException;
            }
        }

        nowMilliseconds = TimeValue::getCurrentTime().toMilliseconds();
    }

    //
    // Reconnect to reset the connection (disregard late response)
    //

    _disconnect();
    _authenticator.resetChallengeStatus();
    _doReconnect = true;

    //
    // Throw timed out exception:
    //
    throw ConnectionTimeoutException();
}

//---------------------------------------------
String CIMClientRep::_getLocalHostName()
{
    static String hostname;

    if (!hostname.size())
    {
        hostname.assign(System::getHostName());
    }

    return hostname;
}
void CIMClientRep::registerClientOpPerformanceDataHandler(
    ClientOpPerformanceDataHandler& handler)
{
    perfDataStore.handler_prt = &handler;
    perfDataStore.setClassRegistered(true);
}

void CIMClientRep::deregisterClientOpPerformanceDataHandler()
{
    perfDataStore.handler_prt = NULL;
    perfDataStore.setClassRegistered(false);
}

PEGASUS_NAMESPACE_END
