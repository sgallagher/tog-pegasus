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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By: Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
//              Yi Zhou, Hewlett-Packard Company (yi_zhou@hp.com)
//              Nag Boranna, Hewlett-Packard Company (nagaraja_boranna@hp.com)
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                  (carolann_graves@hp.com)
//              Jair Santos, Hewlett-Packard Company (jair.santos@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Client_h
#define Pegasus_Client_h

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
#include <Pegasus/Common/AcceptLanguages.h> //l10n

PEGASUS_NAMESPACE_BEGIN

class CIMClientInterface;


/** This class provides the interface that a client uses to communicate
with a CIM server.  This class constructs a CIM client on the local host 
(default) or the specified host.  This class invokes the CIM object 
manager for this client to perform operations such as, adding, modifying, 
or deleting a CIM class, CIM instance, and CIM qualifier type in a 
namespace.  

A client connects to a CIM Object Manager to establish an initial 
connection when it needs to perform CIM operations.  


*/
class PEGASUS_CLIENT_LINKAGE CIMClient
{
public:

    /** Constructs a CIM Client object with null values (default constructor).
    */
    CIMClient();

    /** Destructor for a CIM Client object.
	*/
    ~CIMClient();

    /** Describe method here.
	*/
    Uint32 getTimeout() const;

    /** Sets the timeout in milliseconds for the CIMClient.
        @param timeoutMilliseconds Defines the number of milliseconds the
        CIMClient will wait for a response to an outstanding request.  If a
        request times out, the connection gets reset (disconnected and
        reconnected).  Default is 20 seconds (20000 milliseconds).
    */
    void setTimeout(Uint32 timeoutMilliseconds);

    /** Creates an HTTP connection with the CIM server
        defined by the host and portNumber.
        @param host Specifies the server as a String that the client connects to.
        @param portNumber Uint32 defines the port number for the server and client to use.
        @param userName String that defines the name of the user that the client is connecting as.
        @param password String that contains the password of the user the client is connecting as.
        @exception AlreadyConnectedException
            If a connection between the client and CIM server is already established.
        @exception InvalidLocatorException
            If the specified address is improperly formed.
        @exception CannotCreateSocketException
            If a socket cannot be created.
        @exception CannotConnectException
            If the socket connection fails.
        @exception CIMClientConnectionException
            If any other failure occurs.
        <PRE>
            CIMClient client;
            client.connect("localhost", 5988, "guest", "guest");
        </PRE>
    */
    void connect(
        const String& host,
        const Uint32 portNumber,
        const String& userName,
        const String& password
    );

    /** Creates an HTTP connection with the server
        defined by the URL address.
        @param host Specifies the server that the client connects to.  The server is specified as a String.
        @param portNumber Uint32 defines the port number for the server and client to use.
        @param sslContext Specifies the SSL context to use for this connection.
        @param userName String that defines the name of the user that 
        the client is connecting as.
        @param password String containing the password of the user
        the client is connecting as.
        @exception AlreadyConnectedException
            If a connection between the client and CIM server is already established.
        @exception InvalidLocatorException
            If the specified address is improperly formed.
        @exception CannotCreateSocketException
            If a socket cannot be created.
        @exception CannotConnectException
            If the socket connection fails.
        @exception CIMClientConnectionException
            If any other failure occurs.
    */
    void connect(
        const String& host,
        const Uint32 portNumber,
        const SSLContext& sslContext,
        const String& userName,
        const String& password
    );

#ifndef PEGASUS_REMOVE_DEPRECATED
    /** connect - Creates an HTTP connection with the server
        defined by the URL in address.
        @param address - Specifies the server as a String that the client connects to.  
        @param userName - String that defines the name of the user that 
        the client is connecting as.
        @param password - String containing the password of the user the client is connecting as.
        @exception AlreadyConnectedException
            If a connection between the client and the CIM server is already established.
        @exception InvalidLocatorException
            If the specified address is improperly formed.
        @exception CannotCreateSocketException
            If a socket cannot be created.
        @exception CannotConnectException
            If the socket connection fails.
        @exception CIMClientConnectionException
            If any other failure occurs.
        <PRE>
            CIMClient client;
            client.connect("localhost:5988");
        </PRE>
    */
    void connect(
        const String& address,
        const String& userName = String::EMPTY,
        const String& password = String::EMPTY
    );

