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
// $Log: CIMRepository.h,v $
// Revision 1.3  2001/03/05 19:54:50  mike
// Fixed earlier boo boo (renamed CimException to CIMException).
//
// Revision 1.2  2001/02/18 19:02:18  mike
// Fixed CIM debacle
//
// Revision 1.1  2001/02/18 18:50:50  mike
// new
//
// Revision 1.9  2001/02/18 02:49:01  mike
// Removed ugly workarounds for MSVC++ 5.0 (using SP3 now)
//
// Revision 1.8  2001/02/16 02:06:09  mike
// Renamed many classes and headers.
//
// Revision 1.7  2001/02/13 07:00:18  mike
// Added partial createInstance() method to repository.
//
// Revision 1.6  2001/02/11 05:45:33  mike
// Added case insensitive logic for files in CIMRepository
//
// Revision 1.5  2001/02/08 00:31:51  mike
// Clean up on CIMOperations class.
// Reformmatted documentation.
//
// Revision 1.4  2001/02/06 17:04:03  karl
// add documentation
//
// Revision 1.3  2001/01/31 08:20:51  mike
// Added dispatcher framework.
// Added enumerateInstanceNames.
//
// Revision 1.2  2001/01/28 04:11:03  mike
// fixed qualifier resolution
//
// Revision 1.1.1.1  2001/01/14 19:53:56  mike
// Pegasus import
//
//
//END_HISTORY

#ifndef PegasusRepository_Repository_h
#define PegasusRepository_Repository_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMClass.h>
#include <Pegasus/Common/CIMInstance.h>
#include <Pegasus/Common/CIMQualifierDecl.h>
#include <Pegasus/Common/CIMOperations.h>

PEGASUS_NAMESPACE_BEGIN

class RepositoryDeclContext;

/** This class derives from the CIMOperations class and provides a simple
    implementation of a CIM repository. It only implements the methods
    for manipulating classes and instances. The others throw this exception:

    <pre>
	CIMException(CIMException::NOT_SUPPORTED)
    </pre>
*/

class PEGASUS_REPOSITORY_LINKAGE CIMRepository : public CIMOperations
{
public:

    /// Constructor
    CIMRepository(const String& path);

    /// Descructor
    virtual ~CIMRepository();

    /// virtual class CIMClass. From the operations class
    virtual CIMClass getClass(
	const String& nameSpace,
	const String& className,
	Boolean localOnly = true,
	Boolean includeQualifiers = true,
	Boolean includeClassOrigin = false,
	const Array<String>& propertyList = StringArray());

    /// getInstance
    virtual CIMInstance getInstance(
	const String& nameSpace,
	const CIMReference& instanceName,
	Boolean localOnly = true,
	Boolean includeQualifiers = false,
	Boolean includeClassOrigin = false,
	const Array<String>& propertyList = StringArray());

    /// deleteClass
    virtual void deleteClass(
	const String& nameSpace,
	const String& className);

    /// deleteInstance
    virtual void deleteInstance(
	const String& nameSpace,
	const CIMReference& instanceName);

    /// createClass
    virtual void createClass(
	const String& nameSpace,
	CIMClass& newClass);

    /// createInstance
    virtual void createInstance(
	const String& nameSpace,
	CIMInstance& newInstance);

    virtual void modifyClass(
	const String& nameSpace,
	CIMClass& modifiedClass);

    /// modifyInstance
    virtual void modifyInstance(
	const String& nameSpace,
	const CIMInstance& modifiedInstance);

    /// enumerateClasses
    virtual Array<CIMClass> enumerateClasses(
	const String& nameSpace,
	const String& className = String::EMPTY,
	Boolean deepInheritance = false,
	Boolean localOnly = true,
	Boolean includeQualifiers  = true,
	Boolean includeClassOrigin = false);

    /// enumerateClassNames
    virtual Array<String> enumerateClassNames(
	const String& nameSpace,
	const String& className = String::EMPTY,
	Boolean deepInheritance = false);

