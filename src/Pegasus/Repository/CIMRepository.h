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
// Modified By: Jenny Yu, Hewlett-Packard Company (jenny_yu@hp.com)
//              Yi Zhou, Hewlett-Packard Company (yi_zhou@hp.com)
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//              Carol Ann Krug Graves, Hewlett-Packard Company 
//                  (carolann_graves@hp.com)
//              Adrian Schuur (schuur@de.ibm.com) - PEP 129
//              David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef PegasusRepository_Repository_h
#define PegasusRepository_Repository_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/IPC.h>
#include <Pegasus/Common/CIMClass.h>
#include <Pegasus/Common/CIMObject.h>
#include <Pegasus/Common/CIMInstance.h>
#include <Pegasus/Common/CIMPropertyList.h>
#include <Pegasus/Common/CIMQualifierDecl.h>
#include <Pegasus/Common/CIMRepositoryBase.h>
#include <Pegasus/Common/ContentLanguages.h>
#include <Pegasus/Config/ConfigManager.h>
#include <Pegasus/Repository/NameSpaceManager.h>
#include <Pegasus/Repository/Linkage.h>

#include <Pegasus/Common/ObjectStreamer.h>

PEGASUS_NAMESPACE_BEGIN

class CIMRepository_Mode {
 public:
  int flag;
  enum {
    NONE = 0,
    BIN = 1,
    COMPRESSED = 2,
    INDEXED = 4,
    UNUSED1 = 8,
    UNUSED2 = 16,
    UNUSED3 = 32,
    UNUSED4 = 64,
    ANY = 0xFF
  };
};


class RepositoryDeclContext;
class compilerDeclContext;

/** This class provides a simple implementation of a CIM repository.
    Concurrent access is controlled by an internal lock.
*/
class PEGASUS_REPOSITORY_LINKAGE CIMRepository : public CIMRepositoryBase
{
public:

