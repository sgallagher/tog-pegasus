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
// Modified By: Jenny Yu, Hewlett-Packard Company (jenny_yu@hp.com)
//              Yi Zhou, Hewlett-Packard Company (yi_zhou@hp.com)
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                  (carolann_graves@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
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
#include <Pegasus/Common/Resolver.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/PegasusVersion.h>

#include "CIMRepository.h"
#include "RepositoryDeclContext.h"
#include "InstanceIndexFile.h"
#include "InstanceDataFile.h"
#include "AssocInstTable.h"
#include "AssocClassTable.h"

#define INDENT_XML_FILES

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

static const Uint32 _MAX_FREE_COUNT = 16;

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
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::_LoadObject");

    // Get the real path of the file:

    String realPath;

    if (!FileSystem::existsNoCase(path, realPath))
    { 
        String traceString = path + " does not exist.";
        PEG_TRACE_STRING(TRC_REPOSITORY, Tracer::LEVEL4, traceString);
        PEG_METHOD_EXIT();
        throw CannotOpenFile(path);
    }

    PEG_TRACE_STRING(TRC_REPOSITORY, Tracer::LEVEL4, "realpath = " + realPath);

    // Load file into memory:

    Array<Sint8> data;
    FileSystem::loadFileToMemory(data, realPath);
    data.append('\0');

    XmlParser parser((char*)data.getData());

    XmlReader::getObject(parser, object);

    PEG_METHOD_EXIT();
}

////////////////////////////////////////////////////////////////////////////////
//
// _SaveObject()
//
//      Saves objects (classes and qualifiers) from memory to
//      disk files.
//
////////////////////////////////////////////////////////////////////////////////

void _SaveObject(const String& path, Array<Sint8>& objectXml)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::_SaveObject");

    ArrayDestroyer<char> destroyer(path.allocateCString());
    PEGASUS_STD(ofstream) os(destroyer.getPointer() PEGASUS_IOS_BINARY);

    if (!os)
    {
        PEG_METHOD_EXIT();
        throw CannotOpenFile(path);
    }

#ifdef INDENT_XML_FILES
    objectXml.append('\0');
    XmlWriter::indentedPrint(os, objectXml.getData(), 2);
#else
    os.write((char*)objectXml.getData(), objectXml.size());
#endif

    PEG_METHOD_EXIT();
}

static String _MakeAssocInstPath(
    const String& nameSpace,
    const String& repositoryRoot)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::_MakeAssocInstPath");

    String tmp = namespaceNameToDirName(nameSpace);
    String returnString(repositoryRoot);
    returnString.append('/');
    returnString.append(tmp);
    returnString.append("/instances/associations");

    PEG_METHOD_EXIT();
    return returnString;
}

static String _MakeAssocClassPath(
    const String& nameSpace,
    const String& repositoryRoot)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::_MakeAssocClassPath");

    String tmp = namespaceNameToDirName(nameSpace);
    String returnString(repositoryRoot);
    returnString.append('/');
    returnString.append(tmp);
    returnString.append("/classes/associations");

    PEG_METHOD_EXIT();
    return returnString;
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
   : _repositoryRoot(repositoryRoot), _nameSpaceManager(repositoryRoot),
     _lock(), _resolveInstance(true)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::CIMRepository");

    _context = new RepositoryDeclContext(this);
    _isDefaultInstanceProvider = (ConfigManager::getInstance()->getCurrentValue(
        "repositoryIsDefaultInstanceProvider") == "true");

    PEG_METHOD_EXIT();
}

CIMRepository::~CIMRepository()
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::~CIMRepository");

    delete _context;

    PEG_METHOD_EXIT();
}


void CIMRepository::read_lock(void) throw(IPCException)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::read_lock");

   _lock.wait_read(pegasus_thread_self());

    PEG_METHOD_EXIT();
}

void CIMRepository::read_unlock(void)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::read_unlock");

   _lock.unlock_read(pegasus_thread_self());

    PEG_METHOD_EXIT();
}

void CIMRepository::write_lock(void) throw(IPCException)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::write_lock");

   _lock.wait_write(pegasus_thread_self());

    PEG_METHOD_EXIT();
}

void CIMRepository::write_unlock(void)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::write_unlock");

   _lock.unlock_write(pegasus_thread_self());

    PEG_METHOD_EXIT();
}

CIMClass CIMRepository::getClass(
    const String& nameSpace,
    const String& className,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::getClass");

    PEG_TRACE_STRING(TRC_REPOSITORY, Tracer::LEVEL4, "nameSpace = " + 
                     nameSpace + ", className = " + className);

    // ATTN: localOnly, includeQualifiers, and includeClassOrigin are ignored
    // for now.

    String classFilePath;
    classFilePath = _nameSpaceManager.getClassFilePath(nameSpace, className);

    CIMClass cimClass;

    try
    {
        _LoadObject(classFilePath, cimClass);
    }
    catch (Exception&)
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_FOUND, className);
    }

    PEG_METHOD_EXIT();
    return cimClass;
}

Boolean CIMRepository::_getInstanceIndex(
    const String& nameSpace,
    const CIMObjectPath& instanceName,
    String& className,
    Uint32& index,
    Uint32& size,
    Boolean searchSuperClasses) const
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::_getInstanceIndex");

    //
    // Get all descendent classes of this class:
    //

    className = instanceName.getClassName();

    Array<String> classNames;
    _nameSpaceManager.getSubClassNames(nameSpace, className, true, classNames);
    classNames.prepend(className);

    //
    // Get all superclasses of this one:
    //

    if (searchSuperClasses)
        _nameSpaceManager.getSuperClassNames(nameSpace, className, classNames);

    //
    // Get instance names from each qualifying instance file for the class:
    //

    for (Uint32 i = 0; i < classNames.size(); i++)
    {
        CIMObjectPath tmpInstanceName = instanceName;
        tmpInstanceName.setClassName(classNames[i]);

	//
        // Lookup index of instance:
	//

        String path = _getInstanceIndexFilePath(nameSpace, classNames[i]);

        if (InstanceIndexFile::lookupEntry(path, tmpInstanceName, index, size))
        {
            className = classNames[i];
            PEG_METHOD_EXIT();
            return true;
        }
    }

    PEG_METHOD_EXIT();
    return false;
}