    /// enumerateInstances
    virtual Array<CIMInstance> enumerateInstances(
	const String& nameSpace,
	const String& className,
	Boolean deepInheritance = true,
	Boolean localOnly = true,
	Boolean includeQualifiers = false,
	Boolean includeClassOrigin = false,
	const Array<String>& propertyList = StringArray());

    /// enumerateInstanceNames
    virtual Array<CIMReference> enumerateInstanceNames(
	const String& nameSpace,
	const String& className);

    /// execQuery
    virtual Array<CIMInstance> execQuery(
	const String& queryLanguage,
	const String& query) ;

    /// associators
    virtual Array<CIMInstance> associators(
	const String& nameSpace,
	const CIMReference& objectName,
	const String& assocClass = String::EMPTY,
	const String& resultClass = String::EMPTY,
	const String& role = String::EMPTY,
	const String& resultRole = String::EMPTY,
	Boolean includeQualifiers = false,
	Boolean includeClassOrigin = false,
	const Array<String>& propertyList = StringArray());

    /// associateNames
    virtual Array<CIMReference> associatorNames(
	const String& nameSpace,
	const CIMReference& objectName,
	const String& assocClass = String::EMPTY,
	const String& resultClass = String::EMPTY,
	const String& role = String::EMPTY,
	const String& resultRole = String::EMPTY);

    /// references
    virtual Array<CIMInstance> references(
	const String& nameSpace,
	const CIMReference& objectName,
	const String& resultClass = String::EMPTY,
	const String& role = String::EMPTY,
	Boolean includeQualifiers = false,
	Boolean includeClassOrigin = false,
	const Array<String>& propertyList = StringArray());

    /// referenceNames
    virtual Array<CIMReference> referenceNames(
	const String& nameSpace,
	const CIMReference& objectName,
	const String& resultClass = String::EMPTY,
	const String& role = String::EMPTY);

    /// getProperty
    virtual CIMValue getProperty(
	const String& nameSpace,
	const CIMReference& instanceName,
	const String& propertyName);

    /// setProperty
    virtual void setProperty(
	const String& nameSpace,
	const CIMReference& instanceName,
	const String& propertyName,
	const CIMValue& newValue = CIMValue());

    /// getQualifier
    virtual CIMQualifierDecl getQualifier(
	const String& nameSpace,
	const String& qualifierName);

    /// setQualifier
    virtual void setQualifier(
	const String& nameSpace,
	const CIMQualifierDecl& qualifierDecl);

    /// virtual deleteQualifier
    virtual void deleteQualifier(
	const String& nameSpace,
	const String& qualifierName);

    /// enumerateQualifiers
    virtual Array<CIMQualifierDecl> enumerateQualifiers(
	const String& nameSpace);

    /// Virtual value
    virtual CIMValue invokeMethod(
	const String& nameSpace,
	const CIMReference& instanceName,
	const String& methodName,
	const Array<CIMValue>& inParameters,
	Array<CIMValue>& outParameters);

    /** CIMMethod createNameSpace - Creates a new namespace in the repository
	@param String with the name of the namespace
	@exception - ???
	ATTN: What happens if the namespace already exists.
    */
    void createNameSpace(const String& nameSpace);

    /** CIMMethod enumerateNameSpaces - Get all of the namespaces in the
	repository. \Ref{NAMESPACE}
	@return Array of strings with the namespaces
    */
    virtual Array<String> enumerateNameSpaces() const;

    /** CIMMethod createMetaQualifiers - ATTN:
	This method must be invoked to create the appropriate meta-qualifiers
	required by CIM (they are not part of the CIM schema; rather they are
	part of the meta-schema).
    */
    void createMetaQualifiers(const String& nameSpace);

private:

    String _root;
    RepositoryDeclContext* _context;
};

PEGASUS_NAMESPACE_END

#endif /* PegasusRepository_Repository_h */

