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
// $Log: Repository.h,v $
// Revision 1.7  2001/02/13 07:00:18  mike
// Added partial createInstance() method to repository.
//
// Revision 1.6  2001/02/11 05:45:33  mike
// Added case insensitive logic for files in Repository
//
// Revision 1.5  2001/02/08 00:31:51  mike
// Clean up on Operations class.
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
#include <Pegasus/Common/ClassDecl.h>
#include <Pegasus/Common/InstanceDecl.h>
#include <Pegasus/Common/QualifierDecl.h>
#include <Pegasus/Common/Operations.h>

PEGASUS_NAMESPACE_BEGIN

class RepositoryDeclContext;

/** This class derives from the Operations class and provides a simple
    implementation of a CIM repository. It only implements the methods
    for manipulating classes and instances. The others throw this exception:

    <pre>
	CimException(CimException::NOT_SUPPORTED)
    </pre>
*/

class PEGASUS_REPOSITORY_LINKAGE Repository : public Operations
{
public:

    /// Constructor
    Repository(const String& path);

    /// Descructor
    virtual ~Repository();

    /// virtual class ClassDecl. From the operations class
    virtual ClassDecl getClass(
	const String& nameSpace,
	const String& className,
	Boolean localOnly = true,
	Boolean includeQualifiers = true,
	Boolean includeClassOrigin = false,
	const Array<String>& propertyList = _getStringArray());

    /// getInstance
    virtual InstanceDecl getInstance(
	const String& nameSpace,
	const Reference& instanceName,
	Boolean localOnly = true,
	Boolean includeQualifiers = false,
	Boolean includeClassOrigin = false,
	const Array<String>& propertyList = _getStringArray());

    /// deleteClass
    virtual void deleteClass(
	const String& nameSpace,
	const String& className);

    /// deleteInstance
    virtual void deleteInstance(
	const String& nameSpace,
	const Reference& instanceName);

    /// createClass
    virtual void createClass(
	const String& nameSpace,
	ClassDecl& newClass);

    /// createInstance
    virtual void createInstance(
	const String& nameSpace,
	InstanceDecl& newInstance);

    virtual void modifyClass(
	const String& nameSpace,
	ClassDecl& modifiedClass);

    /// modifyInstance
    virtual void modifyInstance(
	const String& nameSpace,
	const InstanceDecl& modifiedInstance);

    /// enumerateClasses
    virtual Array<ClassDecl> enumerateClasses(
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
    virtual Array<InstanceDecl> enumerateInstances(
	const String& nameSpace,
	const String& className,
	Boolean deepInheritance = true,
	Boolean localOnly = true,
	Boolean includeQualifiers = false,
	Boolean includeClassOrigin = false,
	const Array<String>& propertyList = _getStringArray());

    /// enumerateInstanceNames
    virtual Array<Reference> enumerateInstanceNames(
	const String& nameSpace,
	const String& className);

    /// execQuery
    virtual Array<InstanceDecl> execQuery(
	const String& queryLanguage,
	const String& query) ;

    /// associators
    virtual Array<InstanceDecl> associators(
	const String& nameSpace,
	const Reference& objectName,
	const String& assocClass = String::EMPTY,
	const String& resultClass = String::EMPTY,
	const String& role = String::EMPTY,
	const String& resultRole = String::EMPTY,
	Boolean includeQualifiers = false,
	Boolean includeClassOrigin = false,
	const Array<String>& propertyList = _getStringArray());

    /// associateNames
    virtual Array<Reference> associatorNames(
	const String& nameSpace,
	const Reference& objectName,
	const String& assocClass = String::EMPTY,
	const String& resultClass = String::EMPTY,
	const String& role = String::EMPTY,
	const String& resultRole = String::EMPTY);

    /// references
    virtual Array<InstanceDecl> references(
	const String& nameSpace,
	const Reference& objectName,
	const String& resultClass = String::EMPTY,
	const String& role = String::EMPTY,
	Boolean includeQualifiers = false,
	Boolean includeClassOrigin = false,
	const Array<String>& propertyList = _getStringArray());

    /// referenceNames
    virtual Array<Reference> referenceNames(
	const String& nameSpace,
	const Reference& objectName,
	const String& resultClass = String::EMPTY,
	const String& role = String::EMPTY);

    /// getProperty
    virtual Value getProperty(
	const String& nameSpace,
	const Reference& instanceName,
	const String& propertyName);

    /// setProperty
    virtual void setProperty(
	const String& nameSpace,
	const Reference& instanceName,
	const String& propertyName,
	const Value& newValue = Value());

    /// getQualifier
    virtual QualifierDecl getQualifier(
	const String& nameSpace,
	const String& qualifierName);

    /// setQualifier
    virtual void setQualifier(
	const String& nameSpace,
	const QualifierDecl& qualifierDecl);

    /// virtual deleteQualifier
    virtual void deleteQualifier(
	const String& nameSpace,
	const String& qualifierName);

    /// enumerateQualifiers
    virtual Array<QualifierDecl> enumerateQualifiers(
	const String& nameSpace);

    /// Virtual value
    virtual Value invokeMethod(
	const String& nameSpace,
	const Reference& instanceName,
	const String& methodName,
	const Array<Value>& inParameters,
	Array<Value>& outParameters);

    /** Method createNameSpace - Creates a new namespace in the repository
	@param String with the name of the namespace
	@exception - ???
	ATTN: What happens if the namespace already exists.
    */
    void createNameSpace(const String& nameSpace);

    /** Method enumerateNameSpaces - Get all of the namespaces in the
	repository. \Ref{NAMESPACE}
	@return Array of strings with the namespaces
    */
    virtual Array<String> enumerateNameSpaces() const;

    /** Method createMetaQualifiers - ATTN:
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

