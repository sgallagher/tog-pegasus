//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 BMC Software, Hewlett-Packard Company, IBM,
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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By: Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
//              Yi Zhou, Hewlett-Packard Company (yi_zhou@hp.com)
//              Nag Boranna, Hewlett-Packard Company (nagaraja_boranna@hp.com)
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Client_h
#define Pegasus_Client_h

#include <fstream>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Monitor.h>
#include <Pegasus/Common/HTTPConnector.h>
#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Common/CIMObject.h>
#include <Pegasus/Common/CIMNamedInstance.h>
#include <Pegasus/Common/CIMPropertyList.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Client/ClientAuthenticator.h>
#include <Pegasus/Client/Linkage.h>

//
// Wbem service name
//
#define WBEM_SERVICE_NAME          "wbem-http"

PEGASUS_NAMESPACE_BEGIN

class Monitor;
class CIMOperationResponseDecoder;
class CIMOperationRequestEncoder;

//
// Wbem default local port number
//
static const Uint32 WBEM_DEFAULT_PORT =  5988;


/** This class provides the interface that a client uses to communicate
    with a CIMOM.
*/
class PEGASUS_CLIENT_LINKAGE CIMClient : public MessageQueue
{
public:

    enum { DEFAULT_TIMEOUT_MILLISECONDS = 20000 };

    /** Constructor for a CIM Client object.
    @param monitor Defines the monitor object to be used with this
    CIMClient object.
    @param httpConnector defines the connector object to use with
    this CIMClient object. The connector defines TBD.
    @param timeOutMilliseconds Defines the number of milliseconds
    of inactivity before the connection will timeout
    
    <PRE>
        Monitor* monitor = new Monitor;
        HTTPConnector* connector;
        connector = new HTTPConnector(monitor);

        CIMClient client(monitor, connector, 60 * 1000);

        char * connection = connectionList[i].allocateCString();
        cout << "connecting to " << connection << endl;
        client.connect(connection);
    
    </PRE>
    @exception TBD
    */
    CIMClient(
	Monitor* monitor,
	HTTPConnector* httpConnector,
	Uint32 timeOutMilliseconds = DEFAULT_TIMEOUT_MILLISECONDS);

    ///
    ~CIMClient();

    /**  TBD
    */
    virtual void handleEnqueue();

    /** Returns the queue name. */
    virtual const char* getQueueName() const;

    /** TBD
    */
    Uint32 getTimeOut() const
    {
	return _timeOutMilliseconds;
    }

    /** Sets the timeout in milliseconds for the CIMClient.
    */
    void setTimeOut(Uint32 timeOutMilliseconds)
    {
	_timeOutMilliseconds = timeOutMilliseconds;
    }

    /** connect - Creates an HTTP connection with the server
        defined by the URL in address.
        @param address - String defining the URL of the server
        to which the client should connect
        @param userName - String containing the name of the user
        the client is connecting as.
        @param password - String containing the password of the user
        the client is connecting as.
        @return - No return defined. Failure to connect throws an exception
        @exception AlreadyConnected if connection already established
        @exception InvalidLocator
        @exception CannotCreateSocket
        @exception CannotConnect
        @exception UnexpectedFailure
        <PRE>
            TBD
        </PRE>
    */
    void connect(
        const String& address,
        const String& userName = String::EMPTY,
        const String& password = String::EMPTY);

    /** connectLocal - Creates connection to the server for
        Local clients. The connectLocal connects to the CIM server
        running on the local system in the default location.  The
        connection is automatically authenticated for the current
        user.
        @return - No return defined. Failure to connect throws an exception.
        @SeeAlso connect - The exceptions are defined in connect.
    */
    void connectLocal();

    /** disconnect - Closes the connection with the server if the connection
        was open, simply returns if the connection was not open. Clients are
        expected to use this method to close the open connection before
        opening a new connection.
        @return - No return defined.
    */
    void disconnect();


