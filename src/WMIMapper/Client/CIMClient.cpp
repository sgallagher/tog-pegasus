//%/////////////////////////////////////////////////////////////////////////////
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
// Author: Jair F. T. Santos (t.dos.santos.francisco@hp.com)
//
// Modified By: 
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/Monitor.h>
#include <Pegasus/Common/HTTPConnector.h>
#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Common/HTTPConnection.h>
#include <Pegasus/Common/Destroyer.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/TimeValue.h>
#include <Pegasus/Common/PegasusVersion.h>

#include "CIMOperationResponseDecoder.h"
#include "CIMOperationRequestEncoder.h"
#include "ClientAuthenticator.h"
#include "CIMClient.h"

#include <iostream>
#include <fstream>
#ifdef PEGASUS_PLATFORM_WIN32_IX86_MSVC
# include <windows.h>
#else
# include <netinet/in.h>
# include <arpa/inet.h>
# include <sys/socket.h>
#endif

///////////////////////////////////////////////////////////////////////////////
// WMI Provider interface headers
#include <WMIMapper/WMIProvider/WMIInstanceProvider.h>
#include <WMIMapper/WMIProvider/WMIClassProvider.h>
#include <WMIMapper/WMIProvider/WMIAssociatorProvider.h>
#include <WMIMapper/WMIProvider/WMIReferenceProvider.h>
#include <WMIMapper/WMIProvider/WMIQualifierProvider.h>
#include <WMIMapper/WMIProvider/WMIMethodProvider.h>
#include <WMIMapper/WMIProvider/WMIQueryProvider.h>
///////////////////////////////////////////////////////////////////////////////

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
//
// WMIClientRep
//
///////////////////////////////////////////////////////////////////////////////
class WMIClientRep : public MessageQueue
{
public:

    enum { DEFAULT_TIMEOUT_MILLISECONDS = 20000 };

    // Timeout value defines the time the CIMClient will wait for a response
    // to an outstanding request.  If a request times out, the connection
    // gets reset (disconnected and reconnected).
    WMIClientRep(Uint32 timeoutMilliseconds = DEFAULT_TIMEOUT_MILLISECONDS);

    ~WMIClientRep();

    virtual void handleEnqueue();

    Uint32 getTimeout() const
    {
	return _timeoutMilliseconds;
    }

    void setTimeout(Uint32 timeoutMilliseconds)
    {
	_timeoutMilliseconds = timeoutMilliseconds;
    }

    void connect(
        const String& host,
        const Uint32 portNumber,
        const String& userName,
        const String& password
    );

    void connect(
        const String& host,
        const Uint32 portNumber,
        const SSLContext& sslContext,
        const String& userName,
        const String& password
    );

    void connectLocal();

    void disconnect();

    virtual CIMClass getClass(
	const CIMNamespaceName& nameSpace,
	const CIMName& className,
	Boolean localOnly = true,
	Boolean includeQualifiers = true,
	Boolean includeClassOrigin = false,
	const CIMPropertyList& propertyList = CIMPropertyList()
    );

    virtual CIMInstance getInstance(
	const CIMNamespaceName& nameSpace,
	const CIMObjectPath& instanceName,
	Boolean localOnly = true,
	Boolean includeQualifiers = false,
	Boolean includeClassOrigin = false,
	const CIMPropertyList& propertyList = CIMPropertyList()
    );

    virtual void deleteClass(
	const CIMNamespaceName& nameSpace,
	const CIMName& className
    );

    virtual void deleteInstance(
	const CIMNamespaceName& nameSpace,
	const CIMObjectPath& instanceName
    );

    virtual void createClass(
	const CIMNamespaceName& nameSpace,
	const CIMClass& newClass
    );

    virtual CIMObjectPath createInstance(
	const CIMNamespaceName& nameSpace,
	const CIMInstance& newInstance
    );

    virtual void modifyClass(
	const CIMNamespaceName& nameSpace,
	const CIMClass& modifiedClass
    );

    virtual void modifyInstance(
	const CIMNamespaceName& nameSpace,
	const CIMInstance& modifiedInstance,
	Boolean includeQualifiers = true,
	const CIMPropertyList& propertyList = CIMPropertyList()
    );

    virtual Array<CIMClass> enumerateClasses(
	const CIMNamespaceName& nameSpace,
	const CIMName& className = CIMName(),
	Boolean deepInheritance = false,
	Boolean localOnly = true,
	Boolean includeQualifiers = true,
	Boolean includeClassOrigin = false
    );

    virtual Array<CIMName> enumerateClassNames(
	const CIMNamespaceName& nameSpace,
	const CIMName& className = CIMName(),
	Boolean deepInheritance = false
    );

    virtual Array<CIMInstance> enumerateInstances(
	const CIMNamespaceName& nameSpace,
	const CIMName& className,
	Boolean deepInheritance = true,
	Boolean localOnly = true,
	Boolean includeQualifiers = false,
	Boolean includeClassOrigin = false,
	const CIMPropertyList& propertyList = CIMPropertyList()
    );

    virtual Array<CIMObjectPath> enumerateInstanceNames(
	const CIMNamespaceName& nameSpace,
	const CIMName& className
    );

    virtual Array<CIMObject> execQuery(
	const CIMNamespaceName& nameSpace,
	const String& queryLanguage,
	const String& query
    );

    virtual Array<CIMObject> associators(
	const CIMNamespaceName& nameSpace,
	const CIMObjectPath& objectName,
	const CIMName& assocClass = CIMName(),
	const CIMName& resultClass = CIMName(),
	const String& role = String::EMPTY,
	const String& resultRole = String::EMPTY,
	Boolean includeQualifiers = false,
	Boolean includeClassOrigin = false,
	const CIMPropertyList& propertyList = CIMPropertyList()
    );

    virtual Array<CIMObjectPath> associatorNames(
	const CIMNamespaceName& nameSpace,
	const CIMObjectPath& objectName,
	const CIMName& assocClass = CIMName(),
	const CIMName& resultClass = CIMName(),
	const String& role = String::EMPTY,
	const String& resultRole = String::EMPTY
    );

    virtual Array<CIMObject> references(
	const CIMNamespaceName& nameSpace,
	const CIMObjectPath& objectName,
	const CIMName& resultClass = CIMName(),
	const String& role = String::EMPTY,
	Boolean includeQualifiers = false,
	Boolean includeClassOrigin = false,
	const CIMPropertyList& propertyList = CIMPropertyList()
    );

    virtual Array<CIMObjectPath> referenceNames(
	const CIMNamespaceName& nameSpace,
	const CIMObjectPath& objectName,
	const CIMName& resultClass = CIMName(),
	const String& role = String::EMPTY
    );

    virtual CIMValue getProperty(
	const CIMNamespaceName& nameSpace,
	const CIMObjectPath& instanceName,
	const CIMName& propertyName
    );

    virtual void setProperty(
	const CIMNamespaceName& nameSpace,
	const CIMObjectPath& instanceName,
	const CIMName& propertyName,
	const CIMValue& newValue = CIMValue()
    );

    virtual CIMQualifierDecl getQualifier(
	const CIMNamespaceName& nameSpace,
	const CIMName& qualifierName
    );

    virtual void setQualifier(
	const CIMNamespaceName& nameSpace,
	const CIMQualifierDecl& qualifierDeclaration
    );

    virtual void deleteQualifier(
	const CIMNamespaceName& nameSpace,
	const CIMName& qualifierName
    );

    virtual Array<CIMQualifierDecl> enumerateQualifiers(
	const CIMNamespaceName& nameSpace
    );

    virtual CIMValue invokeMethod(
	const CIMNamespaceName& nameSpace,
	const CIMObjectPath& instanceName,
	const CIMName& methodName,
	const Array<CIMParamValue>& inParameters,
	Array<CIMParamValue>& outParameters
    );

private:

    void _connect();

    void _reconnect();

    Message* _doRequest(
        CIMRequestMessage * request,
		const Uint32 expectedResponseMessageType);

    String _getLocalHostName();

    Monitor* _monitor;
    HTTPConnector* _httpConnector;
    HTTPConnection* _httpConnection;
    Uint32 _timeoutMilliseconds;
    Boolean _connected;
    CIMOperationResponseDecoder* _responseDecoder;
    CIMOperationRequestEncoder* _requestEncoder;
    ClientAuthenticator _authenticator;
    String _connectHost;
    Uint32 _connectPortNumber;
    SSLContext* _connectSSLContext;
	Boolean _isLocal;
};

static Boolean verifyServerCertificate(SSLCertificateInfo &certInfo)
{
    //ATTN-NB-03-05132002: Add code to handle server certificate verification.
    return true;
}

///////////////////////////////////////////////////////////////////////////////
WMIClientRep::WMIClientRep(Uint32 timeoutMilliseconds)
    : 
    MessageQueue(PEGASUS_QUEUENAME_CLIENT),
    _httpConnection(0),
    _timeoutMilliseconds(timeoutMilliseconds),
    _connected(false),
	_isLocal(false),
    _responseDecoder(0),
    _requestEncoder(0),
    _connectSSLContext(0)
{
    //
    // Create Monitor and HTTPConnector
    //
    _monitor = new Monitor();
    _httpConnector = new HTTPConnector(_monitor);
}

///////////////////////////////////////////////////////////////////////////////
WMIClientRep::~WMIClientRep()
{
   disconnect();
   delete _httpConnector;
   delete _monitor;
}

///////////////////////////////////////////////////////////////////////////////
void WMIClientRep::handleEnqueue()
{
}

///////////////////////////////////////////////////////////////////////////////
void WMIClientRep::_connect()
{
    //
    // Create response decoder:
    //
    _responseDecoder = new CIMOperationResponseDecoder(
        this, _requestEncoder, &_authenticator);
    
    //
    // Attempt to establish a connection:
    //
    try
    {
        _httpConnection = _httpConnector->connect(_connectHost,
                                                  _connectPortNumber,
                                                  _connectSSLContext,
                                                  _responseDecoder);
    }
    catch (CannotCreateSocketException& e)
    {
        delete _responseDecoder;
        throw e;
    }
    catch (CannotConnectException& e)
    {
        delete _responseDecoder;
        throw e;
    }
    catch (InvalidLocatorException& e)
    {
        delete _responseDecoder;
        throw e;
    }
    
    //
    // Create request encoder:
    //
    _requestEncoder = new CIMOperationRequestEncoder(
        _httpConnection, &_authenticator);

    _responseDecoder->setEncoderQueue(_requestEncoder);

    _connected = true;
}

///////////////////////////////////////////////////////////////////////////////
void WMIClientRep::_reconnect()
{
	if (!_isLocal)
	{
		disconnect();
		_connect();
	}
}

///////////////////////////////////////////////////////////////////////////////
void WMIClientRep::connect(
    const String& host,
    const Uint32 portNumber,
    const String& userName,
    const String& password
)
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
    _authenticator.clearRequest(true);
    _authenticator.setAuthType(ClientAuthenticator::NONE);

    if (userName.size())
    {
        _authenticator.setUserName(userName);
    }

    if (password.size())
    {
        _authenticator.setPassword(password);
    }

    _connectSSLContext = 0;
    _connectHost = hostName;
    _connectPortNumber = portNumber;

    _connect();
}

///////////////////////////////////////////////////////////////////////////////
void WMIClientRep::connect(
    const String& host,
    const Uint32 portNumber,
    const SSLContext& sslContext,
    const String& userName,
    const String& password
)
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
    _authenticator.clearRequest(true);
    _authenticator.setAuthType(ClientAuthenticator::NONE);

    if (userName.size())
    {
        _authenticator.setUserName(userName);
    }

    if (password.size())
    {
        _authenticator.setPassword(password);
    }

    _connectSSLContext = new SSLContext(sslContext);
    _connectHost = hostName;
    _connectPortNumber = portNumber;


    try
    {
        _connect();
    }
    catch (Exception&)
    {
        delete _connectSSLContext;
        _connectSSLContext = 0;
        throw;
    }
}

///////////////////////////////////////////////////////////////////////////////
void WMIClientRep::connectLocal()
{
    //
    // If already connected, bail out!
    //
    if (_connected)
	throw AlreadyConnectedException();

	_isLocal = true;
	_connected = true;

    /*
	//
    // Set authentication type
    //
    _authenticator.clearRequest(true);
    _authenticator.setAuthType(ClientAuthenticator::LOCAL);

#ifdef PEGASUS_LOCAL_DOMAIN_SOCKET
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
        _connectHost.assign(_getLocalHostName());

        _connectSSLContext = 0;

        _connect();
    }
    catch(CannotConnectException &e)
    {
        //
        // Look up the WBEM HTTPS port number for the local system
        //
        _connectPortNumber = System::lookupPort (WBEM_HTTPS_SERVICE_NAME, 
            WBEM_DEFAULT_HTTPS_PORT);

        //
        //  Assign host
        //
        _connectHost.assign(_getLocalHostName());

        //
        // Create SSLContext
        //

        const char* pegasusHome = getenv("PEGASUS_HOME");

        String certpath = FileSystem::getAbsolutePath(
            pegasusHome, PEGASUS_SSLCLIENT_CERTIFICATEFILE);

        String randFile = String::EMPTY;

#ifdef PEGASUS_SSL_RANDOMFILE
        randFile = FileSystem::getAbsolutePath(
            pegasusHome, PEGASUS_SSLCLIENT_RANDOMFILE);
#endif

        try
        {
            _connectSSLContext =
                new SSLContext(certpath, verifyServerCertificate, randFile);
        }
        catch (SSLException &se)
        {
            throw se;
        }

        try
        {
            _connect();
        }
        catch (Exception&)
        {
            delete _connectSSLContext;
            _connectSSLContext = 0;
            throw;
        }
    }
#endif
	*/
}

///////////////////////////////////////////////////////////////////////////////
void WMIClientRep::disconnect()
{
    if (_connected)
    {
        if (!_isLocal)
		{
			//
			// destroy response decoder
			//
			if (_responseDecoder)
			{
				delete _responseDecoder;
				_responseDecoder = 0;
			}

			//
			// Close the connection
			//
			if (_httpConnector)
			{
				_httpConnector->disconnect(_httpConnection);
			}

			//
			// destroy request encoder
			//
			if (_requestEncoder)
			{
				delete _requestEncoder;
				_requestEncoder = 0;
			}

			_authenticator.clearRequest(true);

			if (_connectSSLContext)
			{
				delete _connectSSLContext;
				_connectSSLContext = 0;
			}
		}
        _connected = false;
		_isLocal = false;
    }
}