CIMInstance CIMRepository::getInstance(
    const String& nameSpace,
    const CIMObjectPath& instanceName,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::getInstance");

    //
    // Get the index for this instance:
    //

    String className;
    Uint32 index;
    Uint32 size;

    if (!_getInstanceIndex(nameSpace, instanceName, className, index, size))
    {
	PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_FOUND, instanceName.toString());
    }

    //
    // Load the instance from file:
    //

    String path = _getInstanceDataFilePath(nameSpace, className);
    CIMInstance cimInstance;

    if (!_loadInstance(path, cimInstance, index, size))
    {
	PEG_METHOD_EXIT();
        throw CannotOpenFile(path);
    }

    //
    // Resolve the instance (if requested):
    //

    if (_resolveInstance)
    {
	CIMConstClass cimClass;
	Resolver::resolveInstance (cimInstance, _context, nameSpace, cimClass, 
            true);
    }

    PEG_METHOD_EXIT();
    return cimInstance;
}

void CIMRepository::deleteClass(
    const String& nameSpace,
    const String& className)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY,"CIMRepository::deleteClass");

    //
    // Get the class and check to see if it is an association class:
    //

    CIMClass cimClass = getClass(nameSpace, className, false);
    Boolean isAssociation = cimClass.isAssociation();

    //
    // Disallow deletion if class has instances:
    //

    String indexFilePath = _getInstanceIndexFilePath(nameSpace, className);
    PEG_TRACE_STRING(TRC_REPOSITORY, Tracer::LEVEL4, 
                     "instance indexFilePath = " + indexFilePath);

    String dataFilePath = _getInstanceDataFilePath(nameSpace, className);
    PEG_TRACE_STRING(TRC_REPOSITORY, Tracer::LEVEL4, 
                     "instance dataFilePath = " + dataFilePath);

    if (InstanceIndexFile::hasNonFreeEntries(indexFilePath))
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_CLASS_HAS_INSTANCES, className);
    }

    //
    // Delete the class. The NameSpaceManager::deleteClass() method throws
    // and exception if the class has sublclasses.
    //
    try
    {
        _nameSpaceManager.deleteClass(nameSpace, className);
    }
    catch (CIMException& e)
    {
        PEG_METHOD_EXIT();
        throw e;
    }

    FileSystem::removeFileNoCase(indexFilePath);

    FileSystem::removeFileNoCase(dataFilePath);

    //
    // Kill off empty instance files:
    //

    //
    // Remove association:
    //

    if (isAssociation)
    {
        String assocFileName = _MakeAssocClassPath(nameSpace, _repositoryRoot);

        if (FileSystem::exists(assocFileName))
            AssocClassTable::deleteAssociation(assocFileName, className);
    }
    
    PEG_METHOD_EXIT();
}

void _CompactInstanceRepository(
    const String& indexFilePath, 
    const String& dataFilePath)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::_CompactInstanceRepository");

    //
    // Compact the data file first:
    //

    Array<Uint32> freeFlags;
    Array<Uint32> indices;
    Array<Uint32> sizes;
    Array<CIMObjectPath> instanceNames;

    if (!InstanceIndexFile::enumerateEntries(
	indexFilePath, freeFlags, indices, sizes, instanceNames, true))
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "compact failed");
    }

    if (!InstanceDataFile::compact(dataFilePath, freeFlags, indices, sizes))
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "compact failed");
    }

    //
    // Now compact the index file:
    //

    if (!InstanceIndexFile::compact(indexFilePath))
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "compact failed");
    }

    PEG_METHOD_EXIT();
}

void CIMRepository::deleteInstance(
    const String& nameSpace,
    const CIMObjectPath& instanceName)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::deleteInstance");

    String errMessage;

    //
    // Get paths of index and data files:
    //

    String indexFilePath = _getInstanceIndexFilePath(
        nameSpace, instanceName.getClassName());

    String dataFilePath = _getInstanceDataFilePath(
        nameSpace, instanceName.getClassName());

    //
    // Attempt rollback (if there are no rollback files, this will have no 
    // effect). This code is here to rollback uncommitted changes left over 
    // from last time an instance-oriented function was called.
    //

    if (!InstanceIndexFile::rollbackTransaction(indexFilePath))
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "rollback failed");
    }

    if (!InstanceDataFile::rollbackTransaction(dataFilePath))
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "rollback failed");
    }

    //
    // Lookup instance from the index file (raise error if not found).
    //

    Uint32 index;
    Uint32 size;

    if (!InstanceIndexFile::lookupEntry(
	indexFilePath, instanceName, index, size))
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_FOUND, instanceName.toString());
    }

    //
    // Begin the transaction (any return prior to commit will cause
    // a rollback next time an instance-oriented routine is invoked).
    //

    if (!InstanceIndexFile::beginTransaction(indexFilePath))
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "begin failed");
    }

    if (!InstanceDataFile::beginTransaction(dataFilePath))
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "begin failed");
    }

    //
    // Remove entry from index file.
    //

    Uint32 freeCount;

    if (!InstanceIndexFile::deleteEntry(indexFilePath, instanceName, freeCount))
    {
        errMessage.append("Failed to delete instance: ");
        errMessage.append(instanceName.toString());
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, errMessage);
    }

    //
    // Commit the transaction:
    //

    if (!InstanceIndexFile::commitTransaction(indexFilePath))
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "commit failed");
    }

    if (!InstanceDataFile::commitTransaction(dataFilePath))
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "commit failed");
    }

    //
    // Compact the index and data files if the free count max was
    // reached.
    //

    if (freeCount == _MAX_FREE_COUNT)
	_CompactInstanceRepository(indexFilePath, dataFilePath);

    //
    // Delete from assocation table (if an assocation).
    //

    String assocFileName = _MakeAssocInstPath(nameSpace, _repositoryRoot);

    if (FileSystem::exists(assocFileName))
        AssocInstTable::deleteAssociation(assocFileName, instanceName);

    PEG_METHOD_EXIT();
}

