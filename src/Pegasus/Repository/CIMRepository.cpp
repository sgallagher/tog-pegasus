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
// Modified By: 
//         Jenny Yu, Hewlett-Packard Company (jenny_yu@hp.com)
//
// Modified By: Yi Zhou (yi_zhou@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <cctype>
#include <cstdio>
#include <fstream>
#include <Pegasus/Common/Pair.h>
#include <Pegasus/Common/Destroyer.h>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/XmlReader.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/DeclContext.h>
#include <Pegasus/Common/DeclContext.h>
#include <Pegasus/Common/System.h>
#include "CIMRepository.h"
#include "RepositoryDeclContext.h"
#include "InstanceIndexFile.h"
#include "InstanceFile.h"
#include "AssocInstTable.h"
#include "AssocClassTable.h"

#define INDENT_XML_FILES

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
//
// _LoadObject()
//
//      Loads objects (classes and qualifiers) from disk to
//      memory objects.
//
////////////////////////////////////////////////////////////////////////////////

template<class Object>
void _LoadObject(
    const String& path,
    Object& object)
{
    // Get the real path of the file:

    String realPath;

    if (!FileSystem::existsNoCase(path, realPath))
        throw CannotOpenFile(path);

    // Load file into memory:

    Array<Sint8> data;
    FileSystem::loadFileToMemory(data, realPath);
    data.append('\0');

    XmlParser parser((char*)data.getData());

    XmlReader::getObject(parser, object);
}

////////////////////////////////////////////////////////////////////////////////
//
// _SaveObject()
//
//      Saves objects (classes and qualifiers) from memory to
//      disk files.
//
////////////////////////////////////////////////////////////////////////////////

template<class Object>
void _SaveObject(const String& path, const Object& object)
{
    Array<Sint8> out;
    object.toXml(out);

    ArrayDestroyer<char> destroyer(path.allocateCString());
    PEGASUS_STD(ofstream) os(destroyer.getPointer() PEGASUS_IOS_BINARY);

    if (!os)
        throw CannotOpenFile(path);

#ifdef INDENT_XML_FILES
    out.append('\0');
    XmlWriter::indentedPrint(os, out.getData(), 2);
#else
    os.write((char*)out.getData(), out.size());
#endif
}

static String _MakeAssocInstPath(
    const String& nameSpace,
    const String& repositoryRoot)
{
    String tmp = nameSpace;
    tmp.translate('/', '#');
    return String(Cat(repositoryRoot, "/", tmp, "/instances/associations"));
}

static String _MakeAssocClassPath(
    const String& nameSpace,
    const String& repositoryRoot)
{
    String tmp = nameSpace;
    tmp.translate('/', '#');
    return String(Cat(repositoryRoot, "/", tmp, "/classes/associations"));
}

////////////////////////////////////////////////////////////////////////////////
//
// CIMRepository
//
//     The following are not implemented:
//
//         CIMRepository::execQuery()
//         CIMRepository::referencesNames()
//         CIMRepository::invokeMethod()
//
//     Note that invokeMethod() will not never implemented since it is not
//     meaningful for a repository.
//
//     ATTN: make operations on files non-case-sensitive.
//
////////////////////////////////////////////////////////////////////////////////

CIMRepository::CIMRepository(const String& repositoryRoot)
    : _repositoryRoot(repositoryRoot), _nameSpaceManager(repositoryRoot)
{
    _context = new RepositoryDeclContext(this);
}

CIMRepository::~CIMRepository()
{
    delete _context;
}

CIMClass CIMRepository::getClass(
    const String& nameSpace,
    const String& className,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const Array<String>& propertyList)
{
    // ATTN: localOnly, includeQualifiers, and includeClassOrigin are ignored
    // for now.

    String classFilePath;
    classFilePath = _nameSpaceManager.getClassFilePath(nameSpace, className);

    CIMClass cimClass;

    try
    {
        _LoadObject(classFilePath, cimClass);
    }
    catch (Exception & e)
    {
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_FOUND, className);
    }

    return cimClass;
}

//----------------------------------------------------------------------
//
// _getInstanceIndex()
//
//      Returns the index (or byte location) and size of the instance 
//      record in the instance file for a given instance.  Returns false
//      if the instance cannot be found.
//
//----------------------------------------------------------------------

