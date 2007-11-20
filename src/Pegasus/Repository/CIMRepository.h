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

#ifndef Pegasus_Repository_h
#define Pegasus_Repository_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMClass.h>
#include <Pegasus/Common/CIMObject.h>
#include <Pegasus/Common/CIMInstance.h>
#include <Pegasus/Common/CIMPropertyList.h>
#include <Pegasus/Common/CIMQualifierDecl.h>
#include <Pegasus/Common/ContentLanguageList.h>
#include <Pegasus/Config/ConfigManager.h>
#include <Pegasus/Repository/NameSpaceManager.h>
#include <Pegasus/Repository/Linkage.h>
#include <Pegasus/Common/ReadWriteSem.h>

#include <Pegasus/Common/ObjectStreamer.h>

PEGASUS_NAMESPACE_BEGIN

class RepositoryDeclContext;
class compilerDeclContext;

/** This class provides a simple implementation of a CIM repository.
    Concurrent access is controlled by an internal lock.
*/
class PEGASUS_REPOSITORY_LINKAGE CIMRepository
{
public:

    enum CIMRepositoryMode
    {
        MODE_DEFAULT = 0,
        MODE_XML = 1,
        MODE_BIN = 2,
        MODE_COMPRESSED = 4
    };

    /// Constructor
    CIMRepository(
        const String& repositoryRoot,
        Uint32 mode = CIMRepository::MODE_DEFAULT,
        RepositoryDeclContext* declContext = 0);

    /// Descructor
    virtual ~CIMRepository();

    /// getClass
    virtual CIMClass getClass(
        const CIMNamespaceName& nameSpace,
        const CIMName& className,
        Boolean localOnly = true,
        Boolean includeQualifiers = true,
        Boolean includeClassOrigin = false,
        const CIMPropertyList& propertyList = CIMPropertyList());

    /// getInstance
    virtual CIMInstance getInstance(
        const CIMNamespaceName& nameSpace,
        const CIMObjectPath& instanceName,
        Boolean localOnly = true,
        Boolean includeQualifiers = false,
        Boolean includeClassOrigin = false,
        const CIMPropertyList& propertyList = CIMPropertyList());

    /// deleteClass
    virtual void deleteClass(
        const CIMNamespaceName& nameSpace,
        const CIMName& className);

    /// deleteInstance
    virtual void deleteInstance(
        const CIMNamespaceName& nameSpace,
        const CIMObjectPath& instanceName);

    /// createClass
    virtual void createClass(
        const CIMNamespaceName& nameSpace,
        const CIMClass& newClass,
        const ContentLanguageList& contentLangs = ContentLanguageList());

    /// createInstance
    virtual CIMObjectPath createInstance(
        const CIMNamespaceName& nameSpace,
        const CIMInstance& newInstance,
        const ContentLanguageList& contentLangs = ContentLanguageList());

    /// modifyClass
    virtual void modifyClass(
        const CIMNamespaceName& nameSpace,
        const CIMClass& modifiedClass,
        const ContentLanguageList& contentLangs = ContentLanguageList());

    /// modifyInstance
    virtual void modifyInstance(
        const CIMNamespaceName& nameSpace,
        const CIMInstance& modifiedInstance,
        Boolean includeQualifiers = true,
        const CIMPropertyList& propertyList = CIMPropertyList(),
        const ContentLanguageList& contentLangs = ContentLanguageList());

    /// enumerateClasses
    virtual Array<CIMClass> enumerateClasses(
        const CIMNamespaceName& nameSpace,
        const CIMName& className = CIMName(),
        Boolean deepInheritance = false,
        Boolean localOnly = true,
        Boolean includeQualifiers = true,
        Boolean includeClassOrigin = false);

    /// enumerateClassNames
    virtual Array<CIMName> enumerateClassNames(
        const CIMNamespaceName& nameSpace,
        const CIMName& className = CIMName(),
        Boolean deepInheritance = false);

    /**
        Enumerates the instances of the specified class and its subclasses.
        This method mimics the client behavior for the EnumerateInstances
        operation, but of course it can only return the instances that reside
        in the repository.  This method does not perform deepInheritance
        filtering regardless of the value given for that parameter.

        This method is useful mainly for testing purposes, and should not be
        relied upon for complete results in a CIM Server environment.
    */
    virtual Array<CIMInstance> enumerateInstancesForSubtree(
        const CIMNamespaceName& nameSpace,
        const CIMName& className,
        Boolean deepInheritance = true,
        Boolean localOnly = true,
        Boolean includeQualifiers = false,
        Boolean includeClassOrigin = false,
        const CIMPropertyList& propertyList = CIMPropertyList());