    /** connect - Creates an HTTP connection with the server defined by the URL in address.
        @param address - Specifies the server as a String that the client connects to.  
        @param sslContext - The SSL context to use for this connection.
        @param userName - String that defines the name of the user that 
        the client is connecting as.
        @param password - String containing the password of the user
        the client is connecting as.
        @exception AlreadyConnectedException
            If a connection between the client and CIM server is already established.
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
        const SSLContext& sslContext,
        const String& userName = String::EMPTY,
        const String& password = String::EMPTY
    );
#endif

    /** Creates connection to the server for
        Local clients. The connectLocal connects to the CIM server
        running on the local system in the default location.  The
        connection is automatically authenticated for the current
        user.
        @See connect - The exceptions are defined in connect.
    */
    void connectLocal();

    /** If the connection to the server was open, the connection with the server closes;
 otherwise, it returns. Before opening a new connection, clients should use this method
to close the open connection.
    */
    void disconnect();

#ifdef PEGASUS_USE_EXPERIMENTAL_INTERFACES
// l10n start
    /** Sets the accept languages that will be used on the next request.
        Accept languages are the preferred languages that are to be
        returned on the response to the next request.
    */	
	void setRequestAcceptLanguages(AcceptLanguages& langs);

    /** Gets the accept languages that will be used on the next request.
        Accept languages are the preferred languages that are to be
        returned on the response to the next request.
    */
	AcceptLanguages getRequestAcceptLanguages() const;
	
    /** Sets the content languages that will be used on the next request.
        These content languages are the languages of the CIM objects that will
        sent on the next request.
    */  	
	void setRequestContentLanguages(ContentLanguages& langs);

    /** Gets the content languages that will be used on the next request.
     * These content languages are the languages of the CIM objects that will
     * sent on the next request.
    */    
	ContentLanguages getRequestContentLanguages() const;
	
    /** Gets the content languages of the last response.
     * These content languages are the languages of the CIM objects, or 
     * CIM exceptions, that were returned on the last response..
    */    	
	ContentLanguages getResponseContentLanguages() const;
	
	/**
	 * 
	 */
	void setRequestDefaultLanguages();
// l10n end
#endif // PEGASUS_USE_EXPERIMENTAL_INTERFACES	


    /** The <TT>getClass</TT> method executes a CIM operation that returns
    	a single CIM Class from the
    	target Namespace where the ClassName input parameter defines the name of
    	the class to be retrieved.
    
    	@param nameSpace The <TT>nameSpace</TT> parameter is a string that
    	defines the target Namespace.
    	See definition of \URL[Namespace]{DefinitionofTerms.html#NAMESPACE}.
    
    	@param className The <TT>className</TT> input parameter defines the name
    	of the Class to be retrieved.
    
    	@param localOnly If the <TT>localOnly</TT> input parameter is true, this
    	specifies that only CIM Elements (properties, methods and qualifiers)
    	overridden within the definition of the Class are returned.  If false,
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
    	Any additional method-specific interpretation of the error is given
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
    CIMClass getClass(
	const CIMNamespaceName& nameSpace,
	const CIMName& className,
	Boolean localOnly = true,
	Boolean includeQualifiers = true,
	Boolean includeClassOrigin = false,
	const CIMPropertyList& propertyList = CIMPropertyList()
    );

    /** Gets the CIM instance for the specified CIM object path. 
    	@param nameSpace Specifies the name of the CIMNamespaceName instance of 
    	the CIMNamespaceName object.
        
    	@param instanceName CIM object path that identifies this CIM instance. 
    	This must include all of the keys. 
    	
        @param localOnly If true, only properties and qualifiers overridden 
    	or defined in the returned Instance are included in the response. 
    	If false, all elements of the returned Instance are returned. 
    	@param includeQualifiers If true, all Qualifiers for each Object 
    	(including Qualifiers on the Object and on any returned Properties) 
    	MUST be included. If false no Qualifiers are present in the returned Object. 
    	
        @param includeClassOrigin If true, CLASSORIGIN attribute MUST be 
    	present on all appropriate elements in each returned Object. If false, 
    	no CLASSORIGIN attributes are present in each returned Object. The CLASSORIGIN
        attribute is defined in the DMTF's Specification for the Representation of CIM
        in XML. CLASSORIGIN is an XML tag identifying the following text as a class name.
        It is attached to a property or method (when specified in XML), to indicate the
        class where that property or method is first defined. Where the same property
        name is locally defined in another superclass or subclass, the Server will
        return the value for the property in the lowest subclass. 
    	
        @param propertyList If the PropertyList input parameter is not NULL, the
        members of the array define one or more Property names. Each returned Object
        MUST NOT include elements for any Properties missing from this list.
        Note that if LocalOnly is specified as true this acts as an additional
        filter on the set of Properties returned (for example, if Property A is
        included in the PropertyList but LocalOnly is set to true and A is not local
        to a returned Instance, then it will not be included in that Instance).
        If the PropertyList input parameter is an empty array this signifies that
        no Properties are included in each returned Object. If the PropertyList
        input parameter is NULL this specifies that all Properties (subject to
        the conditions expressed by the other parameters) are included in each
        returned Object. If the PropertyList contains duplicate elements, the
        Server ignores the duplicates but otherwise process the request normally.
        If the PropertyList contains elements which are invalid Property names for
        any target Object, the Server ignores such entries but otherwise process
        the request normally. 
    	
        @return The CIM instance identified by the CIMObjectPath. 
	*/
	