Boolean CIMRepository::_getInstanceIndex(
    const String& nameSpace,
    const CIMReference& instanceName,
    String& className,
    Uint32& size,
    Uint32& index,
    Boolean searchSuperClasses) const
{
    // -- Get all descendent classes of this class:

    className = instanceName.getClassName();

    Array<String> classNames;
    _nameSpaceManager.getSubClassNames(nameSpace, className, true, classNames);
    classNames.prepend(className);

    // -- Get all superclasses of this one:

    if (searchSuperClasses)
        _nameSpaceManager.getSuperClassNames(nameSpace, className, classNames);

    // -- Get instance names from each qualifying instance file for the class

    for (Uint32 i = 0; i < classNames.size(); i++)
    {
        CIMReference tmpInstanceName = instanceName;
        tmpInstanceName.setClassName(classNames[i]);

        // -- Lookup index of instance:

        String path = _getIndexFilePath(nameSpace, classNames[i]);

        if (InstanceIndexFile::lookup(path, tmpInstanceName, size, index))
        {
            className = classNames[i];
            return true;
        }
    }

    return false;
}

CIMInstance CIMRepository::getInstance(
    const String& nameSpace,
    const CIMReference& instanceName,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const Array<String>& propertyList)
{
    // -- Get the index for this instance:

    String className;
    Uint32 index;
    Uint32 size;

    if (!_getInstanceIndex(nameSpace, instanceName, className, size, index))
    {
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_FOUND, instanceName.toString());
    }

    // -- Load the instance from file:

    String path = _getInstanceFilePath(nameSpace, className);
    CIMInstance cimInstance;
    if (!_loadInstance(path, cimInstance, index, size))
    {
        throw CannotOpenFile(path);
    }

    return cimInstance;
}

void CIMRepository::deleteClass(
    const String& nameSpace,
    const String& className)
{
    // -- Get the class and check to see if it is an association class:

    CIMClass cimClass = getClass(nameSpace, className, false);
    Boolean isAssociation = cimClass.isAssociation();

    // -- Disallow deletion if class has instances:

    String path = _getIndexFilePath(nameSpace, className);
    String realPath;

    if (FileSystem::existsNoCase(path, realPath))
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_CLASS_HAS_INSTANCES, className);

    // -- Delete the class (disallowed if there are subclasses):

    _nameSpaceManager.deleteClass(nameSpace, className);

    // -- Remove association:

    if (isAssociation)
    {
        String assocFileName = _MakeAssocClassPath(nameSpace, _repositoryRoot);

        if (FileSystem::exists(assocFileName))
            AssocClassTable::deleteAssociation(assocFileName, className);
    }
}

void CIMRepository::deleteInstance(
    const String& nameSpace,
    const CIMReference& instanceName)
{
    String errMessage;

    // -- Lookup instance from the index file:

    String indexFilePath = _getIndexFilePath(
        nameSpace, instanceName.getClassName());

    Uint32 index;
    Uint32 size;

    if (!InstanceIndexFile::lookup(indexFilePath, instanceName, size, index))
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_FOUND, instanceName.toString());

    // -- Remove entry from index file:

    if (!InstanceIndexFile::remove(indexFilePath, instanceName))
    {
        errMessage.append("Failed to delete instance ");
        errMessage.append(instanceName.toString());
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, errMessage);
    }

    // -- Remove the instance from the instance file:

    String instanceFilePath = _getInstanceFilePath(
        nameSpace, instanceName.getClassName());

    if (!InstanceFile::removeInstance(instanceFilePath, size, index))
    {
        errMessage.append("Failed to delete instance ");
        errMessage.append(instanceName.toString());
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, errMessage);
    }

    // -- Rename the temporary index and instance files back to the original:

    if (!_renameTempInstanceAndIndexFiles(indexFilePath, instanceFilePath))
    {
        errMessage.append("Unexpected error occurred while deleting instance ");
        errMessage.append(instanceName.toString());
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, errMessage);
    }

    // -- Remove it from the association table (if it is really association).
    // -- We ignore the return value intentionally. If it is an association,
    // -- true is returned. Otherwise, true is returned.

    String assocFileName = _MakeAssocInstPath(nameSpace, _repositoryRoot);

    if (FileSystem::exists(assocFileName))
        AssocInstTable::deleteAssociation(assocFileName, instanceName);
}

void CIMRepository::_createAssocClassEntries(
    const String& nameSpace,
    const CIMConstClass& assocClass)
{
    // Open input file:

    String assocFileName = _MakeAssocClassPath(nameSpace, _repositoryRoot);
    ofstream os;

    if (!OpenAppend(os, assocFileName))
        throw CannotOpenFile(assocFileName);

    // Get the association's class name:

    String assocClassName = assocClass.getClassName();

    // For each property:

    Uint32 n = assocClass.getPropertyCount();

    for (Uint32 i = 0; i < n; i++)
    {
        CIMConstProperty fromProp = assocClass.getProperty(i);

        if (fromProp.getType() == CIMType::REFERENCE)
        {
            for (Uint32 j = 0; j < n; j++)
            {
                CIMConstProperty toProp = assocClass.getProperty(j);

                if (toProp.getType() == CIMType::REFERENCE &&
                    fromProp.getName() != toProp.getName())
                {
                    String fromClassName = fromProp.getReferenceClassName();
                    String fromPropertyName = fromProp.getName();
                    String toClassName = toProp.getReferenceClassName();
                    String toPropertyName = toProp.getName();

                    AssocClassTable::append(
                        os,
                        assocClassName,
                        fromClassName,
                        fromPropertyName,
                        toClassName,
                        toPropertyName);
                }
            }
        }
    }
}

