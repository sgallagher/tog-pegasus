//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 The Open group, BMC Software, Tivoli Systems, IBM
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
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef PegasusRepository_Repository_h
#define PegasusRepository_Repository_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/IPC.h>
#include <Pegasus/Common/CIMClass.h>
#include <Pegasus/Common/CIMObject.h>
#include <Pegasus/Common/CIMInstance.h>
#include <Pegasus/Common/CIMNamedInstance.h>
#include <Pegasus/Common/CIMPropertyList.h>
#include <Pegasus/Common/CIMQualifierDecl.h>
#include <Pegasus/Config/ConfigManager.h>
#include <Pegasus/Repository/NameSpaceManager.h>

PEGASUS_NAMESPACE_BEGIN

class RepositoryDeclContext;

/** This class provides a simple implementation of a CIM repository.
*/
class PEGASUS_REPOSITORY_LINKAGE CIMRepository
{
public:

    /// Constructor
    CIMRepository(const String& repositoryRoot);

    /// Descructor
    virtual ~CIMRepository();

    // Repositories MUST Have a read/write lock 

    virtual void read_lock(void) throw(IPCException);
    virtual void read_unlock(void);
    
    virtual void write_lock(void) throw(IPCException);
    virtual void write_unlock(void);
    
    /// virtual class CIMClass. From the operations class
    virtual CIMClass getClass(
        const String& nameSpace,
        const String& className,
        Boolean localOnly = true,
        Boolean includeQualifiers = true,
        Boolean includeClassOrigin = false,
        const CIMPropertyList& propertyList = CIMPropertyList());

