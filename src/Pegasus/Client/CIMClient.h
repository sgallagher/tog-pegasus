//BEGIN_LICENSE
//
// Copyright (c) 2000 The Open Group, BMC Software, Tivoli Systems, IBM
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
//END_LICENSE
//BEGIN_HISTORY
//
// Author:
//
// $Log: CIMClient.h,v $
// Revision 1.1  2001/02/16 02:08:26  mike
// Renamed several classes
//
// Revision 1.4  2001/02/13 07:00:18  mike
// Added partial createInstance() method to repository.
//
// Revision 1.3  2001/02/06 17:04:03  karl
// add documentation
//
// Revision 1.2  2001/01/31 08:20:51  mike
// Added dispatcher framework.
// Added enumerateInstanceNames.
//
// Revision 1.1.1.1  2001/01/14 19:50:31  mike
// Pegasus import
//
//
//END_HISTORY

////////////////////////////////////////////////////////////////////////////////
//
// CIMClient.h
//
////////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Client_h
#define Pegasus_Client_h

#include <fstream>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMOperations.h>
#include <Pegasus/Client/ClientException.h>

PEGASUS_NAMESPACE_BEGIN

struct ClientRep;
/** Class CIMClient - This class defines the client interfaces for Pegasus.
These are the interfaces that could be used by a CIM CIMClient written in C++.
These interfaces are based completely on the operations interfaces defined in
the Pegasus CIMOperations.h file with some extensions for the client interface.
@see "The CIMOperations Class"
*/
class PEGASUS_CLIENT_LINKAGE CIMClient : public CIMOperations
{
public:

    enum { DEFAULT_TIMEOUT_MILLISECONDS = 20000 };
    ///
    CIMClient(Uint32 timeOutMilliseconds = DEFAULT_TIMEOUT_MILLISECONDS);
    ///
    ~CIMClient();
    ///
    Uint32 getTimeOut() const
    {
	return _timeOutMilliseconds;
    }
    ///
    void setTimeOut(Uint32 timeOutMilliseconds)
    {
	_timeOutMilliseconds = timeOutMilliseconds;
    }
     ///
    void connect(const char* hostName, Uint32 port);
    ///
    void get(const char* document) const;
    ///
    void runOnce();
    ///
    void runForever();
    ///
    virtual CIMClass getClass(
	const String& nameSpace,
	const String& className,
	Boolean localOnly = true,
	Boolean includeQualifiers = true,
	Boolean includeClassOrigin = false,
	const Array<String>& propertyList = _getStringArray());
    ///
    virtual CIMInstance getInstance(
	const String& nameSpace,
	const CIMReference& instanceName,
	Boolean localOnly = true,
	Boolean includeQualifiers = false,
	Boolean includeClassOrigin = false,
	const Array<String>& propertyList = _getStringArray());
    ///
    virtual void deleteClass(
	const String& nameSpace,
	const String& className);
    ///
    virtual void deleteInstance(
	const String& nameSpace,
	const CIMReference& instanceName);
    ///
    virtual void createClass(
	const String& nameSpace,
	CIMClass& newClass);
    ///
    virtual void createInstance(
	const String& nameSpace,
	CIMInstance& newInstance) ;
    ///
    virtual void modifyClass(
	const String& nameSpace,
	CIMClass& modifiedClass);
    ///
    virtual void modifyInstance(
	const String& nameSpace,
	const CIMInstance& modifiedInstance);
    ///
    virtual Array<CIMClass> enumerateClasses(
	const String& nameSpace,
	const String& className = String::EMPTY,
	Boolean deepInheritance = false,
	Boolean localOnly = true,
	Boolean includeQualifiers  = true,
	Boolean includeClassOrigin = false);
    ///
    virtual Array<String> enumerateClassNames(
	const String& nameSpace,
	const String& className = String::EMPTY,
	Boolean deepInheritance = false);
    ///
    virtual Array<CIMInstance> enumerateInstances(
	const String& nameSpace,
	const String& className,
	Boolean deepInheritance = true,
	Boolean localOnly = true,
	Boolean includeQualifiers = false,
	Boolean includeClassOrigin = false,
	const Array<String>& propertyList = _getStringArray());
    ///
    virtual Array<CIMReference> enumerateInstanceNames(
	const String& nameSpace,
	const String& className);
    ///
    virtual Array<CIMInstance> execQuery(
	const String& queryLanguage,
	const String& query) ;
    ///
    virtual Array<CIMInstance> associators(
	const String& nameSpace,
	const CIMReference& objectName,
	const String& assocClass = String::EMPTY,
	const String& resultClass = String::EMPTY,
	const String& role = String::EMPTY,
	const String& resultRole = String::EMPTY,
	Boolean includeQualifiers = false,
	Boolean includeClassOrigin = false,
	const Array<String>& propertyList = _getStringArray());
    ///
    virtual Array<CIMReference> associatorNames(
	const String& nameSpace,
	const CIMReference& objectName,
	const String& assocClass = String::EMPTY,
	const String& resultClass = String::EMPTY,
	const String& role = String::EMPTY,
	const String& resultRole = String::EMPTY);
    ///
    virtual Array<CIMInstance> references(
	const String& nameSpace,
	const CIMReference& objectName,
	const String& resultClass = String::EMPTY,
	const String& role = String::EMPTY,
	Boolean includeQualifiers = false,
	Boolean includeClassOrigin = false,
	const Array<String>& propertyList = _getStringArray());
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
	const Array<CIMValue>& inParameters,
	Array<CIMValue>& outParameters);

private:

    static Array<String> _getStringArray()
    {
	return Array<String>();
    }

    void _sendMessage(const Array<Sint8>& message);

    ClientRep* _rep;
    void* _handler;
    Uint32 _timeOutMilliseconds;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Client_h */