void CIMRepository::createClass(
    const String& nameSpace,
    const CIMClass& newClass)
{
    // -- Resolve the class:
        CIMClass cimClass(newClass);
        
    cimClass.resolve(_context, nameSpace);

    // -- If an association, populate associations file:

    if (cimClass.isAssociation())
        _createAssocClassEntries(nameSpace, cimClass);

    // -- Create namespace manager entry:

    String classFilePath;

    _nameSpaceManager.createClass(nameSpace, cimClass.getClassName(),
        cimClass.getSuperClassName(), classFilePath);

    // -- Create the class file:

    _SaveObject(classFilePath, cimClass);
}

/*------------------------------------------------------------------------------

    This routine does the following:

        1.  Creates two entries in the association file for each relationship
            formed by this new assocation instance. A binary association
            (one with two references) ties two instances together. Suppose
            there are two instances: I1 and I2. Then two entries are created:

                I2 -> I1
                I1 -> I2

            For a ternary relationship, six entries will be created. Suppose
            there are three instances: I1, I2, and I3:

                I1 -> I2
                I1 -> I3
                I2 -> I1
                I2 -> I3
                I3 -> I1
                I3 -> I2

            So for an N-ary relationship, there will be 2*N entries created.

        2.  Verifies that the association instance refers to real objects.
            (note that an association reference may refer to either an instance
            or a class). Throws an exception if one of the references does not
            refer to a valid object.

------------------------------------------------------------------------------*/

void CIMRepository::_createAssocInstEntries(
    const String& nameSpace,
    const CIMConstClass& cimClass,
    const CIMInstance& cimInstance,
    const CIMReference& instanceName)
{
    // Open input file:

    String assocFileName = _MakeAssocInstPath(nameSpace, _repositoryRoot);
    ofstream os;

    if (!OpenAppend(os, assocFileName))
        throw CannotOpenFile(assocFileName);

    // Get the association's instance name and class name:

    String assocInstanceName = instanceName.toString();
    String assocClassName = instanceName.getClassName();

    // For each property:

    for (Uint32 i = 0, n = cimInstance.getPropertyCount(); i < n; i++)
    {
        CIMConstProperty fromProp = cimInstance.getProperty(i);

        // If a reference property:

        if (fromProp.getType() == CIMType::REFERENCE)
        {
            // For each property:

            for (Uint32 j = 0, n = cimInstance.getPropertyCount(); j < n; j++)
            {
                CIMConstProperty toProp = cimInstance.getProperty(j);

                // If a reference property and not the same property:

                if (toProp.getType() == CIMType::REFERENCE &&
                    fromProp.getName() != toProp.getName())
                {
                    CIMReference fromRef;
                    fromProp.getValue().get(fromRef);

                    CIMReference toRef;
                    toProp.getValue().get(toRef);

                    String fromObjectName = fromRef.toString();
                    String fromClassName = fromRef.getClassName();
                    String fromPropertyName = fromProp.getName();
                    String toObjectName = toRef.toString();
                    String toClassName = toRef.getClassName();
                    String toPropertyName = toProp.getName();

                    AssocInstTable::append(
                        os,
                        assocInstanceName,
                        assocClassName,
                        fromObjectName,
                        fromClassName,
                        fromPropertyName,
                        toObjectName,
                        toClassName,
                        toPropertyName);
                }
            }
        }
    }
}

CIMReference CIMRepository::createInstance(
    const String& nameSpace,
    const CIMInstance& newInstance)
{
    String errMessage;

    // -- Resolve the instance (looks up class):
    CIMInstance cimInstance(newInstance);

    CIMConstClass cimClass;
    cimInstance.resolve(_context, nameSpace, cimClass);
    CIMReference instanceName = cimInstance.getInstanceName(cimClass);

    // -- Make sure the class has keys (otherwise it will be impossible to
    // -- create the instance.

    if (!cimClass.hasKeys())
    {
        errMessage = "class has no keys: ";
        errMessage += cimClass.getClassName();
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, errMessage);
    }

    // -- Be sure instance does not already exist:

    String className;
    Uint32 dummyIndex;
    Uint32 dummySize;

    if (_getInstanceIndex(nameSpace, instanceName, className, dummySize, 
        dummyIndex, true))
    {
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_ALREADY_EXISTS, 
            instanceName.toString());
    }

    // -- Handle if association:

    if (cimClass.isAssociation())
    {
        _createAssocInstEntries(nameSpace,
            cimClass, cimInstance, instanceName);
    }

    // -- Get instance file path:

    String instanceFilePath = _getInstanceFilePath(nameSpace,
        cimInstance.getClassName());

    // -- Save instance to file:

    Uint32 index;
    Uint32 size;
    if (!_saveInstance(instanceFilePath, cimInstance, index, size))
    {
        errMessage.append("Failed to create instance ");
        errMessage.append(instanceName.toString());
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, errMessage);
    }

    // -- Make index file entry:

    String indexFilePath = _getIndexFilePath(
        nameSpace, cimInstance.getClassName());

    if (!InstanceIndexFile::insert(indexFilePath, instanceName, size, index))
    {
        errMessage.append("Failed to create instance ");
        errMessage.append(instanceName.toString());
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, errMessage);
    }

    // -- Rename the temporary index and instance files back to the original

    if (!_renameTempInstanceAndIndexFiles(indexFilePath, instanceFilePath))
    {
        errMessage.append("Unexpected error occurred while creating instance ");
        errMessage.append(instanceName.toString());
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, errMessage);
    }
    return (instanceName);
}