    CIMInstance getInstance(
	const CIMNamespaceName& nameSpace,
	const CIMObjectPath& instanceName,
	Boolean localOnly = true,
	Boolean includeQualifiers = false,
	Boolean includeClassOrigin = false,
	const CIMPropertyList& propertyList = CIMPropertyList()
    );

    /** Deletes the specified class of the CIMNamespaceName object.
	*/
    void deleteClass(
	const CIMNamespaceName& nameSpace,
	const CIMName& className
    );

    /**Deletes the specified instance of the CIMNamespaceName object.
	*/
    void deleteInstance(
	const CIMNamespaceName& nameSpace,
	const CIMObjectPath& instanceName
    );

    /**Creates the specified class of the CIMNamespaceName object.
	*/
    void createClass(
	const CIMNamespaceName& nameSpace,
	const CIMClass& newClass
    );

    /**Creates the specified instance of the CIMNamespaceName object.
	*/
    CIMObjectPath createInstance(
	const CIMNamespaceName& nameSpace,
	const CIMInstance& newInstance
    );

    /**Modifies the specified class by adding the values and parameters of CIMClass object to
    the modified class.
	*/
    void modifyClass(
	const CIMNamespaceName& nameSpace,
	const CIMClass& modifiedClass
    );

    /**Modifies the specified instance by adding the values and parameters of CIMClass
    object to the modified instance.
	*/
    void modifyInstance(
	const CIMNamespaceName& nameSpace,
	const CIMInstance& modifiedInstance,
	Boolean includeQualifiers = true,
	const CIMPropertyList& propertyList = CIMPropertyList()
    );

    /** Insert documentation here.
	*/
    Array<CIMClass> enumerateClasses(
	const CIMNamespaceName& nameSpace,
	const CIMName& className = CIMName(),
	Boolean deepInheritance = false,
	Boolean localOnly = true,
	Boolean includeQualifiers = true,
	Boolean includeClassOrigin = false
    );

    /** Insert documentation here.
	@param nameSpace
	@param className
	@param boolean deepInheritance
	*/
    Array<CIMName> enumerateClassNames(
	const CIMNamespaceName& nameSpace,
	const CIMName& className = CIMName(),
	Boolean deepInheritance = false
    );

    /** Insert documentation here.
	@param nameSpace Insert documentation here.
	@param className Insert documentation here.
	@param deepInheritance Insert documentation here.
	@param localOnly Insert documentation here.
	@param includeQualifiers Insert documentation here.
	@param includeClassOrigin Insert documentation here.
	@param propertyList Insert documentation here.
	*/
    Array<CIMInstance> enumerateInstances(
	const CIMNamespaceName& nameSpace,
	const CIMName& className,
	Boolean deepInheritance = true,
	Boolean localOnly = true,
	Boolean includeQualifiers = false,
	Boolean includeClassOrigin = false,
	const CIMPropertyList& propertyList = CIMPropertyList()
    );

     /** Insert documentation here.
	@param nameSpace Insert documentation here.
	@param className Insert documentation here.
	*/
    Array<CIMObjectPath> enumerateInstanceNames(
	const CIMNamespaceName& nameSpace,
	const CIMName& className
    );

     /** Insert documentation here.
	@param nameSpace Insert documentation here.
	@param queryLanguage Insert documentation here.
	@param query Insert documentation here.
	*/
    Array<CIMObject> execQuery(
	const CIMNamespaceName& nameSpace,
	const String& queryLanguage,
	const String& query
    );