void CIMRepository::_createAssocClassEntries(
    const String& nameSpace,
    const CIMConstClass& assocClass)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::_createAssocClassEntries");

    // Open input file:


    String assocFileName = _MakeAssocClassPath(nameSpace, _repositoryRoot);
    ofstream os;

    if (!OpenAppend(os, assocFileName))
    {
        PEG_METHOD_EXIT();
        throw CannotOpenFile(assocFileName);
    }

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

    PEG_METHOD_EXIT();
}

void CIMRepository::createClass(
    const String& nameSpace,
    const CIMClass& newClass)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::createClass");

    // -- Resolve the class:
        CIMClass cimClass(newClass);
        
    Resolver::resolveClass (cimClass, _context, nameSpace);

    // -- If an association, populate associations file:

    if (cimClass.isAssociation())
        _createAssocClassEntries(nameSpace, cimClass);

    // -- Create namespace manager entry:

    String classFilePath;

    _nameSpaceManager.createClass(nameSpace, cimClass.getClassName(),
        cimClass.getSuperClassName(), classFilePath);

    // -- Create the class file:

    Array<Sint8> classXml;
    XmlWriter::appendClassElement(classXml, cimClass);
    _SaveObject(classFilePath, classXml);

    PEG_METHOD_EXIT();
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
    const CIMObjectPath& instanceName)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::_createAssocInstEntries");

    // Open input file:

    String assocFileName = _MakeAssocInstPath(nameSpace, _repositoryRoot);
    ofstream os;

    if (!OpenAppend(os, assocFileName))
    {
        PEG_METHOD_EXIT();
        throw CannotOpenFile(assocFileName);
    }

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
                    CIMObjectPath fromRef;
                    fromProp.getValue().get(fromRef);

                    CIMObjectPath toRef;
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

    PEG_METHOD_EXIT();
}

CIMObjectPath CIMRepository::createInstance(
    const String& nameSpace,
    const CIMInstance& newInstance)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::createInstance");

    String errMessage;

    //
    // Get paths to data and index files:
    //

    String dataFilePath = _getInstanceDataFilePath(
	nameSpace, newInstance.getClassName());

    String indexFilePath = _getInstanceIndexFilePath(
        nameSpace, newInstance.getClassName());

    //
    // Attempt rollback (if there are no rollback files, this will have no 
    // effect). This code is here to rollback uncommitted changes left over 
    // from last time an instance-oriented function was called.
    //

    if (!InstanceIndexFile::rollbackTransaction(indexFilePath))
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "rollback failed");
    }

    if (!InstanceDataFile::rollbackTransaction(dataFilePath))
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "rollback failed");
    }

    //
    // Resolve the instance. Looks up class and fills out properties but
    // not the qualifiers.
    //

    CIMInstance cimInstance(newInstance);
    CIMConstClass cimClass;
    Resolver::resolveInstance (cimInstance, _context, nameSpace, cimClass, 
        false);
    CIMObjectPath instanceName = cimInstance.getInstanceName(cimClass);

    //
    // Make sure the class has keys (otherwise it will be impossible to
    // create the instance).
    //

    if (!cimClass.hasKeys())
    {
        errMessage = "class has no keys: ";
        errMessage += cimClass.getClassName();
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, errMessage);
    }

    //
    // Be sure instance does not already exist:
    //

    String className;
    Uint32 dummyIndex;
    Uint32 dummySize;

    if (_getInstanceIndex(nameSpace, instanceName, className, dummyIndex, 
        dummySize, true))
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_ALREADY_EXISTS, 
            instanceName.toString());
    }

    //
    // Create association entries if an association instance.
    //

    if (cimClass.isAssociation())
        _createAssocInstEntries(nameSpace, cimClass, cimInstance, instanceName);

    //
    // Begin the transaction (any return prior to commit will cause
    // a rollback next time an instance-oriented routine is invoked).
    //

    if (!InstanceIndexFile::beginTransaction(indexFilePath))
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "begin failed");
    }

    if (!InstanceDataFile::beginTransaction(dataFilePath))
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "begin failed");
    }

    //
    // Save instance to file:
    //

    Uint32 index;
    Uint32 size;

    {
	Array<Sint8> data;
	XmlWriter::appendInstanceElement(data, cimInstance);
	size = data.size();

	if (!InstanceDataFile::appendInstance(dataFilePath, data, index))
	{
	    errMessage.append("Failed to create instance: ");
	    errMessage.append(instanceName.toString());
	    PEG_METHOD_EXIT();
	    throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, errMessage);
	}
    }

    //
    // Create entry in index file:
    //

    if (!InstanceIndexFile::createEntry(
	indexFilePath, instanceName, index, size))
    {
        errMessage.append("Failed to create instance: ");
        errMessage.append(instanceName.toString());
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, errMessage);
    }

    //
    // Commit the changes:
    //

    if (!InstanceIndexFile::commitTransaction(indexFilePath))
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "commit failed");
    }

    if (!InstanceDataFile::commitTransaction(dataFilePath))
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "commit failed");
    }

    Resolver::resolveInstance (cimInstance, _context, nameSpace, cimClass, 
        true);

    PEG_METHOD_EXIT();
    return instanceName;
}