void CIMRepository::modifyClass(
    const String& nameSpace,
    const CIMClass& modifiedClass)
{
    // -- Resolve the class:
        CIMClass cimClass(modifiedClass);

    cimClass.resolve(_context, nameSpace);

    // -- Check to see if it is okay to modify this class:

    String classFilePath;

    _nameSpaceManager.checkModify(nameSpace, cimClass.getClassName(),
        cimClass.getSuperClassName(), classFilePath);

    // -- Delete the old file containing the class:

    if (!FileSystem::removeFileNoCase(classFilePath))
    {
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
            "failed to remove file in CIMRepository::modifyClass()");
    }

    // -- Create new class file:

    _SaveObject(classFilePath, cimClass);
}

void CIMRepository::modifyInstance(
    const String& nameSpace,
    const CIMInstance& modifiedInstance)
{
    String errMessage;

    // -- Resolve the instance (looks up the class):
        CIMInstance cimInstance(modifiedInstance);

    CIMConstClass cimClass;
    cimInstance.resolve(_context, nameSpace, cimClass);

    // -- Lookup index of entry from index file:

    CIMReference instanceName = cimInstance.getInstanceName(cimClass);

    String indexFilePath = _getIndexFilePath(
        nameSpace, instanceName.getClassName());

    Uint32 oldSize;
    Uint32 oldIndex;
    Uint32 newSize;
    Uint32 newIndex;

    if (!InstanceIndexFile::lookup(indexFilePath, instanceName, oldSize, 
        oldIndex))
    {
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_FOUND, instanceName.toString());
    }

    // -- modify the instance file

    String instanceFilePath = _getInstanceFilePath(
        nameSpace, instanceName.getClassName());

    if (!_modifyInstance(instanceFilePath, cimInstance, oldIndex,  oldSize,
        newIndex, newSize))
    {
        errMessage.append("Failed to modify instance ");
        errMessage.append(instanceName.toString());
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, errMessage);
    }

    // -- modify the instance index file

    if (!InstanceIndexFile::modify(indexFilePath, instanceName,  newSize,
        newIndex))
    {
        errMessage.append("Failed to modify instance ");
        errMessage.append(instanceName.toString());
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, errMessage);
    }

    // -- Rename the temporary index and instance files back to the original

    if (!_renameTempInstanceAndIndexFiles(indexFilePath, instanceFilePath))
    {
        errMessage.append("Unexpected error occurred while modifying instance ");
        errMessage.append(instanceName.toString());
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, errMessage);
    }
}

Array<CIMClass> CIMRepository::enumerateClasses(
    const String& nameSpace,
    const String& className,
    Boolean deepInheritance,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin)
{
    Array<String> classNames;

    _nameSpaceManager.getSubClassNames(
        nameSpace, className, deepInheritance, classNames);

    Array<CIMClass> result;

    for (Uint32 i = 0; i < classNames.size(); i++)
    {
        result.append(getClass(nameSpace, classNames[i], localOnly,
            includeQualifiers, includeClassOrigin));
    }

    return result;
}

Array<String> CIMRepository::enumerateClassNames(
    const String& nameSpace,
    const String& className,
    Boolean deepInheritance)
{
    Array<String> classNames;

    _nameSpaceManager.getSubClassNames(
        nameSpace, className, deepInheritance, classNames);

    return classNames;
}

Array<CIMInstance> CIMRepository::enumerateInstances(
    const String& nameSpace,
    const String& className,
    Boolean deepInheritance,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const Array<String>& propertyList)
{
    // -- Get all descendent classes of this class:

    Array<String> classNames;
    _nameSpaceManager.getSubClassNames(nameSpace, className, true, classNames);
    classNames.prepend(className);

    // -- Get all instances for this class and all its descendent classes

    Array<CIMInstance> instances;

    for (Uint32 i = 0; i < classNames.size(); i++)
    {
        if (!_loadAllInstances(nameSpace, classNames[i], instances))
        {
            String errMessage = "Failed to load instances in class ";
            errMessage.append(classNames[i]);
            throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, errMessage);
        }
    }

    return instances;
}