    /// getInstance
    virtual CIMInstance getInstance(
        const String& nameSpace,
        const CIMReference& instanceName,
        Boolean localOnly = true,
        Boolean includeQualifiers = false,
        Boolean includeClassOrigin = false,
        const CIMPropertyList& propertyList = CIMPropertyList());

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
        const CIMClass& newClass);

    /// createInstance
    virtual CIMReference createInstance(
        const String& nameSpace,
        const CIMInstance& newInstance);

    /// modifyClass
    virtual void modifyClass(
        const String& nameSpace,
        const CIMClass& modifiedClass);

    /// modifyInstance
    virtual void modifyInstance(
        const String& nameSpace,
        const CIMNamedInstance& modifiedInstance,
        Boolean includeQualifiers = true,
        const CIMPropertyList& propertyList = CIMPropertyList());

    /// enumerateClasses
    virtual Array<CIMClass> enumerateClasses(
        const String& nameSpace,
        const String& className = String::EMPTY,
        Boolean deepInheritance = false,
        Boolean localOnly = true,
        Boolean includeQualifiers = true,
        Boolean includeClassOrigin = false);

    /// enumerateClassNames
    virtual Array<String> enumerateClassNames(
        const String& nameSpace,
        const String& className = String::EMPTY,
        Boolean deepInheritance = false);

    /// enumerateInstances
    virtual Array<CIMNamedInstance> enumerateInstances(
        const String& nameSpace,
        const String& className,
        Boolean deepInheritance = true,
        Boolean localOnly = true,
        Boolean includeQualifiers = false,
        Boolean includeClassOrigin = false,
        const CIMPropertyList& propertyList = CIMPropertyList());

    /// enumerateInstanceNames
    virtual Array<CIMReference> enumerateInstanceNames(
        const String& nameSpace,
        const String& className);

    /// execQuery
    virtual Array<CIMInstance> execQuery(
        const String& queryLanguage,
        const String& query) ;

    /// associators
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

    /// associateNames
    virtual Array<CIMReference> associatorNames(
        const String& nameSpace,
        const CIMReference& objectName,
        const String& assocClass = String::EMPTY,
        const String& resultClass = String::EMPTY,
        const String& role = String::EMPTY,
        const String& resultRole = String::EMPTY);

    /// references
    virtual Array<CIMObjectWithPath> references(
        const String& nameSpace,
        const CIMReference& objectName,
        const String& resultClass = String::EMPTY,
        const String& role = String::EMPTY,
        Boolean includeQualifiers = false,
        Boolean includeClassOrigin = false,
        const CIMPropertyList& propertyList = CIMPropertyList());

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

    /// invokeMethod
    virtual CIMValue invokeMethod(
        const String& nameSpace,
        const CIMReference& instanceName,
        const String& methodName,
        const Array<CIMValue>& inParameters,
        Array<CIMValue>& outParameters);

    /** CIMMethod createNameSpace - Creates a new namespace in the repository
        @param String with the name of the namespace
        @exception - Throws "Already_Exists if the Namespace exits.
        Throws "CannotCreateDirectory" if there are problems in the
        creation.
    */
    void createNameSpace(const String& nameSpace);

    /** CIMMethod enumerateNameSpaces - Get all of the namespaces in the
        repository. \Ref{NAMESPACE}
        @return Array of strings with the namespaces
    */
    virtual Array<String> enumerateNameSpaces() const;

    /** CIMMethod deleteNameSpace - Deletes a namespace in the repository.
        The deleteNameSpace method will only delete a namespace if there are
        no classed defined in the namespace.  Today this is a Pegasus
        characteristics and not defined as part of the DMTF standards.
        @param String with the name of the namespace
        @exception - Throws NoSuchDirectory if the Namespace does not exist.
    */
    void deleteNameSpace(const String& nameSpace);

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

    /** Indicates the name by which this repository explicitly registers as
        an instance provider.
    */
    const String& getProviderName()
    {
        return _providerName;
    }

    // used for enumerateInstances
     /** Get subclass names of the given class in the given namespace.
        @param nameSpaceName
        @param className - class whose subclass names will be gotten. If
            className is empty, all classnames are returned.
        @param deepInheritance - if true all descendent classes of class
            are returned. If className is empty, only root classes are returned.        @param subClassNames - output argument to hold subclass names.
        @exception CIMException(CIM_ERR_INVALID_CLASS)
    */
    void getSubClassNames(
        const String& nameSpaceName,
        const String& className,
        Boolean deepInheritance,
        Array<String>& subClassNames) const
    {
        _nameSpaceManager.getSubClassNames(nameSpaceName,
                                           className,
                                           deepInheritance,
                                           subClassNames);
    }

    /** Get the names of all superclasses (direct and indirect) of this
        class.
    */
    void getSuperClassNames(
        const String& nameSpaceName,
        const String& className,
        Array<String>& subClassNames) const
    {
        _nameSpaceManager.getSuperClassNames(nameSpaceName,
                                             className,
                                             subClassNames);
    }

private:

    void _createAssocInstEntries(
        const String& nameSpace,
        const CIMConstClass& cimClass,
        const CIMInstance& cimInstance,
        const CIMReference& instanceName);

    void _createAssocClassEntries(
        const String& nameSpace,
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
        const String& nameSpace,
        const CIMReference& instanceName,
        String& className,
        Uint32& size,
        Uint32& index,
        Boolean searchSuperClasses = false) const;

    /** Returns the file path of the instance index file.

        @param   nameSpace      the namespace of the instance
        @param   className      the name of the class

        @return  a string containing the index file path
     */
    String _getIndexFilePath(
        const String& nameSpace,
        const String& className) const;

    /** Returns the file path of the instance file.

        @param   nameSpace      the namespace of the instance
        @param   className      the name of the class

        @return  a string containing the instance file path
     */
    String _getInstanceDataFilePath(
        const String& nameSpace,
        const String& className) const;

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
        @param   namedInstances an array of CIMNamedInstance objects to which
                                the loaded instances are appended

        @return  true      if successful
                 false     if an error occurs in loading the instances
     */
    Boolean _loadAllInstances(
        const String& nameSpace,
        const String& className,
        Array<CIMNamedInstance>& namedInstances);

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

    // These must be initialized in the constructor using values from the
    // ConfigManager.
    Boolean _isDefaultInstanceProvider;
    String _providerName;

protected:
    ReadWriteSem _lock;
    RepositoryDeclContext* _context;
};

PEGASUS_NAMESPACE_END

#endif /* PegasusRepository_Repository_h */