void CIMRepository::modifyClass(
    const String& nameSpace,
    const CIMClass& modifiedClass)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::modifyClass");

    //
    // Resolve the class:
    //

    CIMClass cimClass(modifiedClass);
    Resolver::resolveClass (cimClass, _context, nameSpace);

    //
    // Check to see if it is okay to modify this class:
    //

    String classFilePath;

    _nameSpaceManager.checkModify(nameSpace, cimClass.getClassName(),
        cimClass.getSuperClassName(), classFilePath);

    //
    // ATTN: KS
    // Disallow modification of classes which have instances (that are
    // in the repository). And we have no idea whether the class has
    // instances in other repositories or in providers. We should do
    // an enumerate instance names at a higher level (above the repository).
    //

    //
    // Delete the old file containing the class:
    //

    if (!FileSystem::removeFileNoCase(classFilePath))
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
            "failed to remove file in CIMRepository::modifyClass()");
    }

    //
    // Create new class file:
    //

    Array<Sint8> classXml;
    XmlWriter::appendClassElement(classXml, cimClass);
    _SaveObject(classFilePath, classXml);

    PEG_METHOD_EXIT();
}

void CIMRepository::modifyInstance(
    const String& nameSpace,
    const CIMInstance& modifiedInstance,
    Boolean includeQualifiers,
    const CIMPropertyList& propertyList)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::modifyInstance");

    //
    // Get paths of index and data files:
    //

    const CIMInstance& instance = modifiedInstance;

    String indexFilePath = _getInstanceIndexFilePath(
        nameSpace, instance.getClassName());

    String dataFilePath = _getInstanceDataFilePath(
        nameSpace, instance.getClassName());

    //
    // First attempt rollback:
    //

    if (!InstanceIndexFile::rollbackTransaction(indexFilePath))
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "rollback failed");
    }

    if (!InstanceDataFile::rollbackTransaction(dataFilePath))
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "rollback failed");
    }

    //
    // Begin the transaction:
    //

    if (!InstanceIndexFile::beginTransaction(indexFilePath))
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "begin failed");
    }

    if (!InstanceDataFile::beginTransaction(dataFilePath))
    { 
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "begin failed");
    }

    //
    // Do this:
    //

    String errMessage;
    CIMInstance cimInstance;   // The instance that replaces the original

    if (propertyList.isNull())
    {
        //
        // Replace all the properties in the instance
        //
        if (includeQualifiers)
        {
            //
            // Replace the entire instance with the given instance
            // (this is the default behavior)
            //
            cimInstance = modifiedInstance;
        }
        else
        {
            //
            // Replace all the properties in the instance, but keep the
            // original qualifiers on the instance and on the properties
            //

	    _resolveInstance = false;

            cimInstance = getInstance(
		nameSpace,
                modifiedInstance.getPath (), 
		false, 
		true, 
		true);

	    _resolveInstance = true;

            CIMInstance newInstance(
                modifiedInstance.getPath ().getClassName());

            CIMInstance givenInstance = modifiedInstance;

            //
            // Copy over the original instance qualifiers
            //

            for (Uint32 i = 0; i < cimInstance.getQualifierCount(); i++)
            {
                newInstance.addQualifier(cimInstance.getQualifier(i));
            }

            //
            // Loop through the properties replacing each property in the
            // original with a new value, but keeping the original qualifiers
            //
            for (Uint32 i=0; i<givenInstance.getPropertyCount(); i++)
            {
                // Copy the given property value (not qualifiers)
                CIMProperty givenProperty = givenInstance.getProperty(i);
                CIMProperty newProperty(
                    givenProperty.getName(),
                    givenProperty.getValue(),
                    givenProperty.getArraySize(),
                    givenProperty.getReferenceClassName(),
                    givenProperty.getClassOrigin(),
                    givenProperty.getPropagated());

                // Copy the original property qualifiers
                Uint32 origPos =
                    cimInstance.findProperty(newProperty.getName());
                if (origPos != PEG_NOT_FOUND)
                {
                    CIMProperty origProperty = cimInstance.getProperty(origPos);
                    for (Uint32 j=0; j<origProperty.getQualifierCount(); j++)
                    {
                        newProperty.addQualifier(origProperty.getQualifier(i));
                    }
                }

                // Add the newly constructed property to the new instance
                newInstance.addProperty(newProperty);
            }

            // Use the newly merged instance to replace the original instance
            cimInstance = newInstance;
        }
    }
    else
    {
        //
        // Replace only the properties specified in the given instance
        //

	_resolveInstance = false;

        cimInstance = getInstance(nameSpace,
            modifiedInstance.getPath (), false, true, true);

	_resolveInstance = true;

        CIMInstance givenInstance = modifiedInstance;

        // NOTE: Instance qualifiers are not changed when a property list
        // is specified.  Property qualifiers are replaced with the
        // corresponding property values.

        //
        // Loop through the propertyList replacing each property in the original
        //

        for (Uint32 i=0; i<propertyList.size(); i++)
        {
            Uint32 origPropPos = cimInstance.findProperty(propertyList[i]);
            if (origPropPos != PEG_NOT_FOUND)
            {
                // Case: Property set in original
                CIMProperty origProperty =
                    cimInstance.getProperty(origPropPos);

                // Get the given property value
                Uint32 givenPropPos =
                    givenInstance.findProperty(propertyList[i]);
                if (givenPropPos != PEG_NOT_FOUND)
                {
                    // Case: Property set in original and given
                    CIMProperty givenProperty =
                        givenInstance.getProperty(givenPropPos);

                    // Copy over the property from the given to the original
                    if (includeQualifiers)
                    {
                        // Case: Total property replacement
                        cimInstance.removeProperty(origPropPos);
                        cimInstance.addProperty(givenProperty);
                    }
                    else
                    {
                        // Case: Replace only the property value (not quals)
                        origProperty.setValue(givenProperty.getValue());
                        cimInstance.removeProperty(origPropPos);
                        cimInstance.addProperty(origProperty);
                    }
                }
                else
                {
                    // Case: Property set in original and not in given
                    // Just remove the property (set to null)
                    cimInstance.removeProperty(origPropPos);
                }
            }
            else
            {
                // Case: Property not set in original

                // Get the given property value
                Uint32 givenPropPos =
                    givenInstance.findProperty(propertyList[i]);
                if (givenPropPos != PEG_NOT_FOUND)
                {
                    // Case: Property set in given and not in original
                    CIMProperty givenProperty =
                        givenInstance.getProperty(givenPropPos);

                    // Copy over the property from the given to the original
                    if (includeQualifiers)
                    {
                        // Case: Total property copy
                        cimInstance.addProperty(givenProperty);
                    }
                    else
                    {
                        // Case: Copy only the property value (not qualifiers)
                        CIMProperty newProperty(
                            givenProperty.getName(),
                            givenProperty.getValue(),
                            givenProperty.getArraySize(),
                            givenProperty.getReferenceClassName(),
                            givenProperty.getClassOrigin(),
                            givenProperty.getPropagated());
                        cimInstance.addProperty(newProperty);
                    }
                }
                else
                {
                    // Case: Property not set in original or in given

                    // Nothing to do; just make sure the property name is valid
                    // ATTN: This is not the most efficient solution
                    CIMClass cimClass = getClass(
                        nameSpace, cimInstance.getClassName(), false);
                    if (cimClass.findProperty(propertyList[i]) == PEG_NOT_FOUND)
                    {
                        // ATTN: This exception may be returned by setProperty
                        PEG_METHOD_EXIT();
                        throw PEGASUS_CIM_EXCEPTION(
                            CIM_ERR_NO_SUCH_PROPERTY, "modifyInstance()");
                    }
                }
            }
        }
    }

    //
    // Resolve the instance (do not propagate qualifiers from class since
    // this will bloat the instance).
    //

    CIMConstClass cimClass;
    Resolver::resolveInstance (cimInstance, _context, nameSpace, cimClass, 
        false);

    CIMObjectPath instanceName = cimInstance.getInstanceName(cimClass);

    //
    // Disallow operation if the instance name was changed:
    //

    if (instanceName != modifiedInstance.getPath ())
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
            "Attempted to modify a key property");
    }

    Uint32 oldSize;
    Uint32 oldIndex;
    Uint32 newSize;
    Uint32 newIndex;

    if (!InstanceIndexFile::lookupEntry(
	indexFilePath, instanceName, oldIndex, oldSize))
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_FOUND, instanceName.toString());
    }

    //
    // Modify the data file:
    //

    {
	Array<Sint8> out;
	XmlWriter::appendInstanceElement(out, cimInstance);

	newSize = out.size();

	if (!InstanceDataFile::appendInstance(dataFilePath, out, newIndex))
	{
	    errMessage.append("Failed to modify instance ");
	    errMessage.append(instanceName.toString());
            PEG_METHOD_EXIT();
	    throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, errMessage);
	}
    }

    //
    // Modify the index file:
    //

    Uint32 freeCount;

    if (!InstanceIndexFile::modifyEntry(indexFilePath, instanceName, newIndex,
        newSize, freeCount))
    {
        errMessage.append("Failed to modify instance ");
        errMessage.append(instanceName.toString());
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, errMessage);
    }

    //
    // Commit the transaction:
    //

    if (!InstanceIndexFile::commitTransaction(indexFilePath))
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "commit failed");
    }

    if (!InstanceDataFile::commitTransaction(dataFilePath))
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "commit failed");
    }

    //
    // Compact the index and data files if the free count max was
    // reached.
    //

    if (freeCount == _MAX_FREE_COUNT)
	_CompactInstanceRepository(indexFilePath, dataFilePath);

    //
    // Resolve the instance:
    //

    Resolver::resolveInstance (cimInstance, _context, nameSpace, cimClass, 
        true);

    PEG_METHOD_EXIT();
}