Array<CIMReference> CIMRepository::enumerateInstanceNames(
    const String& nameSpace,
    const String& className)
{
    // -- Get all descendent classes of this class:

    Array<String> classNames;
    _nameSpaceManager.getSubClassNames(nameSpace, className, true, classNames);
    classNames.prepend(className);

    // -- Get instance names from each qualifying instance file for the class:

    Array<CIMReference> instanceNames;
    Array<Uint32> indices;
    Array<Uint32> sizes;

    for (Uint32 i = 0; i < classNames.size(); i++)
    {
        // -- Form the name of the class index file:

        String path = _getIndexFilePath(nameSpace, classNames[i]);

        // Get all instances for that class:

        if (!InstanceIndexFile::appendInstanceNamesTo(path, instanceNames, 
            indices, sizes))
        {
            String errMessage = "Failed to load instance names in class ";
            errMessage.append(classNames[i]);
            throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, errMessage);
        }
        PEGASUS_ASSERT(instanceNames.size() == indices.size());
        PEGASUS_ASSERT(instanceNames.size() == sizes.size());
    }

    return instanceNames;
}

Array<CIMInstance> CIMRepository::execQuery(
    const String& queryLanguage,
    const String& query)
{
    throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, "execQuery()");

    return Array<CIMInstance>();
}

Array<CIMObjectWithPath> CIMRepository::associators(
    const String& nameSpace,
    const CIMReference& objectName,
    const String& assocClass,
    const String& resultClass,
    const String& role,
    const String& resultRole,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const Array<String>& propertyList)
{
    Array<CIMReference> names = associatorNames(
        nameSpace,
        objectName,
        assocClass,
        resultClass,
        role,
        resultRole);

    Array<CIMObjectWithPath> result;

    for (Uint32 i = 0, n = names.size(); i < n; i++)
    {
        String tmpNameSpace = names[i].getNameSpace();

        if (tmpNameSpace.size() == 0)
            tmpNameSpace = nameSpace;

        if (names[i].isClassName())
        {
            CIMReference tmpRef = names[i];
            tmpRef.setHost(String());
            tmpRef.setNameSpace(String());

            CIMClass cimClass = getClass(
                tmpNameSpace,
                tmpRef.getClassName(),
                false,
                includeQualifiers,
                includeClassOrigin,
                propertyList);

            CIMObject cimObject(cimClass);
            result.append(CIMObjectWithPath(names[i], cimObject));
        }
        else
        {
            CIMReference tmpRef = names[i];
            tmpRef.setHost(String());
            tmpRef.setNameSpace(String());

            CIMInstance cimInstance = getInstance(
                tmpNameSpace,
                tmpRef,
                false,
                includeQualifiers,
                includeClassOrigin,
                propertyList);

            CIMObject cimObject(cimInstance);
            result.append(CIMObjectWithPath(names[i], cimObject));
        }
    }

    return result;
}

Array<CIMReference> CIMRepository::associatorNames(
    const String& nameSpace,
    const CIMReference& objectName,
    const String& assocClass,
    const String& resultClass,
    const String& role,
    const String& resultRole)
{
    Array<String> associatorNames;

    if (objectName.isClassName())
    {
        String assocFileName = _MakeAssocClassPath(nameSpace, _repositoryRoot);

        AssocClassTable::getAssociatorNames(
            assocFileName,
            objectName.toString(),
            assocClass,
            resultClass,
            role,
            resultRole,
            associatorNames);
    }
    else
    {
        String assocFileName = _MakeAssocInstPath(nameSpace, _repositoryRoot);

        AssocInstTable::getAssociatorNames(
            assocFileName,
            objectName,
            assocClass,
            resultClass,
            role,
            resultRole,
            associatorNames);
    }

    Array<CIMReference> result;

    for (Uint32 i = 0, n = associatorNames.size(); i < n; i++)
    {
        CIMReference r = associatorNames[i];

        if (r.getHost().size() == 0)
            r.setHost(System::getHostName());

        if (r.getNameSpace().size() == 0)
            r.setNameSpace(nameSpace);

        result.append(r);
    }

    return result;
}