///////////////////////////////////////////////////////////////////////////////
CIMClass WMIClientRep::getClass(
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{
    if (_isLocal)
	{
		CIMClass cimClass;
		CIMException cimException;
		
		try
		{
			//Initializes the WMI Provider Interface
   			WMIClassProvider provider;
			provider.initialize(TRUE);
			
			//Performs the WMI call
			cimClass = provider.getClass(
				nameSpace,
				String::EMPTY,
				String::EMPTY,
				className,
				localOnly,
				includeQualifiers,
				includeClassOrigin,
				propertyList);

			//terminate the provider
			provider.terminate();

		}
		catch(CIMException& exception)
		{
			cimException = exception;
			throw cimException;
		}
		catch(Exception& exception)
		{
		   cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
		   throw cimException;
		}
		catch(...)
		{
			cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "getClass() failed!");
			throw cimException;
		}

		return(cimClass);
	}
	else
	{
		CIMRequestMessage* request = new CIMGetClassRequestMessage(
			String::EMPTY,
			nameSpace,
			className,
			localOnly,
			includeQualifiers,
			includeClassOrigin,
			propertyList,
			QueueIdStack());
    
		Message* message = _doRequest(request, CIM_GET_CLASS_RESPONSE_MESSAGE);

		CIMGetClassResponseMessage* response = 
			(CIMGetClassResponseMessage*)message;
    
		Destroyer<CIMGetClassResponseMessage> destroyer(response);
    
		return(response->cimClass);
	}
}

///////////////////////////////////////////////////////////////////////////////
CIMInstance WMIClientRep::getInstance(
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& instanceName,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{
    if (_isLocal)
	{
		CIMInstance cimInstance;
		CIMException cimException;

		try
		{
			//Initializes the WMI Provider Interface
			WMIInstanceProvider provider;
			provider.initialize(TRUE);

			//Performs the WMI call
			cimInstance = provider.getInstance(
				nameSpace,
				String::EMPTY,
				String::EMPTY,
				instanceName,
				localOnly,
				includeQualifiers,
				includeClassOrigin,
				propertyList);

			//terminate the provider
			provider.terminate();
		}
		catch(CIMException& exception)
		{
			cimException = exception;
			throw cimException;
		}
		catch(Exception& exception)
		{
		   cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
		   throw cimException;
		}
		catch(...)
		{
			cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "getInstance() failed!");
			throw cimException;
		}

		return(cimInstance);
	}
	else
	{
		CIMRequestMessage* request = new CIMGetInstanceRequestMessage(
			String::EMPTY,
			nameSpace,
			instanceName,
			localOnly,
			includeQualifiers,
			includeClassOrigin,
			propertyList,
			QueueIdStack());
    
		Message* message = _doRequest(request, CIM_GET_INSTANCE_RESPONSE_MESSAGE);
    
		CIMGetInstanceResponseMessage* response = 
			(CIMGetInstanceResponseMessage*)message;
    
		Destroyer<CIMGetInstanceResponseMessage> destroyer(response);
    
		return(response->cimInstance);
	}
}

///////////////////////////////////////////////////////////////////////////////
void WMIClientRep::deleteClass(
    const CIMNamespaceName& nameSpace,
	const CIMName& className)
{
    if (_isLocal)
	{
		CIMException cimException;

		try
		{
			//Initializes the WMI Provider Interface
			WMIClassProvider provider;
   			provider.initialize(TRUE);

			//Performs the WMI call
			provider.deleteClass(
				nameSpace,
				String::EMPTY,
				String::EMPTY,
				className);

			//terminate the provider
			provider.terminate();
		}
		catch(CIMException& exception)
		{
			cimException = exception;
			throw cimException;
		}
		catch(Exception& exception)
		{
		   cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
		   throw cimException;
		}
		catch(...)
		{
			cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "deleteClass() failed!");
			throw cimException;
		}
	}
	else
	{
		CIMRequestMessage* request = new CIMDeleteClassRequestMessage(
			String::EMPTY,
			nameSpace,
			className,
			QueueIdStack());
    
		Message* message = _doRequest(request, CIM_DELETE_CLASS_RESPONSE_MESSAGE);
    
		CIMDeleteClassResponseMessage* response = 
			(CIMDeleteClassResponseMessage*)message;
    
		Destroyer<CIMDeleteClassResponseMessage> destroyer(response);
	}
}

///////////////////////////////////////////////////////////////////////////////
void WMIClientRep::deleteInstance(
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& instanceName)
{
	if (_isLocal)
	{
		CIMException cimException;

		try
		{
			//Initializes the WMI Provider Interface
    		WMIInstanceProvider provider;
			provider.initialize(TRUE);

			//Performs the WMI call
			provider.deleteInstance(
				nameSpace,
				String::EMPTY,
				String::EMPTY,
				instanceName);

			//terminate the provider
			provider.terminate();
		}
		catch(CIMException& exception)
		{
			cimException = exception;
			throw cimException;
		}
		catch(Exception& exception)
		{
		   cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
		   throw cimException;
		}
		catch(...)
		{
			cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "deleteInstance() failed!");
			throw cimException;
		}
	}
	else
	{
		CIMRequestMessage* request = new CIMDeleteInstanceRequestMessage(
			String::EMPTY,
			nameSpace,
			instanceName,
			QueueIdStack());
    
		Message* message = _doRequest(request, CIM_DELETE_INSTANCE_RESPONSE_MESSAGE);
    
		CIMDeleteInstanceResponseMessage* response = 
			(CIMDeleteInstanceResponseMessage*)message;
    
		Destroyer<CIMDeleteInstanceResponseMessage> destroyer(response);
	}
}

///////////////////////////////////////////////////////////////////////////////
void WMIClientRep::createClass(
    const CIMNamespaceName& nameSpace,
    const CIMClass& newClass)
{
    if (_isLocal)
	{
		CIMException cimException;

		try
		{
			//Initializes the WMI Provider Interface
    		WMIClassProvider provider;
			provider.initialize(TRUE);

			//Performs the WMI call
			provider.createClass(
				nameSpace,
				String::EMPTY,
				String::EMPTY,
				newClass);

			//terminate the provider
			provider.terminate();
		}
		catch(CIMException& exception)
		{
			cimException = exception;
			throw cimException;
		}
		catch(Exception& exception)
		{
		   cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
		   throw cimException;
		}
		catch(...)
		{
			cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "createClass() failed!");
			throw cimException;
		}
	}
	else
	{
		CIMRequestMessage* request = new CIMCreateClassRequestMessage(
			String::EMPTY,
			nameSpace,
			newClass,
			QueueIdStack());
    
		Message* message = _doRequest(request, CIM_CREATE_CLASS_RESPONSE_MESSAGE);
    
		CIMCreateClassResponseMessage* response = 
			(CIMCreateClassResponseMessage*)message;
    
		Destroyer<CIMCreateClassResponseMessage> destroyer(response);
	}
}

///////////////////////////////////////////////////////////////////////////////
CIMObjectPath WMIClientRep::createInstance(
    const CIMNamespaceName& nameSpace,
    const CIMInstance& newInstance)
{
    if (_isLocal)
	{
		CIMObjectPath instanceName;
		CIMException cimException;

		try
		{
			//Initializes the WMI Provider Interface
    		WMIInstanceProvider provider;
			provider.initialize(TRUE);

			//Performs the WMI call
			instanceName = provider.createInstance(
				nameSpace,
				String::EMPTY,
				String::EMPTY,
				newInstance);

			//terminate the provider
			provider.terminate();
		}
		catch(CIMException& exception)
		{
			cimException = exception;
			throw cimException;
		}
		catch(Exception& exception)
		{
		   cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
		   throw cimException;
		}
		catch(...)
		{
			cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "createInstance() failed!");
			throw cimException;
		}

		return (instanceName);
	}
	else
	{
		CIMRequestMessage* request = new CIMCreateInstanceRequestMessage(
			String::EMPTY,
			nameSpace,
			newInstance,
			QueueIdStack());
    
		Message* message = _doRequest(request, CIM_CREATE_INSTANCE_RESPONSE_MESSAGE);
    
		CIMCreateInstanceResponseMessage* response = 
			(CIMCreateInstanceResponseMessage*)message;
    
		Destroyer<CIMCreateInstanceResponseMessage> destroyer(response);
    
		return(response->instanceName);
	}
}