Array<CIMClass> CIMRepository::enumerateClasses(
    const String& nameSpace,
    const String& className,
    Boolean deepInheritance,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::enumerateClasses");

    Array<String> classNames;

    _nameSpaceManager.getSubClassNames(
        nameSpace, className, deepInheritance, classNames);

    Array<CIMClass> result;

    for (Uint32 i = 0; i < classNames.size(); i++)
    {
        result.append(getClass(nameSpace, classNames[i], localOnly,
            includeQualifiers, includeClassOrigin));
    }

    PEG_METHOD_EXIT();
    return result;
}

Array<String> CIMRepository::enumerateClassNames(
    const String& nameSpace,
    const String& className,
    Boolean deepInheritance)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::enumerateClassNames");

    Array<String> classNames;

    _nameSpaceManager.getSubClassNames(
        nameSpace, className, deepInheritance, classNames);

    PEG_METHOD_EXIT();
    return classNames;
}

Boolean CIMRepository::_loadAllInstances(
    const String& nameSpace,
    const String& className,
    Array<CIMInstance>& namedInstances)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::_loadAllInstances");

    Array<CIMObjectPath> instanceNames;
    Array<Sint8> data;
    Array<Uint32> indices;
    Array<Uint32> sizes;

    //
    // Form the name of the instance index file
    //

    String indexFilePath = _getInstanceIndexFilePath(nameSpace, className);

    //
    // Form the name of the instance file
    //

    String dataFilePath = _getInstanceDataFilePath(nameSpace, className);

    //
    // Enumerate the index file:
    //

    Array<Uint32> freeFlags;

    if (!InstanceIndexFile::enumerateEntries(
        indexFilePath, freeFlags, indices, sizes, instanceNames, true))
    {
        PEG_METHOD_EXIT();
        return false;
    }

    //
    // Form the array of instances result:
    //

    if (instanceNames.size() > 0)
    {
	//
	// Load all instances from the data file:
	//

        if (!InstanceDataFile::loadAllInstances(dataFilePath, data))
        {
            PEG_METHOD_EXIT();
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
	    if (!freeFlags[i])
	    {
		XmlParser parser(&(buffer[indices[i]]));

		XmlReader::getObject(parser, tmpInstance);

		Resolver::resolveInstance (tmpInstance, _context, nameSpace, 
                    true);
                tmpInstance.setPath (instanceNames[i]);

		namedInstances.append (tmpInstance);
	    }
        }
    }

    PEG_METHOD_EXIT();
    return true;
}