Array<CIMObjectWithPath> CIMRepository::references(
    const String& nameSpace,
    const CIMReference& objectName,
    const String& resultClass,
    const String& role,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const Array<String>& propertyList)
{
    Array<CIMReference> names = referenceNames(
        nameSpace,
        objectName,
        resultClass,
        role);

    Array<CIMObjectWithPath> result;

    for (Uint32 i = 0, n = names.size(); i < n; i++)
    {
        String tmpNameSpace = names[i].getNameSpace();

        if (tmpNameSpace.size() == 0)
            tmpNameSpace = nameSpace;

        // ATTN: getInstance() should this be able to handle instance names
        // with host names and namespaces?

        CIMReference tmpRef = names[i];
        tmpRef.setHost(String());
        tmpRef.setNameSpace(String());

        if (objectName.isClassName())
        {
            CIMClass cimClass = getClass(
                tmpNameSpace,
                tmpRef.getClassName(),
                false,
                includeQualifiers,
                includeClassOrigin,
                propertyList);

            result.append(CIMObjectWithPath(names[i], cimClass));
        }
        else
        {
            CIMInstance instance = getInstance(
                tmpNameSpace,
                tmpRef,
                false,
                includeQualifiers,
                includeClassOrigin,
                propertyList);

            result.append(CIMObjectWithPath(names[i], instance));
        }
    }

    return result;
}

Array<CIMReference> CIMRepository::referenceNames(
    const String& nameSpace,
    const CIMReference& objectName,
    const String& resultClass,
    const String& role)
{
    Array<String> tmpReferenceNames;

    if (objectName.isClassName())
    {
        String assocFileName = _MakeAssocClassPath(nameSpace, _repositoryRoot);

        if (!AssocClassTable::getReferenceNames(
            assocFileName,
            objectName.getClassName(),
            resultClass,
            role,
            tmpReferenceNames))
        {
            // Ignore error! It's okay not to have references.
        }
    }
    else
    {
        String assocFileName = _MakeAssocInstPath(nameSpace, _repositoryRoot);

        if (!AssocInstTable::getReferenceNames(
            assocFileName,
            objectName,
            resultClass,
            role,
            tmpReferenceNames))
        {
            // Ignore error! It's okay not to have references.
        }
    }

    Array<CIMReference> result;

    for (Uint32 i = 0, n = tmpReferenceNames.size(); i < n; i++)
    {
        CIMReference r = tmpReferenceNames[i];

        if (r.getHost().size() == 0)
            r.setHost(System::getHostName());

        if (r.getNameSpace().size() == 0)
            r.setNameSpace(nameSpace);

        result.append(r);
    }

    return result;
}

CIMValue CIMRepository::getProperty(
    const String& nameSpace,
    const CIMReference& instanceName,
    const String& propertyName)
{
    // -- Get the index for this instance:

    String className;
    Uint32 index;
    Uint32 size;

    if (!_getInstanceIndex(nameSpace, instanceName, className, size, index))
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_FOUND, instanceName.toString());

    // -- Load the instance into memory:

    String path = _getInstanceFilePath(nameSpace, className);
    CIMInstance cimInstance;
    if (!_loadInstance(path, cimInstance, index, size))
    {
        throw CannotOpenFile(path);
    }

    // -- Grab the property from the instance:

    Uint32 pos = cimInstance.findProperty(propertyName);

    if (pos == PEGASUS_NOT_FOUND)
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NO_SUCH_PROPERTY, "getProperty()");

    CIMProperty prop = cimInstance.getProperty(pos);

    // -- Return the value:

    return prop.getValue();
}

void CIMRepository::setProperty(
    const String& nameSpace,
    const CIMReference& instanceName,
    const String& propertyName,
    const CIMValue& newValue)
{
    // -- Load the instance:

    CIMInstance instance = getInstance(
        nameSpace, instanceName, false, true);

    // -- Get the class:

    CIMClass cimClass = getClass(nameSpace, instance.getClassName(),
        false, true);

    // -- Save instance name:

    CIMReference oldRef = instance.getInstanceName(cimClass);

    // -- Modify the property (disallow if property is a key):

    Uint32 pos = instance.findProperty(propertyName);

    if (pos == PEGASUS_NOT_FOUND)
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NO_SUCH_PROPERTY, "setProperty()");

    CIMProperty prop = instance.getProperty(pos);

    prop.setValue(newValue);

    // -- Disallow if instance name is changed by this operation (attempt
    // -- to modify a key property.

    CIMReference newRef = instance.getInstanceName(cimClass);

    if (oldRef != newRef)
    {
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
            "setProperty(): attempted to modify a key property");
    }

    // -- Modify the instance:

    modifyInstance(nameSpace, instance);
}

CIMQualifierDecl CIMRepository::getQualifier(
    const String& nameSpace,
    const String& qualifierName)
{
    // -- Get path of qualifier file:

    String qualifierFilePath = _nameSpaceManager.getQualifierFilePath(
        nameSpace, qualifierName);

    // -- Load qualifier:

    CIMQualifierDecl qualifierDecl;

    try
    {
        _LoadObject(qualifierFilePath, qualifierDecl);
    }
    catch (CannotOpenFile&)
    {
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_FOUND, qualifierName);
    }

    return qualifierDecl;
}