///////////////////////////////////////////////////////////////////////////////
void WMIClientRep::modifyClass(
    const CIMNamespaceName& nameSpace,
    const CIMClass& modifiedClass)
{
    if (_isLocal)
	{
		CIMException cimException;

		try
		{
			//Initializes the WMI Provider Interface
    		WMIClassProvider provider;
			provider.initialize(TRUE);

			//Performs the WMI call
			provider.modifyClass(
				nameSpace,
				String::EMPTY,
				String::EMPTY,
				modifiedClass);

			//terminate the provider
			provider.terminate();
		}
		catch(CIMException& exception)
		{
			cimException = exception;
			throw cimException;
		}
		catch(Exception& exception)
		{
		   cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
		   throw cimException;
		}
		catch(...)
		{
			cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "modifyClass() failed!");
			throw cimException;
		}
	}
	else
	{
		CIMRequestMessage* request = new CIMModifyClassRequestMessage(
			String::EMPTY,
			nameSpace,
			modifiedClass,
			QueueIdStack());
    
		Message* message = _doRequest(request, CIM_MODIFY_CLASS_RESPONSE_MESSAGE);
    
		CIMModifyClassResponseMessage* response = 
			(CIMModifyClassResponseMessage*)message;
    
		Destroyer<CIMModifyClassResponseMessage> destroyer(response);
	}
}

///////////////////////////////////////////////////////////////////////////////
void WMIClientRep::modifyInstance(
    const CIMNamespaceName& nameSpace,
    const CIMInstance& modifiedInstance,
    Boolean includeQualifiers,
    const CIMPropertyList& propertyList)
{
    if (_isLocal)
	{
		CIMException cimException;

		try
		{
			//Initializes the WMI Provider Interface
    		WMIInstanceProvider provider;
			provider.initialize(TRUE);

			//Performs the WMI call
			provider.modifyInstance(
				nameSpace,
				String::EMPTY,
				String::EMPTY,
				modifiedInstance,
				includeQualifiers,
				propertyList);

			//terminate the provider
			provider.terminate();
		}
		catch(CIMException& exception)
		{
			cimException = exception;
			throw cimException;
		}
		catch(Exception& exception)
		{
		   cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
		   throw cimException;
		}
		catch(...)
		{
			cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "modifyInstance() failed!");
			throw cimException;
		}
	}
	else
	{
		CIMRequestMessage* request = new CIMModifyInstanceRequestMessage(
			String::EMPTY,
			nameSpace,
			modifiedInstance,
			includeQualifiers,
			propertyList,
			QueueIdStack());
    
		Message* message = _doRequest(request, CIM_MODIFY_INSTANCE_RESPONSE_MESSAGE);
    
		CIMModifyInstanceResponseMessage* response = 
			(CIMModifyInstanceResponseMessage*)message;
    
		Destroyer<CIMModifyInstanceResponseMessage> destroyer(response);
	}
}

///////////////////////////////////////////////////////////////////////////////
Array<CIMClass> WMIClientRep::enumerateClasses(
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    Boolean deepInheritance,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin)
{
    if (_isLocal)
	{
		Array<CIMClass> cimClasses;
		CIMException cimException;

		try
		{
			//Initializes the WMI Provider Interface
    		WMIClassProvider provider;
			provider.initialize(TRUE);

			//Performs the WMI call
			cimClasses = provider.enumerateClasses(
				nameSpace,
				String::EMPTY,
				String::EMPTY,
				className,
				deepInheritance,
				localOnly,
				includeQualifiers,
				includeClassOrigin);

			//terminate the provider
			provider.terminate();
		}
		catch(CIMException& exception)
		{
			cimException = exception;
			throw cimException;
		}
		catch(Exception& exception)
		{
		   cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
		   throw cimException;
		}
		catch(...)
		{
			cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "enumerateClasses() failed!");
			throw cimException;
		}

		return(cimClasses);
	}
	else
	{
		CIMRequestMessage* request = new CIMEnumerateClassesRequestMessage(
			String::EMPTY,
			nameSpace,
			className,
			deepInheritance,
			localOnly,
			includeQualifiers,
			includeClassOrigin,
			QueueIdStack());
    
		Message* message = _doRequest(request, CIM_ENUMERATE_CLASSES_RESPONSE_MESSAGE);
    
		CIMEnumerateClassesResponseMessage* response = 
			(CIMEnumerateClassesResponseMessage*)message;
    
		Destroyer<CIMEnumerateClassesResponseMessage> destroyer(response);
    
		return(response->cimClasses);
	}
}

///////////////////////////////////////////////////////////////////////////////
Array<CIMName> WMIClientRep::enumerateClassNames(
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    Boolean deepInheritance)
{
    if (_isLocal)
	{
		Array<CIMName> classNames;
		CIMException cimException;

		try
		{
			//Initializes the WMI Provider Interface
    		WMIClassProvider provider;
			provider.initialize(TRUE);

			//Performs the WMI call
			classNames = provider.enumerateClassNames(
				nameSpace,
				String::EMPTY,
				String::EMPTY,
				className,
				deepInheritance);

			//terminate the provider
			provider.terminate();
		}
		catch(CIMException& exception)
		{
			cimException = exception;
			throw cimException;
		}
		catch(Exception& exception)
		{
		   cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
		   throw cimException;
		}
		catch(...)
		{
			cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "enumerateClassNames() failed!");
			throw cimException;
		}

		return(classNames);
	}
	else
	{
		CIMRequestMessage* request = new CIMEnumerateClassNamesRequestMessage(
			String::EMPTY,
			nameSpace,
			className,
			deepInheritance,
			QueueIdStack());
    
		Message* message = _doRequest(request, CIM_ENUMERATE_CLASS_NAMES_RESPONSE_MESSAGE);
    
		CIMEnumerateClassNamesResponseMessage* response = 
			(CIMEnumerateClassNamesResponseMessage*)message;
    
		Destroyer<CIMEnumerateClassNamesResponseMessage> destroyer(response);
    
		// Temporary code until internal structures use CIMName instead of String
		Array<CIMName> classNameArray;
		classNameArray.reserveCapacity(response->classNames.size());
		for (Uint32 i=0; i<response->classNames.size(); i++)
		{
			classNameArray.append(response->classNames[i]);
		}
		return(classNameArray);
	}
}

///////////////////////////////////////////////////////////////////////////////
Array<CIMInstance> WMIClientRep::enumerateInstances(
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    Boolean deepInheritance,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{
    if (_isLocal)
	{
		Array<CIMInstance> cimInstances;
		CIMPropertyList myPropertyList(propertyList);
		CIMException cimException;

		try
		{
			//Initializes the WMI Provider Interface
    		WMIInstanceProvider provider;
			provider.initialize(TRUE);

			//Performs the WMI call
			cimInstances = provider.enumerateInstances(
				nameSpace,
				String::EMPTY,
				String::EMPTY,
				className,
				deepInheritance,
				localOnly,
				includeQualifiers,
				includeClassOrigin,
				myPropertyList);

			//terminate the provider
			provider.terminate();
		}
		catch(CIMException& exception)
		{
			cimException = exception;
			throw cimException;
		}
		catch(Exception& exception)
		{
		   cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
		   throw cimException;
		}
		catch(...)
		{
			cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "enumerateInstances() failed!");
			throw cimException;
		}

		return(cimInstances);
	}
	else
	{
		CIMRequestMessage* request = new CIMEnumerateInstancesRequestMessage(
			String::EMPTY,
			nameSpace,
			className,
			deepInheritance,
			localOnly,
			includeQualifiers,
			includeClassOrigin,
			propertyList,
			QueueIdStack());
    
		Message* message = _doRequest(request, CIM_ENUMERATE_INSTANCES_RESPONSE_MESSAGE);
    
		CIMEnumerateInstancesResponseMessage* response = 
			(CIMEnumerateInstancesResponseMessage*)message;
    
		Destroyer<CIMEnumerateInstancesResponseMessage> destroyer(response);
    
		return(response->cimNamedInstances);
	}
}