    /// Constructor
    CIMRepository(const String& repositoryRoot);
    CIMRepository(const String& repositoryRoot, const CIMRepository_Mode Mode);

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
	const ContentLanguages& contentLangs = ContentLanguages::EMPTY);

    /// createInstance
    virtual CIMObjectPath createInstance(
        const CIMNamespaceName& nameSpace,
        const CIMInstance& newInstance,
	const ContentLanguages& contentLangs = ContentLanguages::EMPTY);

    /// modifyClass
    virtual void modifyClass(
        const CIMNamespaceName& nameSpace,
        const CIMClass& modifiedClass,
	const ContentLanguages& contentLangs = ContentLanguages::EMPTY);

    /// modifyInstance
    virtual void modifyInstance(
        const CIMNamespaceName& nameSpace,
        const CIMInstance& modifiedInstance,
        Boolean includeQualifiers = true,
        const CIMPropertyList& propertyList = CIMPropertyList(),
	const ContentLanguages& contentLangs = ContentLanguages::EMPTY);

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

    
    virtual Array<CIMInstance> enumerateInstances(
        const CIMNamespaceName& nameSpace,
        const CIMName& className,
        Boolean deepInheritance = true,
        Boolean localOnly = true,
        Boolean includeQualifiers = false,
        Boolean includeClassOrigin = false,
	const CIMPropertyList& propertyList = CIMPropertyList());

    /** enumerateInstances for a single Class. This and the forClass
    // in enumerate instancenames are a temp hack to get a version
    // that only gets for a single class until we can go through all
    // code and put them back together again.
    // This simply adds the includeInheritance property
    */
    virtual Array<CIMInstance> enumerateInstancesForClass(
        const CIMNamespaceName& nameSpace,
        const CIMName& className,
        Boolean deepInheritance = true,
        Boolean localOnly = true,
        Boolean includeQualifiers = false,
        Boolean includeClassOrigin = false,
        Boolean includeInheritance = false,
        const CIMPropertyList& propertyList = CIMPropertyList());


    /** enumerateInstanceNames returns names of instances that
        meet the critera.  This is an extension of the CIM
        enumerate instances client call
        @nameSpace - target namespace
        @param className - Name of the target class
        @param Boolean that defines whether to search inheritance or not
        @return CIMObjectPath defining the names of the found instances.
        @exception - Yes
    */
    virtual Array<CIMObjectPath> enumerateInstanceNames(
        const CIMNamespaceName& nameSpace,
        const CIMName& className);

    /** enumerateInstanceNames for a single Class. This is a temporary
    	hack and should eventually be merged with enumerateInstanceNames
	This function allows you to either include the inheritance tree
	or not with teh boolean includeInheritance.
    */
    virtual Array<CIMObjectPath> enumerateInstanceNamesForClass(
        const CIMNamespaceName& nameSpace,
        const CIMName& className,
        const Boolean includeInheritance = true	);


    /// execQuery
    virtual Array<CIMInstance> execQuery(
        const String& queryLanguage,
        const String& query) ;

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
	const ContentLanguages& contentLangs = ContentLanguages::EMPTY);

    /// getQualifier
    virtual CIMQualifierDecl getQualifier(
        const CIMNamespaceName& nameSpace,
        const CIMName& qualifierName);

    /// setQualifier
    virtual void setQualifier(
        const CIMNamespaceName& nameSpace,
        const CIMQualifierDecl& qualifierDecl,
	const ContentLanguages& contentLangs = ContentLanguages::EMPTY);

    /// deleteQualifier
    virtual void deleteQualifier(
        const CIMNamespaceName& nameSpace,
        const CIMName& qualifierName);

    /// enumerateQualifiers
    virtual Array<CIMQualifierDecl> enumerateQualifiers(
	const CIMNamespaceName& nameSpace);

    /** CIMMethod createNameSpace - Creates a new namespace in the repository
        @param String with the name of the namespace
        @exception - Throws "Already_Exists if the Namespace exits.
        Throws "CannotCreateDirectory" if there are problems in the
        creation.
    */

    virtual void createNameSpace(const CIMNamespaceName& nameSpace,
        const NameSpaceAttributes &attributes=NameSpaceAttributes());

    virtual void modifyNameSpace(const CIMNamespaceName& nameSpace,
        const NameSpaceAttributes &attributes=NameSpaceAttributes());

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

    virtual Boolean getNameSpaceAttributes(const CIMNamespaceName& nameSpace,
        NameSpaceAttributes & attributes);

    ////////////////////////////////////////////////////////////////////////////

    /** CIMMethod setDeclContext - allows the Declaration Context set
        by default in the CIMRepository constructor to be overridden.
        This is useful, for example, when a compiler wants to check syntax
        without actually adding to the repository.
    */
    void setDeclContext(RepositoryDeclContext *context);

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
        _nameSpaceManager.getSuperClassNames(nameSpaceName,
                                             className,
                                             subClassNames);
    }

    virtual Boolean isRemoteNameSpace(
        const CIMNamespaceName& nameSpaceName,
	String & remoteInfo);
	
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

    void _createAssocInstEntries(
        const CIMNamespaceName& nameSpace,
        const CIMConstClass& cimClass,
        const CIMInstance& cimInstance,
        const CIMObjectPath& instanceName);

    void _createAssocClassEntries(
        const CIMNamespaceName& nameSpace,
        const CIMConstClass& assocClass);

    /** Returns the index (or byte location) and size of the instance
        record in the instance file for a given instance.  Returns true
        if successful.  Returns false if the instance cannot be found.

        @param   nameSpace      the namespace of the instance
        @param   instanceName   the name of the instance
        @param   className      the name of the class
        @param   size           the size of the instance record found
        @param   index          the byte positon of the instance record found
        @param   searchSuper    if true, search all superclasses 
     
        @return  true           if the instance is found
                 false          if the instance cannot be found
     */
    Boolean _getInstanceIndex(
        const CIMNamespaceName& nameSpace,
        const CIMObjectPath& instanceName,
        CIMName& className,
        Uint32& size,
        Uint32& index,
        Boolean searchSuperClasses = false) const;

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

    /** Renames the temporary instance and instance index files back to the
        original files.  The temporary files were created for an insert,
        remove, or modify operation (to avoid data inconsistency between
        the two files in case of unexpected system termination or failure).
        This method is called after a successful insert, remove, or modify
        operation on BOTH the index file and the instance file.  Returns
        true on success.

        @param   indexFilePath   the file path of the instance index file
        @param   instancePath    the file path of the instance file

        @return  true      if successful
                 false     if an error occurs in removing the original files
                           or renaming the temporary files.
     */
    Boolean _renameTempInstanceAndIndexFiles(
        const String& indexFilePath,
        const String& instanceFilePath);


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
};

PEGASUS_NAMESPACE_END

#endif /* PegasusRepository_Repository_h */