void CIMRepository::setQualifier(
    const String& nameSpace,
    const CIMQualifierDecl& qualifierDecl)
{
    // -- Get path of qualifier file:

    String qualifierFilePath = _nameSpaceManager.getQualifierFilePath(
        nameSpace, qualifierDecl.getName());

    // -- If qualifier alread exists, throw exception:

    if (FileSystem::existsNoCase(qualifierFilePath))
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_ALREADY_EXISTS, qualifierDecl.getName());

    // -- Save qualifier:

    _SaveObject(qualifierFilePath, qualifierDecl);
}

void CIMRepository::deleteQualifier(
    const String& nameSpace,
    const String& qualifierName)
{
    // -- Get path of qualifier file:

    String qualifierFilePath = _nameSpaceManager.getQualifierFilePath(
        nameSpace, qualifierName);

    // -- Delete qualifier:

    if (!FileSystem::removeFileNoCase(qualifierFilePath))
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_FOUND, qualifierName);
}

Array<CIMQualifierDecl> CIMRepository::enumerateQualifiers(
    const String& nameSpace)
{
    String qualifiersRoot = _nameSpaceManager.getQualifiersRoot(nameSpace);

    Array<String> qualifierNames;

    if (!FileSystem::getDirectoryContents(qualifiersRoot, qualifierNames))
    {
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
            "enumerateQualifiers(): internal error");
    }

    Array<CIMQualifierDecl> qualifiers;

    for (Uint32 i = 0; i < qualifierNames.size(); i++)
    {
        CIMQualifierDecl qualifier = getQualifier(nameSpace, qualifierNames[i]);
        qualifiers.append(qualifier);
    }

    return qualifiers;
}

CIMValue CIMRepository::invokeMethod(
    const String& nameSpace,
    const CIMReference& instanceName,
    const String& methodName,
    const Array<CIMValue>& inParameters,
    Array<CIMValue>& outParameters)
{
    throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, "invokeMethod()");
    return CIMValue();
}

void CIMRepository::createNameSpace(const String& nameSpace)
{
    _nameSpaceManager.createNameSpace(nameSpace);
}

Array<String> CIMRepository::enumerateNameSpaces() const
{
    Array<String> nameSpaceNames;
    _nameSpaceManager.getNameSpaceNames(nameSpaceNames);
    return nameSpaceNames;
}

void CIMRepository::deleteNameSpace(const String& nameSpace)
{
    _nameSpaceManager.deleteNameSpace(nameSpace);
}

//----------------------------------------------------------------------
//
// _getIndexFilePath()
//
//      returns the file path of the instance index file. 
//
//----------------------------------------------------------------------

String CIMRepository::_getIndexFilePath(
    const String& nameSpace,
    const String& className) const
{
    String tmp = _nameSpaceManager.getInstanceFileBase(nameSpace, className);
    tmp.append(".idx");
    return tmp;
}

//----------------------------------------------------------------------
//
// _getInstanceFilePath()
//
//      returns the file path of the instance file. 
//
//----------------------------------------------------------------------

String CIMRepository::_getInstanceFilePath(
    const String& nameSpace,
    const String& className) const
{
    String tmp = _nameSpaceManager.getInstanceFileBase(nameSpace, className);
    tmp.append(".instances");
    return tmp;
}

//----------------------------------------------------------------------
//
// _loadInstance()
//
//      Loads an instance object from disk to memory.  Returns true on 
//      success.
//
//----------------------------------------------------------------------

Boolean CIMRepository::_loadInstance(
    const String& path,
    CIMInstance& object,
    Uint32 index,
    Uint32 size)
{
    // Load instance from instance file into memory:

    Array<Sint8> data;
    if (!InstanceFile::loadInstance(path, index, size, data))
    {
        return false;
    }

    XmlParser parser((char*)data.getData());

    XmlReader::getObject(parser, object);

    return true;
}

//----------------------------------------------------------------------
//
// _loadAllInstances()
//
//      Loads all the instance objects for a given class from disk to memory. 
//      Returns true on success.
//
//----------------------------------------------------------------------