///////////////////////////////////////////////////////////////////////////////
Array<CIMObjectPath> WMIClientRep::enumerateInstanceNames(
    const CIMNamespaceName& nameSpace,
    const CIMName& className)
{
    if (_isLocal)
	{
		Array<CIMObjectPath> instanceNames;
		CIMException cimException;
		
		try
		{
			//Initializes the WMI Provider Interface
    		WMIInstanceProvider provider;
			provider.initialize(TRUE);

			//Performs the WMI call
			instanceNames = provider.enumerateInstanceNames(
				nameSpace,
				String::EMPTY,
				String::EMPTY,
				className);

			//terminate the provider
			provider.terminate();
		}
		catch(CIMException& exception)
		{
			cimException = exception;
			throw cimException;
		}
		catch(Exception& exception)
		{
		   cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
		   throw cimException;
		}
		catch(...)
		{
			cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "enumerateInstanceNames() failed!");
			throw cimException;
		}

		return(instanceNames);
	}
	else
	{
		CIMRequestMessage* request = new CIMEnumerateInstanceNamesRequestMessage(
			String::EMPTY,
			nameSpace,
			className,
			QueueIdStack());
    
		Message* message = _doRequest(request, CIM_ENUMERATE_INSTANCE_NAMES_RESPONSE_MESSAGE);
    
		CIMEnumerateInstanceNamesResponseMessage* response = 
			(CIMEnumerateInstanceNamesResponseMessage*)message;
    
		Destroyer<CIMEnumerateInstanceNamesResponseMessage> destroyer(response);
    
	    return(response->instanceNames);
	}
}

///////////////////////////////////////////////////////////////////////////////
Array<CIMObject> WMIClientRep::execQuery(
    const CIMNamespaceName& nameSpace,
    const String& queryLanguage,
    const String& query)
{
    if (_isLocal)
	{
		Array<CIMObject> cimObjects;
		CIMException cimException;
		
		try
		{
			//Initializes the WMI Provider Interface
    		WMIQueryProvider provider;
			provider.initialize(TRUE);

			//Performs the WMI call
			cimObjects = provider.execQuery(
				nameSpace,
				String::EMPTY,
				String::EMPTY,
				queryLanguage,
				query);

			//terminate the provider
			provider.terminate();
		}
		catch(CIMException& exception)
		{
			cimException = exception;
			throw cimException;
		}
		catch(Exception& exception)
		{
		   cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
		   throw cimException;
		}
		catch(...)
		{
			cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "execQuery() failed!");
			throw cimException;
		}

		return(cimObjects);
	}
	else
	{
		CIMRequestMessage* request = new CIMExecQueryRequestMessage(
			String::EMPTY,
			nameSpace,
			queryLanguage,
			query,
			QueueIdStack());
    
		Message* message = _doRequest(request, CIM_EXEC_QUERY_RESPONSE_MESSAGE);
    
		CIMExecQueryResponseMessage* response = 
			(CIMExecQueryResponseMessage*)message;
    
		Destroyer<CIMExecQueryResponseMessage> destroyer(response);
    
		return(response->cimObjects);
	}
}

///////////////////////////////////////////////////////////////////////////////
Array<CIMObject> WMIClientRep::associators(
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
    if (_isLocal)
	{
		Array<CIMObject> cimObjects;
		CIMPropertyList myPropertyList(propertyList);
		CIMException cimException;
		
		try
		{
			//Initializes the WMI Provider Interface
    		WMIAssociatorProvider provider;
			provider.initialize(TRUE);

			//Performs the WMI call
			cimObjects = provider.associators(
				nameSpace,
				String::EMPTY,
				String::EMPTY,
				objectName,
				assocClass,
				resultClass,
				role,
				resultRole,
				includeQualifiers,
				includeClassOrigin,
				myPropertyList);

			//terminate the provider
			provider.terminate();
		}
		catch(CIMException& exception)
		{
			cimException = exception;
			throw cimException;
		}
		catch(Exception& exception)
		{
		   cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
		   throw cimException;
		}
		catch(...)
		{
			cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "associators() failed!");
			throw cimException;
		}

		return(cimObjects);
	}
	else
	{
		CIMRequestMessage* request = new CIMAssociatorsRequestMessage(
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
			QueueIdStack());
    
		Message* message = _doRequest(request, CIM_ASSOCIATORS_RESPONSE_MESSAGE);
    
		CIMAssociatorsResponseMessage* response = 
			(CIMAssociatorsResponseMessage*)message;
    
		Destroyer<CIMAssociatorsResponseMessage> destroyer(response);
    
		return(response->cimObjects);
	}
}

///////////////////////////////////////////////////////////////////////////////
Array<CIMObjectPath> WMIClientRep::associatorNames(
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& objectName,
    const CIMName& assocClass,
    const CIMName& resultClass,
    const String& role,
    const String& resultRole)
{
    if (_isLocal)
	{
		Array<CIMObjectPath> objectNames;
		CIMException cimException;
		
		try
		{
			//Initializes the WMI Provider Interface
    		WMIAssociatorProvider provider;
			provider.initialize(TRUE);

			//Performs the WMI call
			objectNames = provider.associatorNames(
				nameSpace,
				String::EMPTY,
				String::EMPTY,
				objectName,
				assocClass,
				resultClass,
				role,
				resultRole);

			//terminate the provider
			provider.terminate();
		}
		catch(CIMException& exception)
		{
			cimException = exception;
			throw cimException;
		}
		catch(Exception& exception)
		{
		   cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
		   throw cimException;
		}
		catch(...)
		{
			cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "associatorNames() failed!");
			throw cimException;
		}

		return(objectNames);
	}
	else
	{
		CIMRequestMessage* request = new CIMAssociatorNamesRequestMessage(
			String::EMPTY,
			nameSpace,
			objectName,
			assocClass,
			resultClass,
			role,
			resultRole,
			QueueIdStack());
    
		Message* message = _doRequest(request, CIM_ASSOCIATOR_NAMES_RESPONSE_MESSAGE);
    
		CIMAssociatorNamesResponseMessage* response = 
			(CIMAssociatorNamesResponseMessage*)message;
    
		Destroyer<CIMAssociatorNamesResponseMessage> destroyer(response);
    
		return(response->objectNames);
	}
}

///////////////////////////////////////////////////////////////////////////////
Array<CIMObject> WMIClientRep::references(
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& objectName,
    const CIMName& resultClass,
    const String& role,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{
    if (_isLocal)
	{
		Array<CIMObject> cimObjects;
		CIMPropertyList myPropertyList(propertyList);
		CIMException cimException;
		
		try
		{
			//Initializes the WMI Provider Interface
    		WMIReferenceProvider provider;
			provider.initialize(TRUE);

			//Performs the WMI call
			cimObjects = provider.references(
				nameSpace,
				String::EMPTY,
				String::EMPTY,
				objectName,
				resultClass,
				role,
				includeQualifiers,
				includeClassOrigin,
				myPropertyList);

			//terminate the provider
			provider.terminate();
		}
		catch(CIMException& exception)
		{
			cimException = exception;
			throw cimException;
		}
		catch(Exception& exception)
		{
		   cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
		   throw cimException;
		}
		catch(...)
		{
			cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "references() failed!");
			throw cimException;
		}

		return(cimObjects);
	}
	else
	{
		CIMRequestMessage* request = new CIMReferencesRequestMessage(
			String::EMPTY,
			nameSpace,
			objectName,
			resultClass,
			role,
			includeQualifiers,
			includeClassOrigin,
			propertyList,
			QueueIdStack());
    
		Message* message = _doRequest(request, CIM_REFERENCES_RESPONSE_MESSAGE);
    
		CIMReferencesResponseMessage* response = 
			(CIMReferencesResponseMessage*)message;
    
		Destroyer<CIMReferencesResponseMessage> destroyer(response);
    
		return(response->cimObjects);
	}
}