Array<CIMInstance> CIMRepository::enumerateInstances(
    const String& nameSpace,
    const String& className,
    Boolean deepInheritance,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::enumerateInstances");
    //
    // Get all descendent classes of this class:
    //

    Array<String> classNames;
    //ATTN: Should this be protected with a try block KS 15 May 2002 P0
    _nameSpaceManager.getSubClassNames(nameSpace, className, true, classNames);
    classNames.prepend(className);

    //
    // Get all instances for this class and all its descendent classes
    //

    Array<CIMInstance> namedInstances;
    
    for (Uint32 i = 0; i < classNames.size(); i++)
    {
        if (!_loadAllInstances(nameSpace, classNames[i], namedInstances))
        {
            String errMessage = "Failed to load instances in class ";
            errMessage.append(classNames[i]);
            PEG_METHOD_EXIT();
            throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, errMessage);
        }
    }
    
    PEG_METHOD_EXIT();
    return namedInstances;
}

Array<CIMInstance> CIMRepository::enumerateInstancesForClass(
    const String& nameSpace,
    const String& className,
    Boolean deepInheritance,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    Boolean includeInheritance,
    const CIMPropertyList& propertyList)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::enumerateInstances");
    //
    // Get all descendent classes of this class:
    //

    Array<String> classNames;
    // If includeInheritance is true, get all subclasses.
    // ATTN: P3 KS Look at whether the subclassNames requires an empty array.
    if(includeInheritance)
    {
	try
	{
	    _nameSpaceManager.getSubClassNames(nameSpace, className, true, classNames);
	}
	catch(CIMException& e)
	{
	    PEG_METHOD_EXIT();
	    throw e;
	}
    }
    classNames.prepend(className);

    //
    // Get all instances for this class and all its descendent classes
    //

    Array<CIMInstance> namedInstances;
    
    for (Uint32 i = 0; i < classNames.size(); i++)
    {
        if (!_loadAllInstances(nameSpace, classNames[i], namedInstances))
        {
            String errMessage = "Failed to load instances in class ";
            errMessage.append(classNames[i]);
            PEG_METHOD_EXIT();
            throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, errMessage);
        }
    }
    
    PEG_METHOD_EXIT();
    return namedInstances;
}
Array<CIMObjectPath> CIMRepository::enumerateInstanceNames(
    const String& nameSpace,
    const String& className)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::enumerateInstanceNames");

    //
    // Get names of descendent classes:
    //
    Array<String> classNames;

    try
    {
	_nameSpaceManager.getSubClassNames(nameSpace, className, true, classNames);
    }
    catch(CIMException& e)
    {
	PEG_METHOD_EXIT();
	throw e;
    }
    classNames.prepend(className);

    //
    // Get instance names from each qualifying instance file for the class:
    //
    Array<CIMObjectPath> instanceNames;
    Array<Uint32> indices;
    Array<Uint32> sizes;

    for (Uint32 i = 0; i < classNames.size(); i++)
    {
	//
        // Form the name of the class index file:
	//

        String indexFilePath = _getInstanceIndexFilePath(
	    nameSpace, classNames[i]);

	//
        // Get all instances for that class:
	//

	Array<Uint32> freeFlags;

	if (!InstanceIndexFile::enumerateEntries(
	    indexFilePath, freeFlags, indices, sizes, instanceNames, false))
        {
            String errMessage = "Failed to load instance names in class ";
	    errMessage.append(classNames[i]);
            PEG_METHOD_EXIT();
            throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, errMessage);
        }
    }

    PEG_METHOD_EXIT();
    return instanceNames;
}

Array<CIMObjectPath> CIMRepository::enumerateInstanceNamesForClass(
    const String& nameSpace,
    const String& className,
    Boolean includeInheritance)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::enumerateInstanceNames");

    //
    // Get names of descendent classes:
    //
    Array<String> classNames;

    // If includeInheritance is true, get all subclasses.
    if(includeInheritance)
    {
	try
	{
	    _nameSpaceManager.getSubClassNames(nameSpace, className, true, classNames);
	}
	catch(CIMException& e)
	{
	    PEG_METHOD_EXIT();
	    throw e;
	}
    }
    classNames.prepend(className);

    //
    // Get instance names from each qualifying instance file for the class:
    //
    Array<CIMObjectPath> instanceNames;
    Array<Uint32> indices;
    Array<Uint32> sizes;

    for (Uint32 i = 0; i < classNames.size(); i++)
    {
	//
        // Form the name of the class index file:
	//

        String indexFilePath = _getInstanceIndexFilePath(
	    nameSpace, classNames[i]);

	//
        // Get all instances for that class:
	//

	Array<Uint32> freeFlags;

	if (!InstanceIndexFile::enumerateEntries(
	    indexFilePath, freeFlags, indices, sizes, instanceNames, false))
        {
            String errMessage = "Failed to load instance names in class ";
	    errMessage.append(classNames[i]);
            PEG_METHOD_EXIT();
            throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, errMessage);
        }
    }

    PEG_METHOD_EXIT();
    return instanceNames;
}


