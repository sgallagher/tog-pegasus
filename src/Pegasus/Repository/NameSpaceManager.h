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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By: Carol Ann Krug Graves, Hewlett-Packard Company
//                  (carolann_graves@hp.com)
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//              Amit K Arora, IBM (amita@in.ibm.com) for PEP#101
//              David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_NameSpaceManager_h
#define Pegasus_NameSpaceManager_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/HashTable.h>
#include <Pegasus/Common/AutoPtr.h>
#include <Pegasus/Repository/InheritanceTree.h>
#include <Pegasus/Repository/Linkage.h>
#include <Pegasus/Common/MessageLoader.h> //l10n

PEGASUS_NAMESPACE_BEGIN

struct NameSpaceManagerRep;
class NameSpace;

enum NameSpaceIntendedOp {
   NameSpaceRead,
   NameSpaceWrite,
   NameSpaceDelete
};

/** The NameSpaceManager class manages a collection of NameSpace objects.
*/
class PEGASUS_REPOSITORY_LINKAGE NameSpaceManager
{
   friend class NameSpace;
public:

    /** Constructor.
	@param repositoryRoot path to directory called "repository".
	@exception NoSuchDirectory if repositoryRoot not a valid directory.
    */
    NameSpaceManager(const String& repositoryRoot);

    /** Destructor.
    */
    ~NameSpaceManager();

    /** Determines whether the given namespace exists:
	@param nameSpaceName name of namespace.
	@return true if namespace eixsts; false otherwise.
    */
    Boolean nameSpaceExists(const CIMNamespaceName& nameSpaceName) const;

    typedef HashTable <String, String, EqualNoCaseFunc, HashLowerCaseFunc>
        NameSpaceAttributes;

    /** Creates the given namespace.
	@param nameSpaceName name of namespace to be created.
	@exception CIMException(CIM_ERR_ALREADY_EXISTS)
	@exception CannotCreateDirectory
    */
    void createNameSpace(const CIMNamespaceName& nameSpaceName,
	 const NameSpaceAttributes &attributes);

    void modifyNameSpace(const CIMNamespaceName& nameSpaceName,
	 const NameSpaceAttributes &attributes);

    /** Deletes the given namespace.
	@param nameSpaceName name of namespace to be deleted.
	@exception CIMException(CIM_ERR_INVALID_NAMESPACE)
	@exception NonEmptyNameSpace
	@exception FailedToRemoveDirectory
    */
    void deleteNameSpace(const CIMNamespaceName& nameSpaceName);

    Boolean isRemoteNameSpace(const CIMNamespaceName& nameSpaceName,
        String & remoteInfo);

    /** Gets array of all namespace names.
	@param nameSpaceNames filled with names of all namespaces.
    */
    void getNameSpaceNames(Array<CIMNamespaceName>& nameSpaceNames) const;

    Boolean getNameSpaceAttributes(const CIMNamespaceName& nameSpace, NameSpaceAttributes &attributes);

    /** Get path to the class file for the given class.
	@param nameSpaceName name of the namespace.
	@param className name of class.
	@exception CIMException(CIM_ERR_INVALID_NAMESPACE)
	@exception CIMException(CIM_ERR_INVALID_CLASS)
    */
    String getClassFilePath(
	NameSpace *nameSpace,
	const CIMName& className,
        NameSpaceIntendedOp op) const;

    String getClassFilePath(
	const CIMNamespaceName& nameSpaceName,
	const CIMName& className,
        NameSpaceIntendedOp op) const;

    /** Get path to the qualifier file for the given class.
	@param nameSpaceName name of the namespace.
	@param qualifierName name of qualifier.
	@exception CIMException(CIM_ERR_INVALID_NAMESPACE)
	@exception CIMException(CIM_ERR_NOT_FOUND)
    */
    String getQualifierFilePath(
	const CIMNamespaceName& nameSpaceName,
	const CIMName& qualifierName,
        NameSpaceIntendedOp op) const;

    String getInstanceDataFileBase(
	const CIMNamespaceName& nameSpaceName,
	const CIMName& className) const;

    String getInstanceDataFileBase(
        const NameSpace *nameSpace,
        const CIMName& className) const;