///////////////////////////////////////////////////////////////////////////////
Array<CIMObjectPath> WMIClientRep::referenceNames(
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& objectName,
    const CIMName& resultClass,
    const String& role)
{
    if (_isLocal)
	{
		Array<CIMObjectPath> objectNames;
		CIMException cimException;
		
		try
		{
			//Initializes the WMI Provider Interface
    		WMIReferenceProvider provider;
			provider.initialize(TRUE);

			//Performs the WMI call
			objectNames = provider.referenceNames(
				nameSpace,
				String::EMPTY,
				String::EMPTY,
				objectName,
				resultClass,
				role);

			//terminate the provider
			provider.terminate();
		}
		catch(CIMException& exception)
		{
			cimException = exception;
			throw cimException;
		}
		catch(Exception& exception)
		{
		   cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
		   throw cimException;
		}
		catch(...)
		{
			cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "referenceNames() failed!");
			throw cimException;
		}

		return(objectNames);
	}
	else
	{
		CIMRequestMessage* request = new CIMReferenceNamesRequestMessage(
			String::EMPTY,
			nameSpace,
			objectName,
			resultClass,
			role,
			QueueIdStack());
    
		Message* message = _doRequest(request, CIM_REFERENCE_NAMES_RESPONSE_MESSAGE);
    
		CIMReferenceNamesResponseMessage* response = 
			(CIMReferenceNamesResponseMessage*)message;
    
		Destroyer<CIMReferenceNamesResponseMessage> destroyer(response);
    
		return(response->objectNames);
	}
}

///////////////////////////////////////////////////////////////////////////////
CIMValue WMIClientRep::getProperty(
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& instanceName,
    const CIMName& propertyName)
{
    if (_isLocal)
	{
		CIMValue value;
		CIMException cimException;
		
		try
		{
			//Initializes the WMI Provider Interface
    		WMIInstanceProvider provider;
			provider.initialize(TRUE);

			//Performs the WMI call
			value = provider.getProperty(
				nameSpace,
				String::EMPTY,
				String::EMPTY,
				instanceName,
				propertyName);

			//terminate the provider
			provider.terminate();
		}
		catch(CIMException& exception)
		{
			cimException = exception;
			throw cimException;
		}
		catch(Exception& exception)
		{
		   cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
		   throw cimException;
		}
		catch(...)
		{
			cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "getProperty() failed!");
			throw cimException;
		}

		return(value);
	}
	else
	{
		CIMRequestMessage* request = new CIMGetPropertyRequestMessage(
			String::EMPTY,
			nameSpace,
			instanceName,
			propertyName,
			QueueIdStack());
    
		Message* message = _doRequest(request, CIM_GET_PROPERTY_RESPONSE_MESSAGE);
    
		CIMGetPropertyResponseMessage* response = 
			(CIMGetPropertyResponseMessage*)message;
    
		Destroyer<CIMGetPropertyResponseMessage> destroyer(response);
    
	    return(response->value);
	}
}

///////////////////////////////////////////////////////////////////////////////
void WMIClientRep::setProperty(
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& instanceName,
    const CIMName& propertyName,
    const CIMValue& newValue
)
{
    if (_isLocal)
	{
		CIMException cimException;

		try
		{
			//Initializes the WMI Provider Interface
    		WMIInstanceProvider provider;
			provider.initialize(TRUE);

			//Performs the WMI call
			provider.setProperty(
				nameSpace,
				String::EMPTY,
				String::EMPTY,
				instanceName,
				propertyName,
				newValue);

			//terminate the provider
			provider.terminate();
		}
		catch(CIMException& exception)
		{
			cimException = exception;
			throw cimException;
		}
		catch(Exception& exception)
		{
		   cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
		   throw cimException;
		}
		catch(...)
		{
			cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "setProperty() failed!");
			throw cimException;
		}
	}
	else
	{
		CIMRequestMessage* request = new CIMSetPropertyRequestMessage(
			String::EMPTY,
			nameSpace,
			instanceName,
			propertyName,
			newValue,
			QueueIdStack());
    
		Message* message = _doRequest(request, CIM_SET_PROPERTY_RESPONSE_MESSAGE);
    
		CIMSetPropertyResponseMessage* response = 
			(CIMSetPropertyResponseMessage*)message;
    
	    Destroyer<CIMSetPropertyResponseMessage> destroyer(response);
	}
}

///////////////////////////////////////////////////////////////////////////////
CIMQualifierDecl WMIClientRep::getQualifier(
    const CIMNamespaceName& nameSpace,
    const CIMName& qualifierName)
{
    if (_isLocal)
	{
		CIMQualifierDecl cimQualifierDecl;
		CIMException cimException;

		try
		{
			//Initializes the WMI Provider Interface
    		WMIQualifierProvider provider;
			provider.initialize(TRUE);

			//Performs the WMI call
			cimQualifierDecl = provider.getQualifier(
				nameSpace,
				String::EMPTY,
				String::EMPTY,
				qualifierName);

			//terminate the provider
			provider.terminate();
		}
		catch(CIMException& exception)
		{
			cimException = exception;
			throw cimException;
		}
		catch(Exception& exception)
		{
		   cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
		   throw cimException;
		}
		catch(...)
		{
			cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "getQualifier() failed!");
			throw cimException;
		}

		return(cimQualifierDecl);
	}
	else
	{
		CIMRequestMessage* request = new CIMGetQualifierRequestMessage(
			String::EMPTY,
			nameSpace,
			qualifierName,
			QueueIdStack());
    
		Message* message = _doRequest(request, CIM_GET_QUALIFIER_RESPONSE_MESSAGE);
    
		CIMGetQualifierResponseMessage* response = 
			(CIMGetQualifierResponseMessage*)message;
    
		Destroyer<CIMGetQualifierResponseMessage> destroyer(response);
    
		return(response->cimQualifierDecl);
	}
}

///////////////////////////////////////////////////////////////////////////////
void WMIClientRep::setQualifier(
    const CIMNamespaceName& nameSpace,
    const CIMQualifierDecl& qualifierDeclaration)
{
    if (_isLocal)
	{
		CIMException cimException;

		try
		{
			//Initializes the WMI Provider Interface
    		WMIQualifierProvider provider;
			provider.initialize(TRUE);

			//Performs the WMI call
			provider.setQualifier(
				nameSpace,
				String::EMPTY,
				String::EMPTY,
				qualifierDeclaration);

			//terminate the provider
			provider.terminate();
		}
		catch(CIMException& exception)
		{
			cimException = exception;
			throw cimException;
		}
		catch(Exception& exception)
		{
		   cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
		   throw cimException;
		}
		catch(...)
		{
			cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "setQualifier() failed!");
			throw cimException;
		}
	}
	else
	{
		CIMRequestMessage* request = new CIMSetQualifierRequestMessage(
			String::EMPTY,
			nameSpace,
			qualifierDeclaration,
			QueueIdStack());
    
		Message* message = _doRequest(request, CIM_SET_QUALIFIER_RESPONSE_MESSAGE);
    
		CIMSetQualifierResponseMessage* response = 
			(CIMSetQualifierResponseMessage*)message;
    
		Destroyer<CIMSetQualifierResponseMessage> destroyer(response);
	}
}