Boolean CIMRepository::_loadAllInstances(
    const String& nameSpace,
    const String& className,
    Array<CIMInstance>& instances)
{
    Array<CIMReference> instanceNames;
    Array<Sint8> data;
    Array<Uint32> indices;
    Array<Uint32> sizes;

    //
    // Form the name of the instance index file
    //
    String indexFilePath = _getIndexFilePath(nameSpace, className);

    //
    // Form the name of the instance file
    //
    String instanceFilePath = _getInstanceFilePath(nameSpace, className);

    //
    // Get all instance names and record information from the index file
    //
    if (!InstanceIndexFile::appendInstanceNamesTo(
        indexFilePath, instanceNames, indices, sizes))
    {
        return false;
    }
    PEGASUS_ASSERT(instanceNames.size() == indices.size());
    PEGASUS_ASSERT(instanceNames.size() == sizes.size());
   
    //
    // Load all instance data from the instance file
    //
    if (instanceNames.size() > 0)
    {
        if (!InstanceFile::loadAllInstances(instanceFilePath, data))
        {
            return false;
        }
 
        //
        // for each instance loaded, call XML parser to parse the XML
        // data and create a CIMInstance object.
        //
        CIMInstance tmpInstance;

        Uint32 bufferSize = data.size();
        char* buffer = (char*)data.getData();

        for (Uint32 i = 0; i < instanceNames.size(); i++)
        {
            XmlParser parser(&(buffer[indices[i]]));

            XmlReader::getObject(parser, tmpInstance);

            instances.append(tmpInstance);
        }
    }

    return true;
}

//----------------------------------------------------------------------
//
// _saveInstance()
//
//      Saves an instance object from memory to disk file.  Returns true
//      on success.
//
//----------------------------------------------------------------------

Boolean CIMRepository::_saveInstance(
    const String& path,
    const CIMInstance& object,
    Uint32& index,
    Uint32& size)
{
    Array<Sint8> out;
    object.toXml(out);

    if (!InstanceFile::insertInstance(out, path, index, size))
    {
        return false;
    }

    return true;
}

//----------------------------------------------------------------------
//
// _modifyInstance()
//
//      Modifies an instance object saved in the disk file.  Returns true
//      on success.
//
//----------------------------------------------------------------------

Boolean CIMRepository::_modifyInstance(
    const String& path,
    const CIMInstance& object,
    Uint32 oldIndex,
    Uint32 oldSize,
    Uint32& newIndex,
    Uint32& newSize)
{
    Array<Sint8> out;
    object.toXml(out);

    if (!InstanceFile::modifyInstance(out, path, oldIndex, oldSize, newIndex, 
                                      newSize))
    {
        return false;
    }

    return true;
}

//------------------------------------------------------------------------------
//
// _renameTempInstanceAndIndexFiles()
//
//      Renames the temporary instance and instance index files back to the
//      original files.  The temporary files were created for an insert,
//      remove, or modify operation (to avoid data inconsistency between
//      the two files in case of unexpected system termination or failure).  
//      This method is called after a successful insert, remove, or modify
//      operation on BOTH the index file and the instance file.  Returns
//      true on success.
//
//------------------------------------------------------------------------------

Boolean CIMRepository::_renameTempInstanceAndIndexFiles(
    const String& indexFilePath, 
    const String& instanceFilePath) 
{
    //
    // Rename the original files to backup files
    // 
    // This is done so that we would not lose the original files if an error
    // occurs in renaming the temporary files back after the original files
    // have been removed.
    //
    String realIndexFilePath;
    if (FileSystem::existsNoCase(indexFilePath, realIndexFilePath))
    {
        if (!FileSystem::renameFile(realIndexFilePath, 
                                    realIndexFilePath + ".orig"))
            return false;
    }
    else
    {
        realIndexFilePath = indexFilePath;
    }

    String realInstanceFilePath;
    if (FileSystem::existsNoCase(instanceFilePath, realInstanceFilePath))
    {
        if (!FileSystem::renameFile(realInstanceFilePath, 
                                    realInstanceFilePath + ".orig"))
            return false;
    }
    else
    {
        realInstanceFilePath = instanceFilePath;
    }

    //
    // Rename the temporary instance and index files back to be the original
    // files.
    //
    // If the index file is now empty (zero size), delete the temporary 
    // files instead.
    //
    Uint32 fileSize;
    String tmpIndexFilePath = realIndexFilePath + ".tmp";
    String tmpInstanceFilePath = realInstanceFilePath + ".tmp";

    if (!FileSystem::getFileSizeNoCase(tmpIndexFilePath, fileSize))
        return false;

    if (fileSize == 0)
    {
        if (!FileSystem::removeFileNoCase(tmpIndexFilePath))
            return false;

        if (!FileSystem::removeFileNoCase(tmpInstanceFilePath))
            return false;
    }
    else
    {
        if (!FileSystem::renameFile(tmpIndexFilePath, realIndexFilePath))
            return false;

        if (!FileSystem::renameFile(tmpInstanceFilePath, realInstanceFilePath))
            return false;
    }

    //
    // Now remove the backup files 
    //
    FileSystem::removeFile(realIndexFilePath + ".orig");
    FileSystem::removeFile(realInstanceFilePath + ".orig");

    return true;
}

void CIMRepository::setDeclContext(RepositoryDeclContext *context)
{
  _context = context;
}

PEGASUS_NAMESPACE_END