    /** Get path to the directory containing qualifiers:
	@param nameSpaceName name of the namespace.
    */
    String getQualifiersRoot(const CIMNamespaceName& nameSpaceName) const;

    /** Get path to the file containing association classes:
	@param nameSpaceName name of the namespace.
    */
    Array<String> getAssocClassPath(const CIMNamespaceName& nameSpaceName,
        NameSpaceIntendedOp op) const;

    /** Get path to the file containing association instances:
	@param nameSpaceName name of the namespace.
    */
    String getAssocInstPath(const CIMNamespaceName& nameSpaceName) const;

    /** Deletes the class file for the given class.
	@param nameSpaceName name of namespace.
	@param className name of class.
	@exception CIMException(CIM_ERR_INVALID_NAMESPACE)
	@exception CIMException(CIM_ERR_INVALID_CLASS)
	@exception CIMException(CIM_ERR_CLASS_HAS_CHILDREN)
    */
    void deleteClass(
	const CIMNamespaceName& nameSpaceName,
	const CIMName& className) const;

    /** Print out the namespaces. */
    void print(PEGASUS_STD(ostream)& os) const;

    /** Creates an entry for a new class.
	@param nameSpaceName namespace to contain class.
	@param className name of class
	@param superClassName name of superClassName
	@param classFilePath path of file to contain class itself.
    */
    void createClass(
	const CIMNamespaceName& nameSpaceName,
	const CIMName& className,
	const CIMName& superClassName,
	String& classFilePath);

    /** Checks if it is okay to modify this class.
	@param nameSpaceName namespace.
	@param className name of class being modified.
	@param superClassName superclass of class being modified.
	@param classFilePath full path to file containing class.
	@exception CIMException(CIM_ERR_INVALID_CLASS)
	@exception CIMException(CIM_ERR_FAILED) if there is an attempt
	    to change the superclass of this class.
	@exception CIMException(CIM_ERR_CLASS_HAS_CHILDREN) if class
	    has any children.
    */
    void checkModify(
	const CIMNamespaceName& nameSpaceName,
	const CIMName& className,
	const CIMName& superClassName,
	String& classFilePath);

    /** Get subclass names of the given class in the given namespace.
	@param nameSpaceName
	@param className - class whose subclass names will be gotten. If
	    className is empty, all classnames are returned.
	@param deepInheritance - if true all descendent classes of class
	    are returned. If className is empty, only root classes are returned.
	@param subClassNames - output argument to hold subclass names.
	@exception CIMException(CIM_ERR_INVALID_CLASS)
    */
    void getSubClassNames(
	const CIMNamespaceName& nameSpaceName,
	const CIMName& className,
	Boolean deepInheritance,
	Array<CIMName>& subClassNames,
	Boolean enm=false) const;

    /** Get the names of all superclasses (direct and indirect) of this
	class.
    */
    void getSuperClassNames(
	const CIMNamespaceName& nameSpaceName,
	const CIMName& className,
	Array<CIMName>& subClassNames) const;

    Boolean classHasInstances(
	NameSpace *nameSpace,
	const CIMName& className,
	Boolean throwExcp=false) const;

    Boolean classHasInstances(
	const CIMNamespaceName& nameSpaceName,
	const CIMName& className,
	Boolean throwExcp=false) const;

    Boolean classExists(
	NameSpace *nameSpace,
	const CIMName& className,
	Boolean throwExcp=false) const;

private:
    NameSpace *lookupNameSpace(String&);
    String _repositoryRoot;
    NameSpaceManagerRep* _rep;
};

/** This exception is thrown if one attempts to remove a namespace that still
    contains classes, instances, or qualifier.
*/
class NonEmptyNameSpace : public Exception
{
public:
	//l10n
    //NonEmptyNameSpace(const String& nameSpaceName) : Exception(
	//"Attempt to delete a non-empty namespace: " + nameSpaceName) { }
	NonEmptyNameSpace(const String& nameSpaceName) :
								Exception(MessageLoaderParms(
								"Repository.NameSpaceManager.ATTEMPT_DELETE_NONEMPTY_NAMESPACE",
								"Attempt to delete a non-empty namespace: $0", nameSpaceName)) { }


	//l10n end
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_NameSpaceManager_h */