    /**
        Enumerates the instances of just the specified class.
        This method mimics the provider behavior for the EnumerateInstances
        operation.
    */
    virtual Array<CIMInstance> enumerateInstancesForClass(
        const CIMNamespaceName& nameSpace,
        const CIMName& className,
        Boolean localOnly = true,
        Boolean includeQualifiers = false,
        Boolean includeClassOrigin = false,
        const CIMPropertyList& propertyList = CIMPropertyList());


    /**
        Enumerates the names of the instances of the specified class and its
        subclasses.  This method mimics the client behavior for the
        EnumerateInstanceNames operation, but of course it can only return
        the names of the instances that reside in the repository.

        This method is useful mainly for testing purposes, and should not be
        relied upon for complete results in a CIM Server environment.

        @param nameSpace The namespace in which className resides.
        @param className The name the class for which to retrieve the instance
            names.
        @return An Array of CIMObjectPath objects containing the names of the
            instances of the specified class in the specified namespace.
    */
    virtual Array<CIMObjectPath> enumerateInstanceNamesForSubtree(
        const CIMNamespaceName& nameSpace,
        const CIMName& className);

    /**
        Enumerates the names of the instances of just the specified class.
        This method mimics the provider behavior for the EnumerateInstanceNames
        operation.

        @param nameSpace The namespace in which className resides.
        @param className The name the class for which to retrieve the instance
            names.
        @return An Array of CIMObjectPath objects containing the names of the
            instances of the specified class in the specified namespace.
    */
    virtual Array<CIMObjectPath> enumerateInstanceNamesForClass(
        const CIMNamespaceName& nameSpace,
        const CIMName& className);


    /// associators
    virtual Array<CIMObject> associators(
        const CIMNamespaceName& nameSpace,
        const CIMObjectPath& objectName,
        const CIMName& assocClass = CIMName(),
        const CIMName& resultClass = CIMName(),
        const String& role = String::EMPTY,
        const String& resultRole = String::EMPTY,
        Boolean includeQualifiers = false,
        Boolean includeClassOrigin = false,
        const CIMPropertyList& propertyList = CIMPropertyList());

    /// associatorNames
    virtual Array<CIMObjectPath> associatorNames(
        const CIMNamespaceName& nameSpace,
        const CIMObjectPath& objectName,
        const CIMName& assocClass = CIMName(),
        const CIMName& resultClass = CIMName(),
        const String& role = String::EMPTY,
        const String& resultRole = String::EMPTY);

    /// references
    virtual Array<CIMObject> references(
        const CIMNamespaceName& nameSpace,
        const CIMObjectPath& objectName,
        const CIMName& resultClass = CIMName(),
        const String& role = String::EMPTY,
        Boolean includeQualifiers = false,
        Boolean includeClassOrigin = false,
        const CIMPropertyList& propertyList = CIMPropertyList());

    /// referenceNames
    virtual Array<CIMObjectPath> referenceNames(
        const CIMNamespaceName& nameSpace,
        const CIMObjectPath& objectName,
        const CIMName& resultClass = CIMName(),
        const String& role = String::EMPTY);

    /// getProperty
    virtual CIMValue getProperty(
        const CIMNamespaceName& nameSpace,
        const CIMObjectPath& instanceName,
        const CIMName& propertyName);

    /// setProperty
    virtual void setProperty(
        const CIMNamespaceName& nameSpace,
        const CIMObjectPath& instanceName,
        const CIMName& propertyName,
        const CIMValue& newValue = CIMValue(),
        const ContentLanguageList& contentLangs = ContentLanguageList());

    /// getQualifier
    virtual CIMQualifierDecl getQualifier(
        const CIMNamespaceName& nameSpace,
        const CIMName& qualifierName);

    /// setQualifier
    virtual void setQualifier(
        const CIMNamespaceName& nameSpace,
        const CIMQualifierDecl& qualifierDecl,
        const ContentLanguageList& contentLangs = ContentLanguageList());

    /// deleteQualifier
    virtual void deleteQualifier(
        const CIMNamespaceName& nameSpace,
        const CIMName& qualifierName);

    /// enumerateQualifiers
    virtual Array<CIMQualifierDecl> enumerateQualifiers(
        const CIMNamespaceName& nameSpace);

    typedef HashTable <String, String, EqualNoCaseFunc, HashLowerCaseFunc>
        NameSpaceAttributes;

    /** CIMMethod createNameSpace - Creates a new namespace in the repository
        @param String with the name of the namespace
        @exception - Throws "Already_Exists if the Namespace exits.
        Throws "CannotCreateDirectory" if there are problems in the
        creation.
    */

