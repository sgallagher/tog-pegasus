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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By: Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
//              Yi Zhou, Hewlett-Packard Company (yi_zhou@hp.com)
//              Nag Boranna, Hewlett-Packard Company (nagaraja_boranna@hp.com)
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                  (carolann_graves@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Client_h
#define Pegasus_Client_h

#include <fstream>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/CIMName.h>
#include <Pegasus/Common/SSLContext.h>
#include <Pegasus/Common/CIMObject.h>
#include <Pegasus/Common/CIMClass.h>
#include <Pegasus/Common/CIMInstance.h>
#include <Pegasus/Common/CIMObjectPath.h>
#include <Pegasus/Common/CIMValue.h>
#include <Pegasus/Common/CIMParamValue.h>
#include <Pegasus/Common/CIMPropertyList.h>
#include <Pegasus/Common/CIMQualifierDecl.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Client/CIMClientException.h>
#include <Pegasus/Client/Linkage.h>

PEGASUS_NAMESPACE_BEGIN


class CIMClientRep;

/** This class provides the interface that a client uses to communicate
    with a CIMOM.
*/
class PEGASUS_CLIENT_LINKAGE CIMClient
{
public:

    enum { DEFAULT_TIMEOUT_MILLISECONDS = 20000 };

    /** Constructor for a CIM Client object.
    @param timeOutMilliseconds Defines the number of milliseconds the
    CIMClient will wait for a response to an outstanding request.  If a
    request times out, the connection gets reset (disconnected and
    reconnected).
    
    <PRE>
        CIMClient client(60 * 1000);

        char * connection = connectionList[i].allocateCString();
        cout << "connecting to " << connection << endl;
        client.connect(connection);
    </PRE>
    @exception ATTN-TBD
    */
    CIMClient(Uint32 timeOutMilliseconds = DEFAULT_TIMEOUT_MILLISECONDS);

    ///
    virtual ~CIMClient();

    /** TBD
    */
    Uint32 getTimeOut() const;

    /** Sets the timeout in milliseconds for the CIMClient.
    */
    void setTimeOut(Uint32 timeOutMilliseconds);

    /** connect - Creates an HTTP connection with the server
        defined by the URL in address.
        @param address - String defining the URL of the server
        to which the client should connect
        @param userName - String containing the name of the user
        the client is connecting as.
        @param password - String containing the password of the user
        the client is connecting as.
        @return - No return defined. Failure to connect throws an exception
        @exception AlreadyConnectedException
            If a connection has already been established.
        @exception InvalidLocatorException
            If the specified address is improperly formed.
        @exception CannotCreateSocketException
            If a socket cannot be created.
        @exception CannotConnectException
            If the socket connection fails.
        @exception CIMClientConnectionException
            If any other failure occurs.
        <PRE>
            TBD
        </PRE>
    */
    void connect(
        const String& address,
        const String& userName = String::EMPTY,
        const String& password = String::EMPTY
    );

    /** connect - Creates an HTTP connection with the server
        defined by the URL in address.
        @param address - String defining the URL of the server
        to which the client should connect
        @param sslContext - The SSL context to use for this connection
        @param userName - String containing the name of the user
        the client is connecting as.
        @param password - String containing the password of the user
        the client is connecting as.
        @return - No return defined. Failure to connect throws an exception
        @exception AlreadyConnectedException
            If a connection has already been established.
        @exception InvalidLocatorException
            If the specified address is improperly formed.
        @exception CannotCreateSocketException
            If a socket cannot be created.
        @exception CannotConnectException
            If the socket connection fails.
        @exception CIMClientConnectionException
            If any other failure occurs.
        <PRE>
            TBD
        </PRE>
    */
    void connect(
        const String& address,
        SSLContext* sslContext,
        const String& userName = String::EMPTY,
        const String& password = String::EMPTY
    );

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
	const CIMNamespaceName& nameSpace,
	const CIMName& className,
	Boolean localOnly = true,
	Boolean includeQualifiers = true,
	Boolean includeClassOrigin = false,
	const CIMPropertyList& propertyList = CIMPropertyList()
    );

    /** ATTN TBD
    */
    virtual CIMInstance getInstance(
	const CIMNamespaceName& nameSpace,
	const CIMObjectPath& instanceName,
	Boolean localOnly = true,
	Boolean includeQualifiers = false,
	Boolean includeClassOrigin = false,
	const CIMPropertyList& propertyList = CIMPropertyList()
    );

    /** ATTN TBD
    */
    virtual void deleteClass(
	const CIMNamespaceName& nameSpace,
	const CIMName& className
    );

    /** ATTN TBD
    */ 
    virtual void deleteInstance(
	const CIMNamespaceName& nameSpace,
	const CIMObjectPath& instanceName
    );

    /** ATTN TBD
    */
    virtual void createClass(
	const CIMNamespaceName& nameSpace,
	const CIMClass& newClass
    );

    virtual CIMObjectPath createInstance(
	const CIMNamespaceName& nameSpace,
	const CIMInstance& newInstance
    );

    ///
    virtual void modifyClass(
	const CIMNamespaceName& nameSpace,
	const CIMClass& modifiedClass
    );

    ///
    virtual void modifyInstance(
	const CIMNamespaceName& nameSpace,
	const CIMInstance& modifiedInstance,
	Boolean includeQualifiers = true,
	const CIMPropertyList& propertyList = CIMPropertyList()
    );

    ///
    virtual Array<CIMClass> enumerateClasses(
	const CIMNamespaceName& nameSpace,
	const CIMName& className = CIMName(),
	Boolean deepInheritance = false,
	Boolean localOnly = true,
	Boolean includeQualifiers = true,
	Boolean includeClassOrigin = false
    );

    ///
    virtual Array<CIMName> enumerateClassNames(
	const CIMNamespaceName& nameSpace,
	const CIMName& className = CIMName(),
	Boolean deepInheritance = false
    );

    ///
    virtual Array<CIMInstance> enumerateInstances(
	const CIMNamespaceName& nameSpace,
	const CIMName& className,
	Boolean deepInheritance = true,
	Boolean localOnly = true,
	Boolean includeQualifiers = false,
	Boolean includeClassOrigin = false,
	const CIMPropertyList& propertyList = CIMPropertyList()
    );

    ///
    virtual Array<CIMObjectPath> enumerateInstanceNames(
	const CIMNamespaceName& nameSpace,
	const CIMName& className
    );

    ///
    virtual Array<CIMObject> execQuery(
	const CIMNamespaceName& nameSpace,
	const String& queryLanguage,
	const String& query
    );

    ///
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

    ///
    virtual Array<CIMObjectPath> associatorNames(
	const CIMNamespaceName& nameSpace,
	const CIMObjectPath& objectName,
	const CIMName& assocClass = CIMName(),
	const CIMName& resultClass = CIMName(),
	const String& role = String::EMPTY,
	const String& resultRole = String::EMPTY
    );

    ///
    virtual Array<CIMObject> references(
	const CIMNamespaceName& nameSpace,
	const CIMObjectPath& objectName,
	const CIMName& resultClass = CIMName(),
	const String& role = String::EMPTY,
	Boolean includeQualifiers = false,
	Boolean includeClassOrigin = false,
	const CIMPropertyList& propertyList = CIMPropertyList()
    );

    ///
    virtual Array<CIMObjectPath> referenceNames(
	const CIMNamespaceName& nameSpace,
	const CIMObjectPath& objectName,
	const CIMName& resultClass = CIMName(),
	const String& role = String::EMPTY
    );

    ///
    virtual CIMValue getProperty(
	const CIMNamespaceName& nameSpace,
	const CIMObjectPath& instanceName,
	const CIMName& propertyName
    );

    ////
    virtual void setProperty(
	const CIMNamespaceName& nameSpace,
	const CIMObjectPath& instanceName,
	const CIMName& propertyName,
	const CIMValue& newValue = CIMValue()
    );

    ///
    virtual CIMQualifierDecl getQualifier(
	const CIMNamespaceName& nameSpace,
	const CIMName& qualifierName
    );

    ///
    virtual void setQualifier(
	const CIMNamespaceName& nameSpace,
	const CIMQualifierDecl& qualifierDeclaration
    );

    ///
    virtual void deleteQualifier(
	const CIMNamespaceName& nameSpace,
	const CIMName& qualifierName
    );

    ///
    virtual Array<CIMQualifierDecl> enumerateQualifiers(
	const CIMNamespaceName& nameSpace
    );

    ///
    virtual CIMValue invokeMethod(
	const CIMNamespaceName& nameSpace,
	const CIMObjectPath& instanceName,
	const CIMName& methodName,
	const Array<CIMParamValue>& inParameters,
	Array<CIMParamValue>& outParameters
    );

private:

    CIMClientRep* _rep;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Client_h */