///////////////////////////////////////////////////////////////////////////////
void WMIClientRep::deleteQualifier(
    const CIMNamespaceName& nameSpace,
    const CIMName& qualifierName)
{
    if (_isLocal)
	{
		CIMException cimException;

		try
		{
			//Initializes the WMI Provider Interface
    		WMIQualifierProvider provider;
			provider.initialize(TRUE);

			//Performs the WMI call
			provider.deleteQualifier(
				nameSpace,
				String::EMPTY,
				String::EMPTY,
				qualifierName);
			//terminate the provider
			provider.terminate();
		}
		catch(CIMException& exception)
		{
			cimException = exception;
			throw cimException;
		}
		catch(Exception& exception)
		{
		   cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
		   throw cimException;
		}
		catch(...)
		{
			cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "deleteQualifier() failed!");
			throw cimException;
		}
	}
	else
	{
		CIMRequestMessage* request = new CIMDeleteQualifierRequestMessage(
			String::EMPTY,
			nameSpace,
			qualifierName,
			QueueIdStack());
    
		Message* message = _doRequest(request, CIM_DELETE_QUALIFIER_RESPONSE_MESSAGE);
    
		CIMDeleteQualifierResponseMessage* response = 
			(CIMDeleteQualifierResponseMessage*)message;
    
		Destroyer<CIMDeleteQualifierResponseMessage> destroyer(response);
	}
}

///////////////////////////////////////////////////////////////////////////////
Array<CIMQualifierDecl> WMIClientRep::enumerateQualifiers(
    const CIMNamespaceName& nameSpace)
{
    if (_isLocal)
	{
		Array<CIMQualifierDecl> qualifierDeclarations;
		CIMException cimException;

		try
		{
			//Initializes the WMI Provider Interface
    		WMIQualifierProvider provider;
			provider.initialize(TRUE);

			//Performs the WMI call
			qualifierDeclarations = provider.enumerateQualifiers(
				nameSpace,
				String::EMPTY,
				String::EMPTY);

			//terminate the provider
			provider.terminate();
		}
		catch(CIMException& exception)
		{
			cimException = exception;
			throw cimException;
		}
		catch(Exception& exception)
		{
		   cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
		   throw cimException;
		}
		catch(...)
		{
			cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "enumerateQualifiers() failed!");
			throw cimException;
		}

		return(qualifierDeclarations);
	}
	else
	{
		CIMRequestMessage* request = new CIMEnumerateQualifiersRequestMessage(
			String::EMPTY,
			nameSpace,
			QueueIdStack());
    
		Message* message = _doRequest(request, CIM_ENUMERATE_QUALIFIERS_RESPONSE_MESSAGE);
    
		CIMEnumerateQualifiersResponseMessage* response = 
			(CIMEnumerateQualifiersResponseMessage*)message;
    
		Destroyer<CIMEnumerateQualifiersResponseMessage> destroyer(response);
    
		return(response->qualifierDeclarations);
	}
}

///////////////////////////////////////////////////////////////////////////////
CIMValue WMIClientRep::invokeMethod(
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& instanceName,
    const CIMName& methodName,
    const Array<CIMParamValue>& inParameters,
    Array<CIMParamValue>& outParameters)
{
    if (_isLocal)
	{
		CIMValue retValue;
		CIMException cimException;

		try
		{
			//Initializes the WMI Provider Interface
    		WMIMethodProvider provider;
			provider.initialize(TRUE);

			//Performs the WMI call
			retValue = provider.invokeMethod(
				nameSpace,
				String::EMPTY,
				String::EMPTY,
				instanceName,
				methodName,
				inParameters,
				outParameters);

			//terminate the provider
			provider.terminate();
		}
		catch(CIMException& exception)
		{
			cimException = exception;
			throw cimException;
		}
		catch(Exception& exception)
		{
		   cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
		   throw cimException;
		}
		catch(...)
		{
			cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "invokeMethod() failed!");
			throw cimException;
		}

		return(retValue);
	}
	else
	{
		// ATTN-RK-P2-20020301: Does it make sense to have a nameSpace parameter
		// when the namespace should already be included in the instanceName?
		// ATTN-RK-P3-20020301: Do we need to make sure the caller didn't specify
		// a host name in the instanceName?

		CIMRequestMessage* request = new CIMInvokeMethodRequestMessage(
			String::EMPTY,
			nameSpace,
			instanceName,
			methodName,
			inParameters,
			QueueIdStack());
    
		Message* message = _doRequest(request, CIM_INVOKE_METHOD_RESPONSE_MESSAGE);
    
		CIMInvokeMethodResponseMessage* response = 
			(CIMInvokeMethodResponseMessage*)message;
    
		Destroyer<CIMInvokeMethodResponseMessage> destroyer(response);
    
		outParameters = response->outParameters;
    
		return(response->retValue);
	}
}

///////////////////////////////////////////////////////////////////////////////
Message* WMIClientRep::_doRequest(
    CIMRequestMessage * request,
    const Uint32 expectedResponseMessageType
)
{
    if (!_connected)
    {
        delete request;
	throw NotConnectedException();
    }
    
    String messageId = XmlWriter::getNextMessageId();
    const_cast<String &>(request->messageId) = messageId;

    _authenticator.clearRequest();

    // ATTN-RK-P2-20020416: We should probably clear out the queue first.
    PEGASUS_ASSERT(getCount() == 0);  // Shouldn't be any messages in our queue

    //
    //  Set HTTP method in request to M-POST
    //
    request->setHttpMethod (HTTP_METHOD_M_POST);

    _requestEncoder->enqueue(request);

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

	Message* response = dequeue();

	if (response)
	{
            // Shouldn't be any more messages in our queue
            PEGASUS_ASSERT(getCount() == 0);

            //
            //  ATTN-CAKG-20021001: If HTTP response is 501 Not Implemented
            //  or 510 Not Extended, retry with POST method
            //

            if (response->getType() == CLIENT_EXCEPTION_MESSAGE)
            {
                Exception* clientException =
                    ((ClientExceptionMessage*)response)->clientException;
                delete response;
                Destroyer<Exception> d(clientException);
                throw *clientException;
            }
            else if (response->getType() == expectedResponseMessageType)
            {
                CIMResponseMessage* cimResponse = (CIMResponseMessage*)response;
                if (cimResponse->messageId != messageId)
                {
                    CIMClientResponseException responseException(
                        String("Mismatched response message ID:  Got \"") +
                        cimResponse->messageId + "\", expected \"" +
                        messageId + "\".");
                    delete response;
	            throw responseException;
                }
                if (cimResponse->cimException.getCode() != CIM_ERR_SUCCESS)
                {
                    CIMException cimException(
                        cimResponse->cimException.getCode(),
                        cimResponse->cimException.getMessage());
                    delete response;
	            throw cimException;
                }
                return response;
            }
            else
            {
                CIMClientResponseException responseException(
                    "Mismatched response message type.");
                delete response;
	        throw responseException;
            }
	}

        nowMilliseconds = TimeValue::getCurrentTime().toMilliseconds();
	pegasus_yield();
    }

    //
    // Reconnect to reset the connection (disregard late response)
    //
    try
    {
        _reconnect();
    }
    catch (...)
    {
    }

    //
    // Throw timed out exception:
    //
    throw ConnectionTimeoutException();
}

///////////////////////////////////////////////////////////////////////////////
String WMIClientRep::_getLocalHostName()
{
    static String hostname;

    if (!hostname.size())
    {
        hostname.assign(System::getHostName());
    }

    return hostname;
}


///////////////////////////////////////////////////////////////////////////////
//
// CIMClient
//
///////////////////////////////////////////////////////////////////////////////

CIMClient::CIMClient()
{
    _rep = new WMIClientRep();
}

CIMClient::~CIMClient()
{
    delete _rep;
}