    virtual void createNameSpace(const CIMNamespaceName& nameSpace,
        const NameSpaceAttributes& attributes = NameSpaceAttributes());

    virtual void modifyNameSpace(const CIMNamespaceName& nameSpace,
        const NameSpaceAttributes& attributes = NameSpaceAttributes());

    /** CIMMethod enumerateNameSpaces - Get all of the namespaces in the
        repository. \Ref{NAMESPACE}
        @return Array of strings with the namespaces
    */
    virtual Array<CIMNamespaceName> enumerateNameSpaces() const;

    /** CIMMethod deleteNameSpace - Deletes a namespace in the repository.
        The deleteNameSpace method will only delete a namespace if there are
        no classed defined in the namespace.  Today this is a Pegasus
        characteristics and not defined as part of the DMTF standards.
        @param String with the name of the namespace
        @exception - Throws NoSuchDirectory if the Namespace does not exist.
    */
    virtual void deleteNameSpace(const CIMNamespaceName& nameSpace);

    virtual Boolean getNameSpaceAttributes(
        const CIMNamespaceName& nameSpace,
        NameSpaceAttributes& attributes);

    ////////////////////////////////////////////////////////////////////////////

    /** Indicates whether instance operations that do not have a provider
        registered should be served by this repository.
    */
    Boolean isDefaultInstanceProvider()
    {
        return _isDefaultInstanceProvider;
    }

    /** Get subclass names of the given class in the given namespace.
        @param nameSpaceName
        @param className - class whose subclass names will be gotten. If
            className is empty, all classnames are returned.
        @param deepInheritance - if true all descendent classes of class
            are returned. If className is empty, only root classes are returned.
        @param subClassNames - output argument to hold subclass names.
        @exception CIMException(CIM_ERR_INVALID_CLASS)
    */
    virtual void getSubClassNames(
        const CIMNamespaceName& nameSpaceName,
        const CIMName& className,
        Boolean deepInheritance,
        Array<CIMName>& subClassNames) const
    {
        ReadLock lock(const_cast<ReadWriteSem&>(_lock));
        _nameSpaceManager.getSubClassNames(nameSpaceName,
                                           className,
                                           deepInheritance,
                                           subClassNames);
    }

    /** Get the names of all superclasses (direct and indirect) of this
        class.
    */
    virtual void getSuperClassNames(
        const CIMNamespaceName& nameSpaceName,
        const CIMName& className,
        Array<CIMName>& subClassNames) const
    {
        ReadLock lock(const_cast<ReadWriteSem&>(_lock));
        _nameSpaceManager.getSuperClassNames(
            nameSpaceName, className, subClassNames);
    }

    virtual Boolean isRemoteNameSpace(
        const CIMNamespaceName& nameSpaceName,
        String& remoteInfo);

#ifdef PEGASUS_DEBUG
    void DisplayCacheStatistics();
#endif

protected:

    // Internal getClass implementation that does not do access control
    CIMClass _getClass(
        const CIMNamespaceName& nameSpace,
        const CIMName& className,
        Boolean localOnly,
        Boolean includeQualifiers,
        Boolean includeClassOrigin,
        const CIMPropertyList& propertyList);

    /// Internal getInstance implementation that does not do access control
    CIMInstance _getInstance(
        const CIMNamespaceName& nameSpace,
        const CIMObjectPath& instanceName,
        Boolean localOnly,
        Boolean includeQualifiers,
        Boolean includeClassOrigin,
        const CIMPropertyList& propertyList);

    /// Internal createClass implementation that does not do access control
    void _createClass(
        const CIMNamespaceName& nameSpace,
        const CIMClass& newClass);

    /// Internal createInstance implementation that does not do access control
    CIMObjectPath _createInstance(
        const CIMNamespaceName& nameSpace,
        const CIMInstance& newInstance);

    /// Internal modifyClass implementation that does not do access control
    void _modifyClass(
        const CIMNamespaceName& nameSpace,
        const CIMClass& modifiedClass);

    /// Internal associatorNames implementation that does not do access control
    Array<CIMObjectPath> _associatorNames(
        const CIMNamespaceName& nameSpace,
        const CIMObjectPath& objectName,
        const CIMName& assocClass,
        const CIMName& resultClass,
        const String& role,
        const String& resultRole);

    /// Internal referenceNames implementation that does not do access control
    Array<CIMObjectPath> _referenceNames(
        const CIMNamespaceName& nameSpace,
        const CIMObjectPath& objectName,
        const CIMName& resultClass,
        const String& role);

    /// Internal getQualifier implementation that does not do access control
    CIMQualifierDecl _getQualifier(
        const CIMNamespaceName& nameSpace,
        const CIMName& qualifierName);