     /** Insert documentation here.
	@param nameSpace Insert documentation here.
	@param objectName Insert documentation here.
	@param assocClass Insert documentation here.
	@param resultClass Insert documentation here.
	@param role Insert documentation here.
	@param resultRole Insert documentation here.
	@param includeQualifiers Insert documentation here.
	@param includeClassOrigin Insert documentation here.
	@param propertyList Insert documentation here.
	*/
    Array<CIMObject> associators(
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

      /** Insert documentation here.
	@param nameSpace Insert documentation here.
	@param objectName Insert documentation here.
	@param assocClass Insert documentation here.
	@param resultClass Insert documentation here.
	@param role Insert documentation here.
	@param resultRole Insert documentation here.
	*/
    Array<CIMObjectPath> associatorNames(
	const CIMNamespaceName& nameSpace,
	const CIMObjectPath& objectName,
	const CIMName& assocClass = CIMName(),
	const CIMName& resultClass = CIMName(),
	const String& role = String::EMPTY,
	const String& resultRole = String::EMPTY
    );

    /** Insert documentation here.
	@param nameSpace Insert documentation here.
	@param objectName Insert documentation here.
	@param assocClass Insert documentation here.
	@param resultClass Insert documentation here.
	@param role Insert documentation here.
	@param includeQualifiers Insert documentation here.
	@param includeClassOrigin Insert documentation here.
	@param propertyList Insert documentation here.
	*/
    Array<CIMObject> references(
	const CIMNamespaceName& nameSpace,
	const CIMObjectPath& objectName,
	const CIMName& resultClass = CIMName(),
	const String& role = String::EMPTY,
	Boolean includeQualifiers = false,
	Boolean includeClassOrigin = false,
	const CIMPropertyList& propertyList = CIMPropertyList()
    );

    /** Insert documentation here.
	@param nameSpace Insert documentation here.
	@param objectName Insert documentation here.
	@param resultClass Insert documentation here.
	@param role Insert documentation here.
	*/
    Array<CIMObjectPath> referenceNames(
	const CIMNamespaceName& nameSpace,
	const CIMObjectPath& objectName,
	const CIMName& resultClass = CIMName(),
	const String& role = String::EMPTY
    );

    /** Insert documentation here.
	@param nameSpace Insert documentation here.
	@param instanceName Insert documentation here.
	@param propertyName Insert documentation here.
	*/
    CIMValue getProperty(
	const CIMNamespaceName& nameSpace,
	const CIMObjectPath& instanceName,
	const CIMName& propertyName
    );

    /** Insert documentation here.
	@param nameSpace Insert documentation here.
	@param instanceName Insert documentation here.
	@param propertyName Insert documentation here.
	@param newValue Insert documentation here.
	*/
    void setProperty(
	const CIMNamespaceName& nameSpace,
	const CIMObjectPath& instanceName,
	const CIMName& propertyName,
	const CIMValue& newValue = CIMValue()
    );

    /** Insert documentation here.
	@param nameSpace Insert documentation here.
	@param qualifierName Insert documentation here.
	*/
    CIMQualifierDecl getQualifier(
	const CIMNamespaceName& nameSpace,
	const CIMName& qualifierName
    );

    /** Insert documentation here.
	@param nameSpace Insert documentation here.
	@param qualifierDeclaration Insert documentation here.
	*/
    void setQualifier(
	const CIMNamespaceName& nameSpace,
	const CIMQualifierDecl& qualifierDeclaration
    );

    /** Insert documentation here.
	@param nameSpace Insert documentation here.
	@param qualifierName Insert documentation here.
	*/
    void deleteQualifier(
	const CIMNamespaceName& nameSpace,
	const CIMName& qualifierName
    );

    /** Insert documentation here.
	@param nameSpace Insert documentation here.
	*/
    Array<CIMQualifierDecl> enumerateQualifiers(
	const CIMNamespaceName& nameSpace
    );

    /** Insert documentation here.
	@param nameSpace Insert documentation here.
	@param instanceName Insert documentation here.
	@param methodName Insert documentation here.
	@param inParameters Insert documentation here.
	@param outParameters Insert documentation here.
	*/
    CIMValue invokeMethod(
	const CIMNamespaceName& nameSpace,
	const CIMObjectPath& instanceName,
	const CIMName& methodName,
	const Array<CIMParamValue>& inParameters,
	Array<CIMParamValue>& outParameters
    );

private:

    CIMClientInterface* _rep;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Client_h */