Array<CIMInstance> CIMRepository::execQuery(
    const String& queryLanguage,
    const String& query)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::execQuery");

    PEG_METHOD_EXIT();
    throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, "execQuery()");

    PEG_METHOD_EXIT();
    return Array<CIMInstance>();
}

Array<CIMObject> CIMRepository::associators(
    const String& nameSpace,
    const CIMObjectPath& objectName,
    const String& assocClass,
    const String& resultClass,
    const String& role,
    const String& resultRole,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::associators");

    Array<CIMObjectPath> names = associatorNames(
        nameSpace,
        objectName,
        assocClass,
        resultClass,
        role,
        resultRole);

    Array<CIMObject> result;

    for (Uint32 i = 0, n = names.size(); i < n; i++)
    {
        String tmpNameSpace = names[i].getNameSpace();

        if (tmpNameSpace.size() == 0)
            tmpNameSpace = nameSpace;

        if (names[i].isClassName())
        {
            CIMObjectPath tmpRef = names[i];
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
            cimObject.setPath (names[i]);
            result.append(cimObject);
        }
        else
        {
            CIMObjectPath tmpRef = names[i];
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
            cimObject.setPath (names[i]);
            result.append(cimObject);
        }
    }

    PEG_METHOD_EXIT();
    return result;
}

Array<CIMObjectPath> CIMRepository::associatorNames(
    const String& nameSpace,
    const CIMObjectPath& objectName,
    const String& assocClass,
    const String& resultClass,
    const String& role,
    const String& resultRole)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::associatorNames");

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

    Array<CIMObjectPath> result;

    for (Uint32 i = 0, n = associatorNames.size(); i < n; i++)
    {
        CIMObjectPath r = associatorNames[i];

        if (r.getHost().size() == 0)
            r.setHost(System::getHostName());

        if (r.getNameSpace().size() == 0)
            r.setNameSpace(nameSpace);

        result.append(r);
    }

    PEG_METHOD_EXIT();
    return result;
}

Array<CIMObject> CIMRepository::references(
    const String& nameSpace,
    const CIMObjectPath& objectName,
    const String& resultClass,
    const String& role,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::references");

    Array<CIMObjectPath> names = referenceNames(
        nameSpace,
        objectName,
        resultClass,
        role);

    Array<CIMObject> result;

    for (Uint32 i = 0, n = names.size(); i < n; i++)
    {
        String tmpNameSpace = names[i].getNameSpace();

        if (tmpNameSpace.size() == 0)
            tmpNameSpace = nameSpace;

        // ATTN: getInstance() should this be able to handle instance names
        // with host names and namespaces?

        CIMObjectPath tmpRef = names[i];
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

            CIMObject cimObject = CIMObject (cimClass);
            cimObject.setPath (names[i]);
            result.append (cimObject);
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

            CIMObject cimObject = CIMObject (instance);
            cimObject.setPath (names[i]);
            result.append (cimObject);
        }
    }

    PEG_METHOD_EXIT();
    return result;
}

Array<CIMObjectPath> CIMRepository::referenceNames(
    const String& nameSpace,
    const CIMObjectPath& objectName,
    const String& resultClass,
    const String& role)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::referenceNames");

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

    Array<CIMObjectPath> result;

    for (Uint32 i = 0, n = tmpReferenceNames.size(); i < n; i++)
    {
        CIMObjectPath r = tmpReferenceNames[i];

        if (r.getHost().size() == 0)
            r.setHost(System::getHostName());

        if (r.getNameSpace().size() == 0)
            r.setNameSpace(nameSpace);

        result.append(r);
    }

    PEG_METHOD_EXIT();
    return result;
}

CIMValue CIMRepository::getProperty(
    const String& nameSpace,
    const CIMObjectPath& instanceName,
    const String& propertyName)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::getProperty");

    //
    // Get the index for this instance:
    //

    String className;
    Uint32 index;
    Uint32 size;

    if (!_getInstanceIndex(nameSpace, instanceName, className, index, size))
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_FOUND, instanceName.toString());
    }

    //
    // Load the instance into memory:
    //

    String path = _getInstanceDataFilePath(nameSpace, className);
    CIMInstance cimInstance;

    if (!_loadInstance(path, cimInstance, index, size))
    {
        PEG_METHOD_EXIT();
        throw CannotOpenFile(path);
    }

    //
    // Grab the property from the instance:
    //

    Uint32 pos = cimInstance.findProperty(propertyName);

    // ATTN: This breaks if the property is simply null
    if (pos == PEGASUS_NOT_FOUND)
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NO_SUCH_PROPERTY, "getProperty()");
    }

    CIMProperty prop = cimInstance.getProperty(pos);

    //
    // Return the value:
    //

    PEG_METHOD_EXIT();
    return prop.getValue();
}

void CIMRepository::setProperty(
    const String& nameSpace,
    const CIMObjectPath& instanceName,
    const String& propertyName,
    const CIMValue& newValue)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::setProperty");

    //
    // Create the instance to pass to modifyInstance()
    //

    CIMInstance instance(instanceName.getClassName());
    instance.addProperty(CIMProperty(propertyName, newValue));
    instance.setPath (instanceName);

    //
    // Create the propertyList to pass to modifyInstance()
    //

    Array<String> propertyListArray;
    propertyListArray.append(propertyName);
    CIMPropertyList propertyList(propertyListArray);

    //
    // Modify the instance to set the value of the given property
    //
    modifyInstance(nameSpace, instance, false, propertyList);

    PEG_METHOD_EXIT();
}