    /// Internal setQualifier implementation that does not do access control
    void _setQualifier(
        const CIMNamespaceName& nameSpace,
        const CIMQualifierDecl& qualifierDecl);

private:

    /**
        Searches for incomplete instance transactions for all classes in all
        namespaces.  Restores instance index and data files to void an
        incomplete operation.  If no incomplete instance transactions are
        outstanding, this method has no effect.
     */
    void _rollbackIncompleteTransactions();

    void _createAssocInstEntries(
        const CIMNamespaceName& nameSpace,
        const CIMConstClass& cimClass,
        const CIMInstance& cimInstance,
        const CIMObjectPath& instanceName);

    void _createAssocClassEntries(
        const CIMNamespaceName& nameSpace,
        const CIMConstClass& assocClass);

    /**
        Checks whether an instance with the specified key values exists in the
        class hierarchy of the specified class.

        @param   nameSpace      the namespace of the instance
        @param   instanceName   the name of the instance

        @return  true           if the instance is found
                 false          if the instance cannot be found
     */
    Boolean _checkInstanceAlreadyExists(
        const CIMNamespaceName& nameSpace,
        const CIMObjectPath& instanceName) const;

    /** Returns the file path of the instance index file.

        @param   nameSpace      the namespace of the instance
        @param   className      the name of the class

        @return  a string containing the index file path
     */
    String _getInstanceIndexFilePath(
        const CIMNamespaceName& nameSpace,
        const CIMName& className) const;

    /** Returns the file path of the instance file.

        @param   nameSpace      the namespace of the instance
        @param   className      the name of the class

        @return  a string containing the instance file path
     */
    String _getInstanceDataFilePath(
        const CIMNamespaceName& nameSpace,
        const CIMName& className) const;

    /** Saves an instance object from memory to disk file.  The byte
        position and the size of the newly inserted instance record are
        returned.  Returns true on success.

        @param   path      the file path of the instance file
        @param   object    the CIMInstance object to be saved
        @param   index     the byte positon of the saved instance record
        @param   size      the size of the saved instance record

        @return  true      if successful
                 false     if an error occurs in saving the instance to file
     */
    Boolean _saveInstance(
        const String& path,
        const CIMInstance& object,
        Uint32& index,
        Uint32& size);

    /** loads an instance object from disk to memory.  The caller passes
        the byte position and the size of the instance record to be loaded.
        Returns true on success.

        @param   path      the file path of the instance file
        @param   object    the CIMInstance object to be returned
        @param   index     the byte positon of the instance record
        @param   size      the size of the instance record
        @param   data      the buffer to hold the instance data

        @return  true      if successful
                 false     if an error occurs in loading the instance from file
     */
    Boolean _loadInstance(
        const String& path,
        CIMInstance& object,
        Uint32 index,
        Uint32 size);

    /** loads all the instance objects from disk to memeory.  Returns true
        on success.

        @param   nameSpace      the namespace of the instances to be loaded
        @param   className      the class of the instances to be loaded
        @param   namedInstances an array of CIMInstance objects to which
                                the loaded instances are appended

        @return  true      if successful
                 false     if an error occurs in loading the instances
     */
    Boolean _loadAllInstances(
        const CIMNamespaceName& nameSpace,
        const CIMName& className,
        Array<CIMInstance>& namedInstances);

    /** Modifies an instance object saved in the disk file.  The byte position
        and the size of the newly added instance record are returned.  Returns
        true on success.

        @param   path      the file path of the instance file
        @param   object    the modified CIMInstance object
        @param   oldIndex  the byte positon of the old instance record
        @param   oldSize   the size of the old instance record
        @param   newIndex  the byte positon of the new instance record
        @param   newSize   the size of the new instance record

        @return  true      if successful
                 false     if an error occurs in modifying the instance
     */
    Boolean _modifyInstance(
        const String& path,
        const CIMInstance& object,
        Uint32 oldIndex,
        Uint32 oldSize,
        Uint32& newIndex,
        Uint32& newSize);

    String _repositoryRoot;
    NameSpaceManager _nameSpaceManager;

    // This must be initialized in the constructor using values from the
    // ConfigManager.
    Boolean _isDefaultInstanceProvider;

protected:

    ObjectStreamer *streamer;
    ReadWriteSem _lock;

    friend class compilerDeclContext;
    friend class RepositoryDeclContext;
    RepositoryDeclContext* _context;

    /** Used by getInstance(); indicates whether instance should be resolved
        after it is retrieved from the file.
     */
    Boolean _resolveInstance;

    CString _lockFile;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Repository_h */