    /** The <TT>getClass</TT> method executes a CIM operation that returns
	a single CIM Class from the
	target Namespace where the ClassName input parameter defines the name of
	the class to be retrieved.

	@param nameSpace The <TT>nameSpace</TT> parameter is a string that
	defines the target Namespace.
	See defintion of \URL[Namespace]{DefinitionofTerms.html#NAMESPACE}.

	@param className The <TT>className</TT> input parameter defines the name
	of the Class to be retrieved.

	@param localOnly If the <TT>localOnly</TT> input parameter is true, this
	specifies that only CIM Elements (properties, methods and qualifiers)
	overriden within the definition of the Class are returned.  If false,
	all elements are returned.  This parameter therefore effects a CIM
	Server-side mechanism to filter certain elements of the returned object
	based on whether or not they have been propagated from the parent Class
	(as defined by the PROPAGATED attribute).

	@param includeQualifiers If the <TT>includeQualifiers</TT> input
	parameter is true, this specifies that all Qualifiers for that Class
	(including Qualifiers on the Class and on any returned Properties,
	Methods or CIMMethod Parameters) MUST be included as <QUALIFIER>
	elements in the response.  If false no <QUALIFIER> elements are
	present in the returned Class.

	@param includeClassOrigin If the <TT>includeClassOrigin</TT> input
	parameter is true, this specifies that the CLASSORIGIN attribute MUST be
	present on all appropriate elements in the returned Class. If false, no
	CLASSORIGIN attributes are present in the returned Class.

	@param propertyList If the <TT>propertyList</TT> input parameter is not
	NULL, the members of the array define one or more CIMProperty names. The
	returned Class MUST NOT include elements for any Properties missing from
	this list. Note that if LocalOnly is specified as true this acts as an
	additional filter on the set of Properties returned (for example, if
	CIMProperty A is included in the PropertyList but LocalOnly is set to
	true and A is not local to the requested Class, then it will not be
	included in the response). If the PropertyList input parameter is an
	empty array this signifies that no Properties are included in the
	response. If the PropertyList input parameter is NULL this specifies
	that all Properties (subject to the conditions expressed by the other
	parameters) are included in the response.

	If the <TT>propertyList</TT> contains duplicate elements, the Server
	MUST ignore the duplicates but otherwise process the request normally.
	If the PropertyList contains elements which are invalid CIMProperty
	names for the target Class, the Server MUST ignore such entries but
	otherwise process the request normally.

	@return If successful, the return value is a single CIM Class.

	If unsuccessful, one of the following status codes MUST be returned by
	this method, where the first applicable error in the list (starting with
	the first element of the list, and working down) is the error returned.
	Any additional method-specific interpretation of the error in is given
	in parentheses.
	<UL>
		<LI>CIM_ERR_ACCESS_DENIED
		<LI>CIM_ERR_INVALID_NAMESPACE
		<LI>CIM_ERR_INVALID_PARAMETER (including missing,
		duplicate,unrecognized or otherwise incorrect parameters)
		<LI>CIM_ERR_NOT_FOUND (the request CIM Class does not exist in
		the specified namespace)
		<LI>CIM_ERR_FAILED (some other unspecified error occurred)</LI>
	</UL>
    */
    virtual CIMClass getClass(
	const String& nameSpace,
	const String& className,
	Boolean localOnly = true,
	Boolean includeQualifiers = true,
	Boolean includeClassOrigin = false,
	const CIMPropertyList& propertyList = CIMPropertyList());

    /** ATTN TBD
    */
    virtual CIMInstance getInstance(
	const String& nameSpace,
	const CIMReference& instanceName,
	Boolean localOnly = true,
	Boolean includeQualifiers = false,
	Boolean includeClassOrigin = false,
	const CIMPropertyList& propertyList = CIMPropertyList());

    /** ATTN TBD
    */
    virtual void deleteClass(
	const String& nameSpace,
	const String& className);

    /** ATTN TBD
    */ 
    virtual void deleteInstance(
	const String& nameSpace,
	const CIMReference& instanceName);

    /** ATTN TBD
    */
    virtual void createClass(
	const String& nameSpace,
	const CIMClass& newClass);

    virtual CIMReference createInstance(
	const String& nameSpace,
	const CIMInstance& newInstance);

    ///
    virtual void modifyClass(
	const String& nameSpace,
	const CIMClass& modifiedClass);