Uint32 CIMClient::getTimeout() const
{
    return _rep->getTimeout();
}

void CIMClient::setTimeout(Uint32 timeoutMilliseconds)
{
    _rep->setTimeout(timeoutMilliseconds);
}

void CIMClient::connect(
    const String& host,
    const Uint32 portNumber,
    const String& userName,
    const String& password
)
{
    _rep->connect(host, portNumber, userName, password);
}

void CIMClient::connect(
    const String& host,
    const Uint32 portNumber,
    const SSLContext& sslContext,
    const String& userName,
    const String& password
)
{
    _rep->connect(host, portNumber, sslContext, userName, password);
}

#ifndef PEGASUS_REMOVE_DEPRECATED
void CIMClient::connect(
    const String& address,
    const String& userName,
    const String& password
)
{
    Uint32 index = address.find (':');
    String host = address.subString (0, index);
    Uint32 portNumber = 0;
    if (index != PEG_NOT_FOUND)
    {
        String portStr = address.subString (index + 1, address.size ());
        sscanf (portStr.getCString (), "%u", &portNumber);
    }
    else
        throw InvalidLocatorException (address);
    _rep->connect (host, portNumber, userName, password);
}

void CIMClient::connect(
    const String& address,
    const SSLContext& sslContext,
    const String& userName,
    const String& password
)
{
    Uint32 index = address.find (':');
    String host = address.subString (0, index);
    Uint32 portNumber = 0;
    if (index != PEG_NOT_FOUND)
    {
        String portStr = address.subString (index + 1, address.size ());
        sscanf (portStr.getCString (), "%u", &portNumber);
    }
    else
        throw InvalidLocatorException (address);
    _rep->connect (host, portNumber, sslContext, userName, password);
}
#endif

void CIMClient::connectLocal()
{
    _rep->connectLocal();
}

void CIMClient::disconnect()
{
    _rep->disconnect();
}


CIMClass CIMClient::getClass(
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList
)
{
    return _rep->getClass(
        nameSpace,
        className,
        localOnly,
        includeQualifiers,
        includeClassOrigin,
        propertyList);
}

CIMInstance CIMClient::getInstance(
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& instanceName,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList
)
{
    return _rep->getInstance(
        nameSpace,
        instanceName,
        localOnly,
        includeQualifiers,
        includeClassOrigin,
        propertyList);
}

void CIMClient::deleteClass(
    const CIMNamespaceName& nameSpace,
    const CIMName& className
)
{
    _rep->deleteClass(
        nameSpace,
        className);
}

void CIMClient::deleteInstance(
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& instanceName
)
{
    _rep->deleteInstance(
        nameSpace,
        instanceName);
}

void CIMClient::createClass(
    const CIMNamespaceName& nameSpace,
    const CIMClass& newClass
)
{
    _rep->createClass(
        nameSpace,
        newClass);
}

CIMObjectPath CIMClient::createInstance(
    const CIMNamespaceName& nameSpace,
    const CIMInstance& newInstance
)
{
    return _rep->createInstance(
        nameSpace,
        newInstance);
}

void CIMClient::modifyClass(
    const CIMNamespaceName& nameSpace,
    const CIMClass& modifiedClass
)
{
    _rep->modifyClass(
        nameSpace,
        modifiedClass);
}

void CIMClient::modifyInstance(
    const CIMNamespaceName& nameSpace,
    const CIMInstance& modifiedInstance,
    Boolean includeQualifiers,
    const CIMPropertyList& propertyList
)
{
    _rep->modifyInstance(
        nameSpace,
        modifiedInstance,
        includeQualifiers,
        propertyList);
}

Array<CIMClass> CIMClient::enumerateClasses(
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    Boolean deepInheritance,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin
)
{
    return _rep->enumerateClasses(
        nameSpace,
        className,
        deepInheritance,
        localOnly,
        includeQualifiers,
        includeClassOrigin);
}

Array<CIMName> CIMClient::enumerateClassNames(
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    Boolean deepInheritance
)
{
    return _rep->enumerateClassNames(
        nameSpace,
        className,
        deepInheritance);
}

Array<CIMInstance> CIMClient::enumerateInstances(
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    Boolean deepInheritance,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList
)
{
    return _rep->enumerateInstances(
        nameSpace,
        className,
        deepInheritance,
        localOnly,
        includeQualifiers,
        includeClassOrigin,
        propertyList);
}

Array<CIMObjectPath> CIMClient::enumerateInstanceNames(
    const CIMNamespaceName& nameSpace,
    const CIMName& className
)
{
    return _rep->enumerateInstanceNames(
        nameSpace,
        className);
}

Array<CIMObject> CIMClient::execQuery(
    const CIMNamespaceName& nameSpace,
    const String& queryLanguage,
    const String& query
)
{
    return _rep->execQuery(
        nameSpace,
        queryLanguage,
        query);
}

Array<CIMObject> CIMClient::associators(
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& objectName,
    const CIMName& assocClass,
    const CIMName& resultClass,
    const String& role,
    const String& resultRole,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList
)
{
    return _rep->associators(
        nameSpace,
        objectName,
        assocClass,
        resultClass,
        role,
        resultRole,
        includeQualifiers,
        includeClassOrigin,
        propertyList);
}

Array<CIMObjectPath> CIMClient::associatorNames(
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& objectName,
    const CIMName& assocClass,
    const CIMName& resultClass,
    const String& role,
    const String& resultRole
)
{
    return _rep->associatorNames(
        nameSpace,
        objectName,
        assocClass,
        resultClass,
        role,
        resultRole);
}

Array<CIMObject> CIMClient::references(
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& objectName,
    const CIMName& resultClass,
    const String& role,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList
)
{
    return _rep->references(
        nameSpace,
        objectName,
        resultClass,
        role,
        includeQualifiers,
        includeClassOrigin,
        propertyList);
}

Array<CIMObjectPath> CIMClient::referenceNames(
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& objectName,
    const CIMName& resultClass,
    const String& role
)
{
    return _rep->referenceNames(
        nameSpace,
        objectName,
        resultClass,
        role);
}

CIMValue CIMClient::getProperty(
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& instanceName,
    const CIMName& propertyName
)
{
    return _rep->getProperty(
        nameSpace,
        instanceName,
        propertyName);
}

void CIMClient::setProperty(
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& instanceName,
    const CIMName& propertyName,
    const CIMValue& newValue
)
{
    _rep->setProperty(
        nameSpace,
        instanceName,
        propertyName,
        newValue);
}

CIMQualifierDecl CIMClient::getQualifier(
    const CIMNamespaceName& nameSpace,
    const CIMName& qualifierName
)
{
    return _rep->getQualifier(
        nameSpace,
        qualifierName);
}

void CIMClient::setQualifier(
    const CIMNamespaceName& nameSpace,
    const CIMQualifierDecl& qualifierDeclaration
)
{
    _rep->setQualifier(
        nameSpace,
        qualifierDeclaration);
}

void CIMClient::deleteQualifier(
    const CIMNamespaceName& nameSpace,
    const CIMName& qualifierName
)
{
    _rep->deleteQualifier(
        nameSpace,
        qualifierName);
}

Array<CIMQualifierDecl> CIMClient::enumerateQualifiers(
    const CIMNamespaceName& nameSpace
)
{
    return _rep->enumerateQualifiers(
        nameSpace);
}

CIMValue CIMClient::invokeMethod(
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& instanceName,
    const CIMName& methodName,
    const Array<CIMParamValue>& inParameters,
    Array<CIMParamValue>& outParameters
)
{
    return _rep->invokeMethod(
        nameSpace,
        instanceName,
        methodName,
        inParameters,
        outParameters);
}

PEGASUS_NAMESPACE_END