CIMQualifierDecl CIMRepository::getQualifier(
    const String& nameSpace,
    const String& qualifierName)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::getQualifier");

    //
    // Get path of qualifier file:
    //

    String qualifierFilePath = _nameSpaceManager.getQualifierFilePath(
        nameSpace, qualifierName);

    //
    // Load qualifier:
    //

    CIMQualifierDecl qualifierDecl;

    try
    {
        _LoadObject(qualifierFilePath, qualifierDecl);
    }
    catch (CannotOpenFile&)
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_FOUND, qualifierName);
    }

    PEG_METHOD_EXIT();
    return qualifierDecl;
}

void CIMRepository::setQualifier(
    const String& nameSpace,
    const CIMQualifierDecl& qualifierDecl)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::setQualifier");

    // -- Get path of qualifier file:

    String qualifierFilePath = _nameSpaceManager.getQualifierFilePath(
        nameSpace, qualifierDecl.getName());

    // -- If qualifier already exists, throw exception:

    if (FileSystem::existsNoCase(qualifierFilePath))
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(
	    CIM_ERR_ALREADY_EXISTS, qualifierDecl.getName());
    }

    // -- Save qualifier:

    Array<Sint8> qualifierDeclXml;
    XmlWriter::appendQualifierDeclElement(qualifierDeclXml, qualifierDecl);
    _SaveObject(qualifierFilePath, qualifierDeclXml);

    PEG_METHOD_EXIT();
}

void CIMRepository::deleteQualifier(
    const String& nameSpace,
    const String& qualifierName)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::deleteQualifier");

    // -- Get path of qualifier file:

    String qualifierFilePath = _nameSpaceManager.getQualifierFilePath(
        nameSpace, qualifierName);

    // -- Delete qualifier:

    if (!FileSystem::removeFileNoCase(qualifierFilePath))
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_FOUND, qualifierName);
    }

    PEG_METHOD_EXIT();
}

Array<CIMQualifierDecl> CIMRepository::enumerateQualifiers(
    const String& nameSpace)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::enumerateQualifiers");

    String qualifiersRoot = _nameSpaceManager.getQualifiersRoot(nameSpace);

    Array<String> qualifierNames;

    if (!FileSystem::getDirectoryContents(qualifiersRoot, qualifierNames))
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
            "enumerateQualifiers(): internal error");
    }

    Array<CIMQualifierDecl> qualifiers;

    for (Uint32 i = 0; i < qualifierNames.size(); i++)
    {
        CIMQualifierDecl qualifier = getQualifier(nameSpace, qualifierNames[i]);
        qualifiers.append(qualifier);
    }

    PEG_METHOD_EXIT();
    return qualifiers;
}

void CIMRepository::createNameSpace(const String& nameSpace)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::createNameSpace");

    _nameSpaceManager.createNameSpace(nameSpace);

    PEG_METHOD_EXIT();
}

Array<String> CIMRepository::enumerateNameSpaces() const
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::enumerateNameSpaces");

    Array<String> nameSpaceNames;
    _nameSpaceManager.getNameSpaceNames(nameSpaceNames);

    PEG_METHOD_EXIT();
    return nameSpaceNames;
}

void CIMRepository::deleteNameSpace(const String& nameSpace)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::deleteNameSpace");

    _nameSpaceManager.deleteNameSpace(nameSpace);

    PEG_METHOD_EXIT();
}

//----------------------------------------------------------------------
//
// _getInstanceIndexFilePath()
//
//      returns the file path of the instance index file. 
//
//----------------------------------------------------------------------

String CIMRepository::_getInstanceIndexFilePath(
    const String& nameSpace,
    const String& className) const
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::_getInstanceIndexFilePath");

    String tmp = _nameSpaceManager.getInstanceDataFileBase(
	nameSpace, className);

    tmp.append(".idx");

    PEG_METHOD_EXIT();
    return tmp;
}

//----------------------------------------------------------------------
//
// _getInstanceDataFilePath()
//
//      returns the file path of the instance file. 
//
//----------------------------------------------------------------------

String CIMRepository::_getInstanceDataFilePath(
    const String& nameSpace,
    const String& className) const
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::_getInstanceDataFilePath");

    String tmp = _nameSpaceManager.getInstanceDataFileBase(
	nameSpace, className);
    tmp.append(".instances");
  
    PEG_METHOD_EXIT();
    return tmp;
}

Boolean CIMRepository::_loadInstance(
    const String& path,
    CIMInstance& object,
    Uint32 index,
    Uint32 size)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::_loadInstance");

    //
    // Load instance (in XML) from instance file into memory:
    //

    Array<Sint8> data;

    if (!InstanceDataFile::loadInstance(path, index, size, data))
    {
        PEG_METHOD_EXIT();
        return false;
    }

    //
    // Convert XML into an actual object:
    //

    XmlParser parser((char*)data.getData());
    XmlReader::getObject(parser, object);

    PEG_METHOD_EXIT();
    return true;
}

void CIMRepository::setDeclContext(RepositoryDeclContext *context)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::setDeclContext");

    _context = context;

    PEG_METHOD_EXIT();
}

String namespaceNameToDirName(const String& namespaceName)
{
    String dirName = namespaceName;

    for (int i=0; i<dirName.size(); i++)
    {
        if (dirName[i] == '/')
        {
            dirName[i] = '#';
        }
    }

    return dirName;
}

String dirNameToNamespaceName(const String& dirName)
{
    String namespaceName = dirName;

    for (int i=0; i<namespaceName.size(); i++)
    {
        if (namespaceName[i] == '#')
        {
            namespaceName[i] = '/';
        }
    }

    return namespaceName;
}

PEGASUS_NAMESPACE_END