    ///
    virtual void modifyInstance(
	const String& nameSpace,
	const CIMNamedInstance& modifiedInstance,
	Boolean includeQualifiers = true,
	const CIMPropertyList& propertyList = CIMPropertyList());

    ///
    virtual Array<CIMClass> enumerateClasses(
	const String& nameSpace,
	const String& className = String::EMPTY,
	Boolean deepInheritance = false,
	Boolean localOnly = true,
	Boolean includeQualifiers = true,
	Boolean includeClassOrigin = false);

    ///
    virtual Array<String> enumerateClassNames(
	const String& nameSpace,
	const String& className = String::EMPTY,
	Boolean deepInheritance = false);

    ///
    virtual Array<CIMNamedInstance> enumerateInstances(
	const String& nameSpace,
	const String& className,
	Boolean deepInheritance = true,
	Boolean localOnly = true,
	Boolean includeQualifiers = false,
	Boolean includeClassOrigin = false,
	const CIMPropertyList& propertyList = CIMPropertyList());

    ///
    virtual Array<CIMReference> enumerateInstanceNames(
	const String& nameSpace,
	const String& className);

    /// ATTN: should return Array<CIMObject>
    virtual Array<CIMInstance> execQuery(
	const String& queryLanguage,
	const String& query) ;

    ///
    virtual Array<CIMObjectWithPath> associators(
	const String& nameSpace,
	const CIMReference& objectName,
	const String& assocClass = String::EMPTY,
	const String& resultClass = String::EMPTY,
	const String& role = String::EMPTY,
	const String& resultRole = String::EMPTY,
	Boolean includeQualifiers = false,
	Boolean includeClassOrigin = false,
	const CIMPropertyList& propertyList = CIMPropertyList());

    ///
    virtual Array<CIMReference> associatorNames(
	const String& nameSpace,
	const CIMReference& objectName,
	const String& assocClass = String::EMPTY,
	const String& resultClass = String::EMPTY,
	const String& role = String::EMPTY,
	const String& resultRole = String::EMPTY);
    ///
    virtual Array<CIMObjectWithPath> references(
	const String& nameSpace,
	const CIMReference& objectName,
	const String& resultClass = String::EMPTY,
	const String& role = String::EMPTY,
	Boolean includeQualifiers = false,
	Boolean includeClassOrigin = false,
	const CIMPropertyList& propertyList = CIMPropertyList());

    ///
    virtual Array<CIMReference> referenceNames(
	const String& nameSpace,
	const CIMReference& objectName,
	const String& resultClass = String::EMPTY,
	const String& role = String::EMPTY);

    ///
    virtual CIMValue getProperty(
	const String& nameSpace,
	const CIMReference& instanceName,
	const String& propertyName);

    ////
    virtual void setProperty(
	const String& nameSpace,
	const CIMReference& instanceName,
	const String& propertyName,
	const CIMValue& newValue = CIMValue());

    ///
    virtual CIMQualifierDecl getQualifier(
	const String& nameSpace,
	const String& qualifierName);

    ///
    virtual void setQualifier(
	const String& nameSpace,
	const CIMQualifierDecl& qualifierDeclaration);

    ///
    virtual void deleteQualifier(
	const String& nameSpace,
	const String& qualifierName);

    ///
    virtual Array<CIMQualifierDecl> enumerateQualifiers(
	const String& nameSpace);

    ///
    virtual CIMValue invokeMethod(
	const String& nameSpace,
	const CIMReference& instanceName,
	const String& methodName,
	const Array<CIMParamValue>& inParameters,
	Array<CIMParamValue>& outParameters);

private:

    Message* _waitForResponse(
	const Uint32 messageType,
	const String& messageId,
	const Uint32 timeOutMilliseconds = DEFAULT_TIMEOUT_MILLISECONDS);

    void _checkError(const CIMResponseMessage* responseMessage);

    String _getLocalHostName();

    Monitor* _monitor;
    HTTPConnector* _httpConnector;
    HTTPConnection* _httpConnection;
    Uint32 _timeOutMilliseconds;
    Boolean _connected;
    CIMOperationResponseDecoder* _responseDecoder;
    CIMOperationRequestEncoder* _requestEncoder;
    ClientAuthenticator* _authenticator;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Client_h */
