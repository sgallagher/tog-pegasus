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

#include <Pegasus/Common/Config.h>
#include <cctype>
#include <cstdio>
#include <fstream>
#include <Pegasus/Common/Pair.h>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/InternalException.h>
#include <Pegasus/Common/DeclContext.h>
#include <Pegasus/Common/Resolver.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/PegasusVersion.h>
#include <Pegasus/Common/MessageLoader.h>
#include <Pegasus/Common/CommonUTF.h>
#include <Pegasus/Common/ReadWriteSem.h>
#include <Pegasus/Common/Dir.h>
#include <Pegasus/Common/XmlStreamer.h>
#include <Pegasus/Common/BinaryStreamer.h>
#include <Pegasus/Common/AutoStreamer.h>
#include "DefaultRepository.h"
#include "RepositoryDeclContext.h"
#include "InstanceIndexFile.h"
#include "InstanceDataFile.h"
#include "AssocInstTable.h"
#include "AssocClassTable.h"
#include "ObjectCache.h"
#include "Filtering.h"

#ifdef PEGASUS_ENABLE_COMPRESSED_REPOSITORY
// #define win32
# include <zlib.h>
# include <sstream>
#endif

#if 0
#undef PEG_METHOD_ENTER
#undef PEG_METHOD_EXIT
#define PEG_METHOD_ENTER(x,y)  cout<<"--- Enter: "<<y<<endl;
#define PEG_METHOD_EXIT()
#endif

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

static const Uint32 _MAX_FREE_COUNT = 16;

#ifdef PEGASUS_ENABLE_COMPRESSED_REPOSITORY
static int compressMode = 0; // PEP214
#endif

// #define TEST_OUTPUT

//==============================================================================
//
// This is the class cache, which caches up PEGASUS_CLASS_CACHE_SIZE classes
// into memory. To override the default, define PEGASUS_CLASS_CACHE_SIZE in
// your environment. To supress the cache (and not compile it in at all)
// define PEGASUS_CLASS_CACHE_SIZE to 0.
//
//==============================================================================

#define PEGASUS_QUALIFIER_CACHE_SIZE 80

#if !defined(PEGASUS_CLASS_CACHE_SIZE)
# define PEGASUS_CLASS_CACHE_SIZE 8
#endif

#if (PEGASUS_CLASS_CACHE_SIZE != 0)
# define PEGASUS_USE_CLASS_CACHE
#endif

#ifdef PEGASUS_USE_CLASS_CACHE
static ObjectCache<CIMClass> _classCache(PEGASUS_CLASS_CACHE_SIZE);
#endif /* PEGASUS_USE_CLASS_CACHE */

static ObjectCache<CIMQualifierDecl>
    _qualifierCache(PEGASUS_QUALIFIER_CACHE_SIZE);

////////////////////////////////////////////////////////////////////////////////
//
// _LoadFileToMemory()  PEP214
//
// The gzxxxx functions read both compresed and non-compresed files.
//
// There is no conditional flag on reading of files since gzread()
// (from zlib) is capable of reading compressed and non-compressed
// files (so it contains the logic that examines the header
// and magic number). Everything will work properly if the repository
// has some compressed and some non-compressed files.
//
//
////////////////////////////////////////////////////////////////////////////////

void _LoadFileToMemory(Buffer& data, const String& path)
{

#ifdef PEGASUS_ENABLE_COMPRESSED_REPOSITORY

    Uint32 fileSize;

    if (!FileSystem::getFileSize(path, fileSize))
        throw CannotOpenFile(path);

    gzFile fp = gzopen(path.getCString(), "rb");

    if (fp == NULL)
        throw CannotOpenFile(path);

    data.reserveCapacity(fileSize);
    char buffer[4096];
    int n;

    while ((n = gzread(fp, buffer, sizeof(buffer))) > 0)
        data.append(buffer, n);

    gzclose(fp);

#else

    FileSystem::loadFileToMemory(data, path);

#endif /* PEGASUS_ENABLE_COMPRESSED_REPOSITORY */
}

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
    Object& object,
    ObjectStreamer* streamer)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "DefaultRepository::_LoadObject");

    // Get the real path of the file:

    String realPath;

    if (!FileSystem::existsNoCase(path, realPath))
    {
        PEG_TRACE_STRING(TRC_REPOSITORY, Tracer::LEVEL4,
            path + " does not exist.");
        PEG_METHOD_EXIT();
        throw CannotOpenFile(path);
    }

    PEG_TRACE_STRING(TRC_REPOSITORY, Tracer::LEVEL4, "realpath = " + realPath);

    // Load file into memory:

    Buffer data;
    _LoadFileToMemory(data, realPath);
    streamer->decode(data, 0, object);

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

void _SaveObject(
    const String& path,
    Buffer& objectXml,
    ObjectStreamer* streamer)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "DefaultRepository::_SaveObject");

// cout << "SAVE[" << path << "]" << endl;

#ifdef PEGASUS_ENABLE_COMPRESSED_REPOSITORY
    if (compressMode)            // PEP214
    {
        PEGASUS_STD(ostringstream) os;
        streamer->write(os, objectXml);
        string str = os.str();

        gzFile fp = gzopen(path.getCString(), "wb");

        if (fp == NULL)
          throw CannotOpenFile(path);

        const char* ptr = str.data();
        size_t rem = str.size();
        int n;

        while (rem > 0 && (n = gzwrite(fp, (char*)ptr, rem)) > 0)
        {
            ptr += n;
            rem -= n;
        }

        gzclose(fp);
    }
    else
#endif /* PEGASUS_ENABLE_COMPRESSED_REPOSITORY */
    {
        PEGASUS_STD(ofstream) os(path.getCString() PEGASUS_IOS_BINARY);

        if (!os)
        {
            PEG_METHOD_EXIT();
            throw CannotOpenFile(path);
        }

        streamer->write(os, objectXml);
    }
    PEG_METHOD_EXIT();
}

////////////////////////////////////////////////////////////////////////////////
//
// _beginInstanceTransaction()
//
//      Creates rollback files to allow an incomplete transaction to be voided.
//
////////////////////////////////////////////////////////////////////////////////

void _beginInstanceTransaction(
    const String& indexFilePath,
    const String& dataFilePath)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "_beginInstanceTransaction");

    //
    // Begin the transaction (an incomplete transaction will cause
    // a rollback the next time an instance-oriented routine is invoked).
    //

    if (!InstanceIndexFile::beginTransaction(indexFilePath))
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
            MessageLoaderParms("Repository.CIMRepository.BEGIN_FAILED",
                "begin failed"));
    }

    if (!InstanceDataFile::beginTransaction(dataFilePath))
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
            MessageLoaderParms("Repository.CIMRepository.BEGIN_FAILED",
                "begin failed"));
    }

    PEG_METHOD_EXIT();
}

////////////////////////////////////////////////////////////////////////////////
//
// _commitInstanceTransaction()
//
//      Removes the rollback files to complete the transaction.
//
////////////////////////////////////////////////////////////////////////////////

void _commitInstanceTransaction(
    const String& indexFilePath,
    const String& dataFilePath)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "_commitInstanceTransaction");

    //
    // Commit the transaction by removing the rollback files.
    //

    if (!InstanceIndexFile::commitTransaction(indexFilePath))
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
            MessageLoaderParms("Repository.CIMRepository.COMMIT_FAILED",
                "commit failed"));
    }

    if (!InstanceDataFile::commitTransaction(dataFilePath))
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
            MessageLoaderParms("Repository.CIMRepository.COMMIT_FAILED",
                "commit failed"));
    }

    PEG_METHOD_EXIT();
}

////////////////////////////////////////////////////////////////////////////////
//
// _rollbackInstanceTransaction()
//
//      Restores instance index and data files to void an incomplete operation.
//      If there are no rollback files, this method has no effect.
//
////////////////////////////////////////////////////////////////////////////////

static String _dirName(const String& path)
{
    Uint32 n = path.size();

    for (Uint32 i = n; i != 0; )
    {
        if (path[--i] == '/')
            return path.subString(0, i);
    }

    return String(".");
}

void _rollbackInstanceTransaction(
    const String& indexFilePath,
    const String& dataFilePath)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "_rollbackInstanceTransaction");

    // Avoid rollback logic if directory has no .rollback files.

    String path = _dirName(indexFilePath);
    Array<String> rollbackFiles;

    if (FileSystem::glob(path, "*.rollback", rollbackFiles))
    {
        if (rollbackFiles.size() == 0)
            return;
    }

    // Proceed to rollback logic.

    if (!InstanceIndexFile::rollbackTransaction(indexFilePath))
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
            MessageLoaderParms("Repository.CIMRepository.ROLLBACK_FAILED",
                "rollback failed"));
    }

    if (!InstanceDataFile::rollbackTransaction(dataFilePath))
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
            MessageLoaderParms(
                "Repository.CIMRepository.ROLLBACK_FAILED",
                "rollback failed"));
    }

    PEG_METHOD_EXIT();
}

////////////////////////////////////////////////////////////////////////////////
//
// InstanceTransactionHandler
//
//      This class is used to manage a repository instance transaction.  The
//      transaction is started when the class is instantiated, committed when
//      the complete() method is called, and rolled back if the destructor is
//      called without a prior call to complete().
//
//      The appropriate repository write locks must be owned while an
//      InstanceTransactionHandler instance exists.
//
////////////////////////////////////////////////////////////////////////////////

class InstanceTransactionHandler
{
public:
    InstanceTransactionHandler(
        const String& indexFilePath,
        const String& dataFilePath)
    : _indexFilePath(indexFilePath),
      _dataFilePath(dataFilePath),
      _isComplete(false)
    {
        _rollbackInstanceTransaction(_indexFilePath, _dataFilePath);
        _beginInstanceTransaction(_indexFilePath, _dataFilePath);
    }

    ~InstanceTransactionHandler()
    {
        if (!_isComplete)
        {
            _rollbackInstanceTransaction(_indexFilePath, _dataFilePath);
        }
    }

    void complete()
    {
        _commitInstanceTransaction(_indexFilePath, _dataFilePath);
        _isComplete = true;
    }

private:
    String _indexFilePath;
    String _dataFilePath;
    Boolean _isComplete;
};

////////////////////////////////////////////////////////////////////////////////
//
// DefaultRepository::_rollbackIncompleteTransactions()
//
//      Searches for incomplete instance transactions for all classes in all
//      namespaces.  Restores instance index and data files to void an
//      incomplete operation.  If no incomplete instance transactions are
//      outstanding, this method has no effect.
//
////////////////////////////////////////////////////////////////////////////////

static Boolean _containsNoCase(const Array<String>& array, const String& str)
{
    for (Uint32 i = 0; i < array.size(); i++)
    {
        if (String::equalNoCase(array[i], str))
            return true;
    }

    return false;
}

void DefaultRepository::_rollbackIncompleteTransactions()
{
    PEG_METHOD_ENTER(TRC_REPOSITORY,
        "DefaultRepository::_rollbackIncompleteTransactions");

    WriteLock wlock(_lock);
    AutoFileLock fileLock(_lockFile);

    Array<CIMNamespaceName> namespaceNames;
    _nameSpaceManager.getNameSpaceNames(namespaceNames);

    for (Uint32 i = 0; i < namespaceNames.size(); i++)
    {
        // Form a list of .rollback files.

        Array<String> rollbackFiles;
        FileSystem::glob(
            _nameSpaceManager.getInstanceDirRoot(namespaceNames[i]),
            "*.rollback", rollbackFiles);

        // Don't bother doing rollback if there are no rollback files.
        // The algorithm below is expensive.

        if (rollbackFiles.size() == 0)
            continue;

        Array<CIMName> classNames;
        _nameSpaceManager.getSubClassNames(
            namespaceNames[i], CIMName(), true, classNames);

        for (Uint32 j = 0; j < classNames.size(); j++)
        {
            //
            // Get paths of index and data files:
            //

            String indexFilePath = _getInstanceIndexFilePath(
                namespaceNames[i], classNames[j]);

            String dataFilePath = _getInstanceDataFilePath(
                namespaceNames[i], classNames[j]);

            // Only perform rollback processing if there is a rollback file
            // for either the data or index file.

            if (_containsNoCase(rollbackFiles, dataFilePath + ".rollback") ||
                _containsNoCase(rollbackFiles, indexFilePath + ".rollback"))
            {
                //
                // Attempt rollback (if there are no rollback files, this will
                // have no effect). This code is here to rollback uncommitted
                // changes left over from last time an instance-oriented 
                // function
                // was called.
                //

                _rollbackInstanceTransaction(indexFilePath, dataFilePath);
            }
        }
    }

    PEG_METHOD_EXIT();
}

////////////////////////////////////////////////////////////////////////////////
//
// DefaultRepository
//
//     The following are not implemented:
//
//         DefaultRepository::execQuery()
//         DefaultRepository::invokeMethod()
//
//     Note that invokeMethod() will not never implemented since it is not
//     meaningful for a repository.
//
////////////////////////////////////////////////////////////////////////////////

DefaultRepository::DefaultRepository(
    const String& repositoryRoot,
    Uint32 repositoryMode) 
    : 
    _repositoryRoot(repositoryRoot), 
    _repositoryMode(repositoryMode),
    _nameSpaceManager(repositoryRoot),
    _lock(),
    _resolveInstance(true)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "DefaultRepository::DefaultRepository");

    Boolean binaryMode = repositoryMode & DefaultRepository::MODE_BIN;

    if (repositoryMode == DefaultRepository::MODE_DEFAULT)
    {
        binaryMode = ConfigManager::parseBooleanValue(
            ConfigManager::getInstance()->getCurrentValue(
                "enableBinaryRepository"));
    }

#ifdef PEGASUS_ENABLE_COMPRESSED_REPOSITORY    // PEP214
    // FUTURE?? -  compressMode = mode & DefaultRepository::MODE_COMPRESSED;
    compressMode=1;

    char* s = getenv("PEGASUS_ENABLE_COMPRESSED_REPOSITORY");
    if (s && (strcmp(s, "build_non_compressed") == 0))
    {
        compressMode =0;
#ifdef TEST_OUTPUT
        cout << "In Compress mode: build_non_compresed found" << endl;
#endif /* TEST_OUTPUT */
    }
#endif /* PEGASUS_ENABLE_COMPRESSED_REPOSITORY */

#ifdef TEST_OUTPUT
    cout << "repositoryRoot = " << repositoryRoot << endl;
    cout << "DefaultRepository: binaryMode="  << binaryMode <<
        ", mode=" << mode << endl;
    cout << "DefaultRepository: compressMode= " << compressMode << endl;
#endif /* TEST_OUTPUT */

    if (binaryMode>0)
    {
        // BUILD BINARY
        streamer = new AutoStreamer(new BinaryStreamer(), BINREP_MARKER);
        ((AutoStreamer*)streamer)->addReader(new XmlStreamer(), 0);
    }
    else
    {
        // BUILD XML
        streamer = new AutoStreamer(new XmlStreamer(),0xff);
        ((AutoStreamer*)streamer)->addReader(
            new BinaryStreamer(), BINREP_MARKER);
        ((AutoStreamer*)streamer)->addReader(new XmlStreamer(), 0);
    }

    _context = new RepositoryDeclContext(this);

    _isDefaultInstanceProvider = ConfigManager::parseBooleanValue(
        ConfigManager::getInstance()->getCurrentValue(
            "repositoryIsDefaultInstanceProvider"));

    _lockFile = ConfigManager::getInstance()->getHomedPath(
        PEGASUS_REPOSITORY_LOCK_FILE).getCString();

    _rollbackIncompleteTransactions();

    PEG_METHOD_EXIT();
}

DefaultRepository::~DefaultRepository()
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "DefaultRepository::~DefaultRepository");

    delete streamer;
    delete _context;

    AssocClassTable::removeCaches();

    PEG_METHOD_EXIT();
}

String _toString(Boolean x)
{
    return(x ? "true" : "false");
}

CIMClass DefaultRepository::getClass(
    bool lock,
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "DefaultRepository::getClass");

    ReadLock rlock(_lock, lock);

    PEG_TRACE_STRING(TRC_REPOSITORY, Tracer::LEVEL4, "nameSpace= " +
                     nameSpace.getString() + ", className= " +
                     className.getString() +
                     ", localOnly= " + _toString(localOnly) +
                     ", includeQualifiers= " + _toString(includeQualifiers) +
                     ", includeClassOrigin= " + _toString(includeClassOrigin));
    String classFilePath;
    classFilePath = _nameSpaceManager.getClassFilePath(
        nameSpace, className, NameSpaceRead);

    CIMClass cimClass;

    try
    {
#ifdef PEGASUS_USE_CLASS_CACHE

        // Check the cache first:

        if (!_classCache.get(classFilePath, cimClass))
        {
            // Not in cache so load from disk:

            _LoadObject(classFilePath, cimClass, streamer);

            // Put in cache:

            _classCache.put(classFilePath, cimClass);
        }

#else /* PEGASUS_USE_CLASS_CACHE */

        _LoadObject(classFilePath, cimClass, streamer);

#endif /* PEGASUS_USE_CLASS_CACHE */
    }
    catch (Exception&)
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_FOUND, className.getString());
    }

    // Remove properties based on propertylist and localOnly flag (Bug 565)
    Boolean propertyListNull = propertyList.isNull();

    Filtering::filterClass(
        cimClass, 
        localOnly, 
        includeQualifiers,
        includeClassOrigin, 
        propertyList);

    PEG_METHOD_EXIT();
    return cimClass;
}

Boolean DefaultRepository::_checkInstanceAlreadyExists(
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& instanceName) const
{
    PEG_METHOD_ENTER(TRC_REPOSITORY,
        "DefaultRepository::_checkInstanceAlreadyExists");

    //
    // Get the names of all superclasses and subclasses of this class
    //

    Array<CIMName> classNames;
    CIMName className = instanceName.getClassName();
    classNames.append(className);
    _nameSpaceManager.getSubClassNames(nameSpace, className, true, classNames);
    _nameSpaceManager.getSuperClassNames(nameSpace, className, classNames);

    //
    // Search for an instance with the specified key values
    //

    for (Uint32 i = 0; i < classNames.size(); i++)
    {
        CIMObjectPath tmpInstanceName = instanceName;
        tmpInstanceName.setClassName(classNames[i]);

        String path = _getInstanceIndexFilePath(nameSpace, classNames[i]);

        Uint32 index;
        Uint32 size;
        if (InstanceIndexFile::lookupEntry(path, tmpInstanceName, index, size))
        {
            PEG_METHOD_EXIT();
            return true;
        }
    }

    PEG_METHOD_EXIT();
    return false;
}

CIMInstance DefaultRepository::getInstance(
    bool lock,
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& instanceName,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "DefaultRepository::getInstance");

    ReadLock rlock(_lock, lock);

    //
    // Validate namespace
    //
    if ((!instanceName.getNameSpace().isNull()) &&
        (!instanceName.getNameSpace().equal(nameSpace)))
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_FOUND,
                                       instanceName.toString());
    }

    if (!_nameSpaceManager.classExists(nameSpace, instanceName.getClassName()))
    {
        throw PEGASUS_CIM_EXCEPTION(
            CIM_ERR_INVALID_CLASS, instanceName.getClassName().getString());
    }

    //
    // Get paths of index and data files:
    //

    String indexFilePath = _getInstanceIndexFilePath(
        nameSpace, instanceName.getClassName());

    String dataFilePath = _getInstanceDataFilePath(
        nameSpace, instanceName.getClassName());

    //
    // Get the index for this instance:
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
    // Load the instance from file:
    //

    CIMInstance cimInstance;

    if (!_loadInstance(dataFilePath, cimInstance, index, size))
    {
        PEG_METHOD_EXIT();
        throw CannotOpenFile(dataFilePath);
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

    Filtering::filterInstance(
        cimInstance,
        localOnly,
        includeQualifiers,
        includeClassOrigin,
        propertyList);


    PEG_METHOD_EXIT();
    return cimInstance;
}

void DefaultRepository::deleteClass(
    bool lock,
    const CIMNamespaceName& nameSpace,
    const CIMName& className)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY,"DefaultRepository::deleteClass");

    WriteLock wlock(_lock, lock);
    AutoFileLock fileLock(_lockFile, lock);

    //
    // Get the class and check to see if it is an association class:
    //

    CIMClass cimClass = getClass(
        false, nameSpace, className, false, true, false, CIMPropertyList());
    Boolean isAssociation = cimClass.isAssociation();

    //
    // Delete the class. The NameSpaceManager::deleteClass() method throws
    // an exception if the class has subclasses.
    //
#ifdef PEGASUS_USE_CLASS_CACHE

    _classCache.evict(_nameSpaceManager.getClassFilePath(
        nameSpace, className, NameSpaceRead));

#endif /* PEGASUS_USE_CLASS_CACHE */

    _nameSpaceManager.deleteClass(nameSpace, className);

    //
    // Remove associations:
    //

    if (isAssociation)
    {
        Array<String> assocFileName =
            _nameSpaceManager.getAssocClassPath(nameSpace,NameSpaceDelete);

        if (FileSystem::exists(assocFileName[0]))
            AssocClassTable::deleteAssociation(assocFileName[0], className);
    }

    PEG_METHOD_EXIT();
}

void _CompactInstanceRepository(
    const String& indexFilePath,
    const String& dataFilePath)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY,
        "DefaultRepository::_CompactInstanceRepository");

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
        throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
            MessageLoaderParms(
                "Repository.CIMRepository.INDEX_ENUM_ENTRIES_FAILED",
                "Failed to obtain the entries from the Repository Instance"
                " Index file."));
    }

    if (!InstanceDataFile::compact(dataFilePath, freeFlags, indices, sizes))
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
            MessageLoaderParms(
                "Repository.CIMRepository.COMPACT_FAILED",
                "Failed to compact the Repository Instance Data file."));
    }

    //
    // Now compact the index file:
    //

    if (!InstanceIndexFile::compact(indexFilePath))
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
            MessageLoaderParms(
                "Repository.CIMRepository.INDEX_COMPACT_FAILED",
                "Failed to compact the Repository Instance Index file."));
    }

    PEG_METHOD_EXIT();
}

void DefaultRepository::deleteInstance(
    bool lock,
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& instanceName)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "DefaultRepository::deleteInstance");

    //
    // Validate namespace
    //
    if ((!instanceName.getNameSpace().isNull()) &&
        (!instanceName.getNameSpace().equal(nameSpace)))
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(
            CIM_ERR_NOT_FOUND, instanceName.toString());
    }

    WriteLock wlock(_lock, lock);
    AutoFileLock fileLock(_lockFile, lock);

    String errMessage;

    //
    // Get paths of index and data files:
    //

    String indexFilePath = _getInstanceIndexFilePath(
        nameSpace, instanceName.getClassName());

    String dataFilePath = _getInstanceDataFilePath(
        nameSpace, instanceName.getClassName());

    //
    // Perform the operation in a transaction scope to enable rollback on
    // failure.
    //

    InstanceTransactionHandler transaction(indexFilePath, dataFilePath);

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
    // Remove entry from index file.
    //

    Uint32 freeCount;

    if (!InstanceIndexFile::deleteEntry(indexFilePath, instanceName, freeCount))
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
            MessageLoaderParms(
                "Repository.CIMRepository.FAILED_TO_DELETE_INSTANCE",
                "Failed to delete instance: $0",
                instanceName.toString()));
    }

    transaction.complete();

    //
    // Compact the index and data files if the free count max was
    // reached.
    //

    if (freeCount == _MAX_FREE_COUNT)
        _CompactInstanceRepository(indexFilePath, dataFilePath);

    //
    // Delete from assocation table (if an assocation).
    //

    String assocFileName = _nameSpaceManager.getAssocInstPath(nameSpace);

    if (FileSystem::exists(assocFileName))
        AssocInstTable::deleteAssociation(assocFileName, instanceName);

    PEG_METHOD_EXIT();
}

void DefaultRepository::_createAssocClassEntries(
    const CIMNamespaceName& nameSpace,
    const CIMConstClass& assocClass)
{
    PEG_METHOD_ENTER(
        TRC_REPOSITORY, "DefaultRepository::_createAssocClassEntries");

    // Open input file:


    Array<String> assocFileName =
        _nameSpaceManager.getAssocClassPath(nameSpace,NameSpaceWrite);
    ofstream os;

    if (!OpenAppend(os, assocFileName[0]))
    {
        PEG_METHOD_EXIT();
        throw CannotOpenFile(assocFileName[0]);
    }

    // Get the association's class name:

    CIMName assocClassName = assocClass.getClassName();

    // For each property:

    Uint32 n = assocClass.getPropertyCount();

    for (Uint32 i = 0; i < n; i++)
    {
        CIMConstProperty fromProp = assocClass.getProperty(i);

        if (fromProp.getType() == CIMTYPE_REFERENCE)
        {
            for (Uint32 j = 0; j < n; j++)
            {
                CIMConstProperty toProp = assocClass.getProperty(j);

                if (toProp.getType() == CIMTYPE_REFERENCE &&
                    (!fromProp.getName().equal (toProp.getName())))
                {
                    CIMName fromClassName = fromProp.getReferenceClassName();
                    CIMName fromPropertyName = fromProp.getName();
                    CIMName toClassName = toProp.getReferenceClassName();
                    CIMName toPropertyName = toProp.getName();

                    AssocClassTable::append(
                        os,
                        assocFileName[0],
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

void DefaultRepository::createClass(
    bool lock,
    const CIMNamespaceName& nameSpace,
    const CIMClass& newClass,
    const ContentLanguageList& contentLangs)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "DefaultRepository::createClass");

    WriteLock wlock(_lock, lock);
    AutoFileLock fileLock(_lockFile, lock);

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

    Buffer classXml;
    streamer->encode(classXml, cimClass);
    //XmlWriter::appendClassElement(classXml, cimClass);
    _SaveObject(classFilePath, classXml,streamer);


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


void DefaultRepository::_createAssocInstEntries(
    const CIMNamespaceName& nameSpace,
    const CIMConstClass& cimClass,
    const CIMInstance& cimInstance,
    const CIMObjectPath& instanceName)
{
    PEG_METHOD_ENTER(
        TRC_REPOSITORY, "DefaultRepository::_createAssocInstEntries");

    // Open input file:

    String assocFileName = _nameSpaceManager.getAssocInstPath(nameSpace);
    ofstream os;

    if (!OpenAppend(os, assocFileName))
    {
        PEG_METHOD_EXIT();
        throw CannotOpenFile(assocFileName);
    }

    // Get the association's instance name and class name:

    String assocInstanceName = instanceName.toString();
    CIMName assocClassName = instanceName.getClassName();

    // For each property:

    for (Uint32 i = 0, n = cimInstance.getPropertyCount(); i < n; i++)
    {
        CIMConstProperty fromProp = cimInstance.getProperty(i);

        // If a reference property:

        if (fromProp.getType() == CIMTYPE_REFERENCE)
        {
            // For each property:

            for (Uint32 j = 0, n = cimInstance.getPropertyCount(); j < n; j++)
            {
                CIMConstProperty toProp = cimInstance.getProperty(j);

                // If a reference property and not the same property:

                if (toProp.getType() == CIMTYPE_REFERENCE &&
                    (!fromProp.getName().equal (toProp.getName())))
                {
                    CIMObjectPath fromRef;
                    fromProp.getValue().get(fromRef);

                    CIMObjectPath toRef;
                    toProp.getValue().get(toRef);


                    // Fix for bugzilla 667:
                    // Strip off the hostname if it is the same as the
                    // local host
                    if ((fromRef.getHost() != String::EMPTY) &&
                        (System::isLocalHost(fromRef.getHost())))
                    {
                        PEG_TRACE_CSTRING(TRC_REPOSITORY, Tracer::LEVEL4,
                            "_createAssocInstEntries() - Stripping off local "
                                "hostName from fromRef");
                       fromRef.setHost(String::EMPTY);
                    }

                    // Strip off the namespace when it is the same as the
                    // one this instance is created in.
                    if ((fromRef.getHost() == String::EMPTY) &&
                        (fromRef.getNameSpace() == nameSpace))
                    {
                        PEG_TRACE_CSTRING(TRC_REPOSITORY, Tracer::LEVEL4,
                            "_createAssocInstEntries() - Stripping off "
                                "local nameSpace from fromRef");
                        fromRef.setNameSpace(CIMNamespaceName());
                    }

                    // Strip off the hostname if it is the same as the
                    // local host
                    if ((toRef.getHost() != String::EMPTY) &&
                        (System::isLocalHost(toRef.getHost())))
                    {
                        PEG_TRACE_CSTRING(TRC_REPOSITORY, Tracer::LEVEL4,
                            "_createAssocInstEntries() - Stripping off "
                                "local hostName from toRef");
                        toRef.setHost(String::EMPTY);
                    }

                    // Strip off the namespace when it is the same as the
                    // one this instance is created in.
                    if ((toRef.getHost() == String::EMPTY) &&
                        (toRef.getNameSpace() == nameSpace))
                    {
                       PEG_TRACE_CSTRING(TRC_REPOSITORY, Tracer::LEVEL4,
                           "_createAssocInstEntries() - Stripping off "
                               "local nameSpace from toRef");
                       toRef.setNameSpace(CIMNamespaceName());
                    }


                    String fromObjectName = fromRef.toString();
                    CIMName fromClassName = fromRef.getClassName();
                    CIMName fromPropertyName = fromProp.getName();
                    String toObjectName = toRef.toString();
                    CIMName toClassName = toRef.getClassName();
                    CIMName toPropertyName = toProp.getName();

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

CIMObjectPath DefaultRepository::createInstance(
    bool lock,
    const CIMNamespaceName& nameSpace,
    const CIMInstance& newInstance,
    const ContentLanguageList& contentLangs)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "DefaultRepository::createInstance");

    WriteLock wlock(_lock, lock);
    AutoFileLock fileLock(_lockFile, lock);

    String errMessage;

    //
    // Resolve the instance. Looks up class and fills out properties but
    // not the qualifiers.
    //

    CIMInstance cimInstance(newInstance);
    CIMConstClass cimClass;
    Resolver::resolveInstance (cimInstance, _context, nameSpace, cimClass,
        false);
    CIMObjectPath instanceName = cimInstance.buildPath(cimClass);

    //
    // Make sure the class has keys (otherwise it will be impossible to
    // create the instance).
    //

    if (!cimClass.hasKeys())
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
            MessageLoaderParms("Repository.CIMRepository.CLASS_HAS_NO_KEYS",
                "class has no keys: $0",
                cimClass.getClassName().getString()));
    }

    //
    // Be sure instance does not already exist:
    //

    if (_checkInstanceAlreadyExists(nameSpace, instanceName))
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
    // Get paths to data and index files:
    //

    String indexFilePath = _getInstanceIndexFilePath(
        nameSpace, newInstance.getClassName());

    String dataFilePath = _getInstanceDataFilePath(
        nameSpace, newInstance.getClassName());

    //
    // Perform the operation in a transaction scope to enable rollback on
    // failure.
    //

    InstanceTransactionHandler transaction(indexFilePath, dataFilePath);

    //
    // Save instance to file:
    //

    Uint32 index;
    Uint32 size;

    {
        Buffer data;
        streamer->encode(data, cimInstance);
        size = data.size();

        if (!InstanceDataFile::appendInstance(dataFilePath, data, index))
        {
            PEG_METHOD_EXIT();
            throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
                MessageLoaderParms(
                    "Repository.CIMRepository.FAILED_TO_CREATE_INSTANCE",
                    "Failed to create instance: $0",
                    instanceName.toString()));
        }
    }

    //
    // Create entry in index file:
    //

    if (!InstanceIndexFile::createEntry(
            indexFilePath, instanceName, index, size))
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
            MessageLoaderParms(
                "Repository.CIMRepository.FAILED_TO_CREATE_INSTANCE",
                "Failed to create instance: $0",
                instanceName.toString()));
    }

    transaction.complete();

    Resolver::resolveInstance (cimInstance, _context, nameSpace, cimClass,
        true);


    PEG_METHOD_EXIT();
    return instanceName;
}

void DefaultRepository::modifyClass(
    bool lock,
    const CIMNamespaceName& nameSpace,
    const CIMClass& modifiedClass,
    const ContentLanguageList& contentLangs)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "DefaultRepository::modifyClass");

    WriteLock wlock(_lock, lock);
    AutoFileLock fileLock(_lockFile, lock);

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

#ifdef PEGASUS_USE_CLASS_CACHE

    _classCache.evict(classFilePath);

#endif /* PEGASUS_USE_CLASS_CACHE */

    //
    // Delete the old file containing the class:
    //

    if (!FileSystem::removeFileNoCase(classFilePath))
    {
        PEG_METHOD_EXIT();
        String str = "DefaultRepository::modifyClass()";
        throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
            MessageLoaderParms(
                "Repository.CIMRepository.FAILED_TO_REMOVE_FILE",
                "failed to remove file in $0", str));
    }

    //
    // Create new class file:
    //

    Buffer classXml;
    streamer->encode(classXml, cimClass);
    //XmlWriter::appendClassElement(classXml, cimClass);
    _SaveObject(classFilePath, classXml,streamer);

    if (cimClass.isAssociation())
    {
        // Remove from Association
        Array<String> assocFileName =
            _nameSpaceManager.getAssocClassPath(nameSpace,NameSpaceDelete);
        if (FileSystem::exists(assocFileName[0]))
        {
            AssocClassTable::deleteAssociation(
                assocFileName[0], cimClass.getClassName());
            // Create the association again.
            _createAssocClassEntries(nameSpace, cimClass);
        }
        else
        {
            PEG_METHOD_EXIT();
            throw CannotOpenFile(assocFileName[0]);
        }
    }


    //
    // Cache this class:
    //

#ifdef PEGASUS_USE_CLASS_CACHE

    _classCache.put(classFilePath, cimClass);

#endif /* PEGASUS_USE_CLASS_CACHE */

    PEG_METHOD_EXIT();
}

void DefaultRepository::modifyInstance(
    bool lock,
    const CIMNamespaceName& nameSpace,
    const CIMInstance& modifiedInstance,
    Boolean includeQualifiers,
    const CIMPropertyList& propertyList,
    const ContentLanguageList& contentLangs)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "DefaultRepository::modifyInstance");

    WriteLock wlock(_lock, lock);
    AutoFileLock fileLock(_lockFile, lock);

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
                false,
                nameSpace,
                modifiedInstance.getPath (),
                false,
                true,
                true,
                CIMPropertyList());

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
                        newProperty.addQualifier(origProperty.getQualifier(j));
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

        cimInstance = getInstance(false, nameSpace,
            modifiedInstance.getPath (), false, true, true, CIMPropertyList());

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
                        false,
                        nameSpace, 
                        cimInstance.getClassName(), 
                        false,
                        true,
                        false, 
                        CIMPropertyList());

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

    CIMObjectPath instanceName = cimInstance.buildPath(cimClass);

    //
    // Disallow operation if the instance name was changed:
    //

    // For bugzilla 1508. Hostname and namespace are not included
    // in the comparison here.
    CIMObjectPath modifiedInstancePath = modifiedInstance.getPath();
    modifiedInstancePath.setNameSpace(CIMNamespaceName());
    modifiedInstancePath.setHost(String::EMPTY);
    if (instanceName != modifiedInstancePath)
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
            MessageLoaderParms(
                "Repository.CIMRepository.ATTEMPT_TO_MODIFY_KEY_PROPERTY",
                "Attempted to modify a key property"));
    }

    //
    // Get paths of index and data files:
    //

    String indexFilePath = _getInstanceIndexFilePath(
        nameSpace, modifiedInstance.getClassName());

    String dataFilePath = _getInstanceDataFilePath(
        nameSpace, modifiedInstance.getClassName());

    //
    // Look up the specified instance
    //

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
    // Perform the operation in a transaction scope to enable rollback on
    // failure.
    //

    InstanceTransactionHandler transaction(indexFilePath, dataFilePath);

    //
    // Modify the data file:
    //

    {
        Buffer out;
        streamer->encode(out, cimInstance);
        //XmlWriter::appendInstanceElement(out, cimInstance);

        newSize = out.size();

        if (!InstanceDataFile::appendInstance(dataFilePath, out, newIndex))
        {
            PEG_METHOD_EXIT();
            throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
                MessageLoaderParms(
                    "Repository.CIMRepository.FAILED_TO_MODIFY_INSTANCE",
                    "Failed to modify instance $0",
                    instanceName.toString()));
        }
    }

    //
    // Modify the index file:
    //

    Uint32 freeCount;

    if (!InstanceIndexFile::modifyEntry(indexFilePath, instanceName, newIndex,
        newSize, freeCount))
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
            MessageLoaderParms(
                "Repository.CIMRepository.FAILED_TO_MODIFY_INSTANCE",
                "Failed to modify instance $0",
                instanceName.toString()));
    }

    transaction.complete();

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

Array<CIMClass> DefaultRepository::enumerateClasses(
    bool lock,
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    Boolean deepInheritance,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "DefaultRepository::enumerateClasses");

    ReadLock rlock(_lock, lock);

    Array<CIMName> classNames;

    _nameSpaceManager.getSubClassNames(
        nameSpace, className, deepInheritance, classNames);

    Array<CIMClass> result;

    for (Uint32 i = 0; i < classNames.size(); i++)
    {
        result.append(getClass(false, nameSpace, classNames[i], localOnly,
            includeQualifiers, includeClassOrigin, CIMPropertyList()));
    }

    PEG_METHOD_EXIT();
    return result;
}

Array<CIMName> DefaultRepository::enumerateClassNames(
    bool lock,
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    Boolean deepInheritance)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "DefaultRepository::enumerateClassNames");

    ReadLock rlock(_lock, lock);

    Array<CIMName> classNames;

    _nameSpaceManager.getSubClassNames(
        nameSpace, className, deepInheritance, classNames,true);

    PEG_METHOD_EXIT();
    return classNames;
}

Boolean DefaultRepository::_loadAllInstances(
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    Array<CIMInstance>& namedInstances)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "DefaultRepository::_loadAllInstances");

    Array<CIMObjectPath> instanceNames;
    Buffer data;
    Array<Uint32> indices;
    Array<Uint32> sizes;

    //
    // Form the names of the instance index and data files
    //

    String indexFilePath = _getInstanceIndexFilePath(nameSpace, className);
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
                Uint32 pos= (Uint32)((&(buffer[indices[i]]))-buffer);
                streamer->decode(data, pos, tmpInstance);

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

Array<CIMInstance> DefaultRepository::enumerateInstancesForSubtree(
    bool lock,
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    Boolean deepInheritance,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY,
        "DefaultRepository::enumerateInstancesForSubtree");

    // It is not necessary to control access to the ReadWriteSem _lock here.
    // This method calls enumerateInstancesForClass, which does its own
    // access control.

    //
    // Get all descendent classes of this class:
    //

    Array<CIMName> classNames;
    classNames.append(className);
    _nameSpaceManager.getSubClassNames(nameSpace, className, true, classNames);

    //
    // Get all instances for this class and all its descendent classes
    //

    Array<CIMInstance> namedInstances;

    for (Uint32 i = 0; i < classNames.size(); i++)
    {
        Array<CIMInstance> localNamedInstances =
            enumerateInstancesForClass(true, nameSpace, classNames[i],
                false, includeQualifiers, includeClassOrigin, propertyList);

        // ATTN: Handles everything but deepInheritance.
        for (Uint32 i = 0 ; i < localNamedInstances.size(); i++)
        {
            Filtering::filterInstance(localNamedInstances[i],
                localOnly,
                includeQualifiers,
                includeClassOrigin,
                propertyList);
        }
        namedInstances.appendArray(localNamedInstances);
    }

    PEG_METHOD_EXIT();
    return namedInstances;
}

Array<CIMInstance> DefaultRepository::enumerateInstancesForClass(
    bool lock,
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY,
        "DefaultRepository::enumerateInstancesForClass");

    ReadLock rlock(_lock, lock);

    //
    // Get all instances for this class
    //

    Array<CIMInstance> namedInstances;

    if (!_loadAllInstances(nameSpace, className, namedInstances))
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
            MessageLoaderParms(
                "Repository.CIMRepository.FAILED_TO_LOAD_INSTANCES",
                "Failed to load instances in class $0",
                className.getString()));
    }

    // Do any required filtering of properties, qualifiers, classorigin
    // on the returned instances.
    for (Uint32 i = 0 ; i < namedInstances.size(); i++)
    {
        Filtering::filterInstance(namedInstances[i],
            localOnly,
            includeQualifiers,
            includeClassOrigin,
            propertyList);
    }

    PEG_METHOD_EXIT();
    return namedInstances;
}

Array<CIMObjectPath> DefaultRepository::enumerateInstanceNamesForSubtree(
    bool lock,
    const CIMNamespaceName& nameSpace,
    const CIMName& className)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY,
        "DefaultRepository::enumerateInstanceNamesForSubtree");

    // It is not necessary to control access to the ReadWriteSem _lock here.
    // This method calls enumerateInstanceNamesForClass, which does its own
    // access control.

    //
    // Get names of descendent classes:
    //

    Array<CIMName> classNames;
    classNames.append(className);
    _nameSpaceManager.getSubClassNames(nameSpace, className, true, classNames);

    //
    // Enumerate instance names for each of the subclasses
    //
    Array<CIMObjectPath> instanceNames;

    for (Uint32 i = 0; i < classNames.size(); i++)
    {
        instanceNames.appendArray(
            enumerateInstanceNamesForClass(true, nameSpace, classNames[i]));
    }

    PEG_METHOD_EXIT();
    return instanceNames;
}

Array<CIMObjectPath> DefaultRepository::enumerateInstanceNamesForClass(
    bool lock,
    const CIMNamespaceName& nameSpace,
    const CIMName& className)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY,
        "DefaultRepository::enumerateInstanceNamesForClass");

    ReadLock rlock(_lock, lock);

    //
    // Get instance names from the instance index file for the class:
    //
    Array<CIMObjectPath> instanceNames;
    Array<Uint32> indices;
    Array<Uint32> sizes;

    //
    // Form the names of the instance index and data files
    //

    String indexFilePath = _getInstanceIndexFilePath(nameSpace, className);
    String dataFilePath = _getInstanceDataFilePath(nameSpace, className);

    //
    // Get all instances for the class:
    //

    Array<Uint32> freeFlags;

    if (!InstanceIndexFile::enumerateEntries(
        indexFilePath, freeFlags, indices, sizes, instanceNames, false))
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
            MessageLoaderParms(
                "Repository.CIMRepository.FAILED_TO_LOAD_INSTANCE_NAMES",
                "Failed to load instance names in class $0",
                className.getString()));
    }

    PEG_METHOD_EXIT();
    return instanceNames;
}


Array<CIMInstance> DefaultRepository::execQuery(
    bool lock,
    const String& queryLanguage,
    const String& query)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "DefaultRepository::execQuery");

    ReadLock rlock(_lock, lock);

    PEG_METHOD_EXIT();
    throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, "execQuery()");

    PEGASUS_UNREACHABLE(PEG_METHOD_EXIT();
    return Array<CIMInstance>();)
}

Array<CIMObject> DefaultRepository::associators(
    bool lock,
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& objectName,
    const CIMName& assocClass,
    const CIMName& resultClass,
    const String& role,
    const String& resultRole,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "DefaultRepository::associators");

    ReadLock rlock(_lock, lock);

    Array<CIMObjectPath> names = associatorNames(
        false,
        nameSpace,
        objectName,
        assocClass,
        resultClass,
        role,
        resultRole);

    Array<CIMObject> result;

    for (Uint32 i = 0, n = names.size(); i < n; i++)
    {
        CIMNamespaceName tmpNameSpace = names[i].getNameSpace();

        if (tmpNameSpace.isNull())
            tmpNameSpace = nameSpace;

        //
        //  ATTN-CAKG-P2-20020726:  The following condition does not correctly
        //  distinguish instanceNames from classNames in every case
        //  The instanceName of a singleton instance of a keyless class also
        //  has no key bindings
        //
        if (names[i].getKeyBindings ().size () == 0)
        {
            CIMObjectPath tmpRef = names[i];
            tmpRef.setHost(String());
            tmpRef.setNameSpace(CIMNamespaceName());

            CIMClass cimClass = getClass(
                false,
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
            tmpRef.setNameSpace(CIMNamespaceName());

            CIMInstance cimInstance = getInstance(
                false,
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

Array<CIMObjectPath> DefaultRepository::associatorNames(
    bool lock,
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& objectName,
    const CIMName& assocClass,
    const CIMName& resultClass,
    const String& role,
    const String& resultRole)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "DefaultRepository::associatorNames");

    ReadLock rlock(_lock, lock);

    Array<String> associatorNames;

    // The assocClass parameter implies subclasses, so retrieve them
    Array<CIMName> assocClassList;
    if (!assocClass.isNull())
    {
        _nameSpaceManager.getSubClassNames(
            nameSpace, assocClass, true, assocClassList);
        assocClassList.append(assocClass);
    }

    // The resultClass parameter implies subclasses, so retrieve them
    Array<CIMName> resultClassList;
    if (!resultClass.isNull())
    {
        _nameSpaceManager.getSubClassNames(
            nameSpace, resultClass, true, resultClassList);
        resultClassList.append(resultClass);
    }

    //
    //  ATTN-CAKG-P2-20020726:  The following condition does not correctly
    //  distinguish instanceNames from classNames in every case
    //  The instanceName of a singleton instance of a keyless class also
    //  has no key bindings
    //
    if (objectName.getKeyBindings ().size () == 0)
    {
        CIMName className = objectName.getClassName();

        Array<CIMName> classList;
        _nameSpaceManager.getSuperClassNames(nameSpace, className, classList);
        classList.append(className);

        Array<String> assocFileName =
            _nameSpaceManager.getAssocClassPath(nameSpace,NameSpaceRead);

        for (int i=0,m=assocFileName.size(); i<m; i++)
        {
            AssocClassTable::getAssociatorNames(
                assocFileName[i],
                classList,
                assocClassList,
                resultClassList,
                role,
                resultRole,
                associatorNames);
        }
    }
    else
    {
        String assocFileName = _nameSpaceManager.getAssocInstPath(nameSpace);

        AssocInstTable::getAssociatorNames(
            assocFileName,
            objectName,
            assocClassList,
            resultClassList,
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

        if (r.getNameSpace().isNull())
            r.setNameSpace(nameSpace);

        result.append(r);
    }


    PEG_METHOD_EXIT();
    return result;
}

Array<CIMObject> DefaultRepository::references(
    bool lock,
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& objectName,
    const CIMName& resultClass,
    const String& role,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "DefaultRepository::references");

    ReadLock rlock(_lock, lock);

    Array<CIMObjectPath> names = referenceNames(
        false,
        nameSpace,
        objectName,
        resultClass,
        role);

    Array<CIMObject> result;

    for (Uint32 i = 0, n = names.size(); i < n; i++)
    {
        CIMNamespaceName tmpNameSpace = names[i].getNameSpace();

        if (tmpNameSpace.isNull())
            tmpNameSpace = nameSpace;

        // ATTN: getInstance() should this be able to handle instance names
        // with host names and namespaces?

        CIMObjectPath tmpRef = names[i];
        tmpRef.setHost(String());
        tmpRef.setNameSpace(CIMNamespaceName());

        //
        //  ATTN-CAKG-P2-20020726:  The following condition does not correctly
        //  distinguish instanceNames from classNames in every case
        //  The instanceName of a singleton instance of a keyless class also
        //  has no key bindings
        //
        if (objectName.getKeyBindings ().size () == 0)
        {
            CIMClass cimClass = getClass(
                false,
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
                false,
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

Array<CIMObjectPath> DefaultRepository::referenceNames(
    bool lock,
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& objectName,
    const CIMName& resultClass,
    const String& role)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "DefaultRepository::referenceNames");

    ReadLock rlock(_lock, lock);

    Array<String> tmpReferenceNames;

    // The resultClass parameter implies subclasses, so retrieve them
    Array<CIMName> resultClassList;

    try
    {
        if (!resultClass.isNull())
        {
            _nameSpaceManager.getSubClassNames(
                nameSpace, resultClass, true, resultClassList);
            resultClassList.append(resultClass);
        }

        //  ATTN-CAKG-P2-20020726:  The following condition does not correctly
        //  distinguish instanceNames from classNames in every case
        //  The instanceName of a singleton instance of a keyless class also
        //  has no key bindings
        //
        if (objectName.getKeyBindings ().size () == 0)
        {
            CIMName className = objectName.getClassName();

            Array<CIMName> classList;
            _nameSpaceManager.getSuperClassNames(
                nameSpace, className, classList);
            classList.append(className);

            Array<String> assocFileName =
                _nameSpaceManager.getAssocClassPath(nameSpace,NameSpaceRead);

            Boolean refs=false;
            for (int i = 0, m=assocFileName.size(); !refs && i < m; i++)
            {
                if (AssocClassTable::getReferenceNames(
                        assocFileName[i],
                        classList,
                        resultClassList,
                        role,
                        tmpReferenceNames))
                {
                    refs |= true;
                }
            }

            if (refs == false)
            {
                // Ignore error! It's okay not to have references.
            }
        }
        else
        {
            String assocFileName =
                _nameSpaceManager.getAssocInstPath(nameSpace);

            if (!AssocInstTable::getReferenceNames(
                assocFileName,
                objectName,
                resultClassList,
                role,
                tmpReferenceNames))
            {
                // Ignore error! It's okay not to have references.
            }
        }
    }
    catch (const CIMException& exception)
    {
        if (exception.getCode() == CIM_ERR_INVALID_CLASS)
        {
            throw PEGASUS_CIM_EXCEPTION(
                CIM_ERR_INVALID_PARAMETER, exception.getMessage());
        }
        else
        {
            throw;
        }
    }

    Array<CIMObjectPath> result;

    for (Uint32 i = 0, n = tmpReferenceNames.size(); i < n; i++)
    {
        CIMObjectPath r = tmpReferenceNames[i];

        if (r.getHost().size() == 0)
            r.setHost(System::getHostName());

        if (r.getNameSpace().isNull())
            r.setNameSpace(nameSpace);

        result.append(r);
    }

    PEG_METHOD_EXIT();
    return result;
}

CIMValue DefaultRepository::getProperty(
    bool lock,
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& instanceName,
    const CIMName& propertyName)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "DefaultRepository::getProperty");

    ReadLock rlock(_lock, lock);

    //
    // Retrieve the specified instance
    //

    CIMInstance cimInstance = getInstance(
        false, nameSpace, instanceName, false, true, true, CIMPropertyList());

    //
    // Get the requested property from the instance
    //

    Uint32 pos = cimInstance.findProperty(propertyName);

    // ATTN: This breaks if the property is simply null
    if (pos == PEG_NOT_FOUND)
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(
            CIM_ERR_NO_SUCH_PROPERTY,
            propertyName.getString());
    }

    CIMProperty prop = cimInstance.getProperty(pos);

    //
    // Return the value:
    //

    PEG_METHOD_EXIT();
    return prop.getValue();
}

void DefaultRepository::setProperty(
    bool lock,
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& instanceName,
    const CIMName& propertyName,
    const CIMValue& newValue,
    const ContentLanguageList& contentLangs)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "DefaultRepository::setProperty");

    // It is not necessary to control access to the ReadWriteSem _lock here.
    // This method calls modifyInstance, which does its own access control.

    //
    // Create the instance to pass to modifyInstance()
    //

    CIMInstance instance(instanceName.getClassName());
    instance.addProperty(CIMProperty(propertyName, newValue));
    instance.setPath (instanceName);

    //
    // Create the propertyList to pass to modifyInstance()
    //

    Array<CIMName> propertyListArray;
    propertyListArray.append(propertyName);
    CIMPropertyList propertyList(propertyListArray);

    //
    // Modify the instance to set the value of the given property
    //
    modifyInstance(
        true,
        nameSpace, 
        instance, 
        false, 
        propertyList,
        ContentLanguageList());

    PEG_METHOD_EXIT();
}

CIMQualifierDecl DefaultRepository::getQualifier(
    bool lock,
    const CIMNamespaceName& nameSpace,
    const CIMName& qualifierName)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "DefaultRepository::getQualifier");

    ReadLock rlock(_lock, lock);

    //
    // Get path of qualifier file:
    //

    String qualifierFilePath = _nameSpaceManager.getQualifierFilePath(
        nameSpace, qualifierName,NameSpaceRead);

    //
    // Load qualifier:
    //

    CIMQualifierDecl qualifierDecl;

    try
    {
        // Check the cache first:

        if (!_qualifierCache.get(qualifierFilePath, qualifierDecl))
        {
            // Not in cache so load from disk:

            _LoadObject(qualifierFilePath, qualifierDecl, streamer);

            // Put in cache:

            _qualifierCache.put(qualifierFilePath, qualifierDecl);
        }
    }
    catch (const CannotOpenFile&)
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION
            (CIM_ERR_NOT_FOUND, qualifierName.getString());
    }

    PEG_METHOD_EXIT();
    return qualifierDecl;
}

void DefaultRepository::setQualifier(
    bool lock,
    const CIMNamespaceName& nameSpace,
    const CIMQualifierDecl& qualifierDecl,
    const ContentLanguageList& contentLangs)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "DefaultRepository::setQualifier");

    WriteLock wlock(_lock, lock);
    AutoFileLock fileLock(_lockFile, lock);

    // -- Get path of qualifier file:

    String qualifierFilePath = _nameSpaceManager.getQualifierFilePath(
        nameSpace, qualifierDecl.getName(),NameSpaceWrite);

    // -- If qualifier already exists, throw exception:

    if (FileSystem::existsNoCase(qualifierFilePath))
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(
            CIM_ERR_NOT_SUPPORTED, qualifierDecl.getName().getString());
    }

    // -- Save qualifier:

    Buffer qualifierDeclXml;
    streamer->encode(qualifierDeclXml, qualifierDecl);
     //XmlWriter::appendQualifierDeclElement(qualifierDeclXml, qualifierDecl);
    _SaveObject(qualifierFilePath, qualifierDeclXml,streamer);

    _qualifierCache.put(qualifierFilePath, (CIMQualifierDecl&)qualifierDecl);

    PEG_METHOD_EXIT();
}

void DefaultRepository::deleteQualifier(
    bool lock,
    const CIMNamespaceName& nameSpace,
    const CIMName& qualifierName)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "DefaultRepository::deleteQualifier");

    WriteLock wlock(_lock, lock);
    AutoFileLock fileLock(_lockFile, lock);

    // -- Get path of qualifier file:

    String qualifierFilePath = _nameSpaceManager.getQualifierFilePath(
        nameSpace, qualifierName,NameSpaceDelete);

    // -- Delete qualifier:

    if (!FileSystem::removeFileNoCase(qualifierFilePath))
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION
            (CIM_ERR_NOT_FOUND, qualifierName.getString());
    }

    _qualifierCache.evict(qualifierFilePath);

    PEG_METHOD_EXIT();
}

Array<CIMQualifierDecl> DefaultRepository::enumerateQualifiers(
    bool lock,
    const CIMNamespaceName& nameSpace)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "DefaultRepository::enumerateQualifiers");

    ReadLock rlock(_lock, lock);

    String qualifiersRoot = _nameSpaceManager.getQualifiersRoot(nameSpace);

    Array<String> qualifierNames;

    if (!FileSystem::getDirectoryContents(qualifiersRoot, qualifierNames))
    {
        PEG_METHOD_EXIT();
        String str ="enumerateQualifiers()";
        throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
            MessageLoaderParms("Repository.CIMRepository.INTERNAL_ERROR",
                "$0: internal error",
                str));
    }

    Array<CIMQualifierDecl> qualifiers;

    for (Uint32 i = 0; i < qualifierNames.size(); i++)
    {
#ifdef PEGASUS_REPOSITORY_ESCAPE_UTF8
        // All chars above 0x7F will be escape.
        CIMQualifierDecl qualifier = getQualifier(false, nameSpace, 
        escapeStringDecoder(qualifierNames[i]));
#else
        CIMQualifierDecl qualifier = getQualifier( false, nameSpace, 
            qualifierNames[i]);
#endif
        qualifiers.append(qualifier);
    }

    PEG_METHOD_EXIT();
    return qualifiers;
}

void DefaultRepository::createNameSpace(
    bool lock,
    const CIMNamespaceName& nameSpace,
    const NameSpaceAttributes& attributes)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "DefaultRepository::createNameSpace");

    WriteLock wlock(_lock, lock);
    AutoFileLock fileLock(_lockFile, lock);
    _nameSpaceManager.createNameSpace(nameSpace, attributes);

    PEG_METHOD_EXIT();
}

void DefaultRepository::modifyNameSpace(
    bool lock,
    const CIMNamespaceName& nameSpace,
    const NameSpaceAttributes& attributes)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "DefaultRepository::modifyNameSpace");

    WriteLock wlock(_lock, lock);
    AutoFileLock fileLock(_lockFile, lock);
    _nameSpaceManager.modifyNameSpace(nameSpace, attributes);

    PEG_METHOD_EXIT();
}

Array<CIMNamespaceName> DefaultRepository::enumerateNameSpaces(
    bool lock) const
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "DefaultRepository::enumerateNameSpaces");

    ReadLock rlock(const_cast<ReadWriteSem&>(_lock), lock);

    Array<CIMNamespaceName> nameSpaceNames;
    _nameSpaceManager.getNameSpaceNames(nameSpaceNames);

    PEG_METHOD_EXIT();
    return nameSpaceNames;
}

void DefaultRepository::deleteNameSpace(
    bool lock,
    const CIMNamespaceName& nameSpace)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "DefaultRepository::deleteNameSpace");

    WriteLock wlock(_lock, lock);
    AutoFileLock fileLock(_lockFile, lock);
    _nameSpaceManager.deleteNameSpace(nameSpace);

    PEG_METHOD_EXIT();
}

Boolean DefaultRepository::getNameSpaceAttributes(
    bool lock,
    const CIMNamespaceName& nameSpace,
        NameSpaceAttributes& attributes)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "DefaultRepository::deleteNameSpace");

    ReadLock rlock(const_cast<ReadWriteSem&>(_lock), lock);
    attributes.clear();
    PEG_METHOD_EXIT();
    return _nameSpaceManager.getNameSpaceAttributes(nameSpace, attributes);
}

Boolean DefaultRepository::isRemoteNameSpace(
    bool lock,
    const CIMNamespaceName& nameSpaceName,
    String& remoteInfo)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "DefaultRepository::isRemoteNamespace");
    ReadLock rlock(const_cast<ReadWriteSem&>(_lock), lock);
    PEG_METHOD_EXIT();
    return _nameSpaceManager.isRemoteNameSpace(nameSpaceName, remoteInfo);
}

//----------------------------------------------------------------------
//
// _getInstanceIndexFilePath()
//
//      returns the file path of the instance index file.
//
//----------------------------------------------------------------------

String DefaultRepository::_getInstanceIndexFilePath(
    const CIMNamespaceName& nameSpace,
    const CIMName& className) const
{
    PEG_METHOD_ENTER(TRC_REPOSITORY,
        "DefaultRepository::_getInstanceIndexFilePath");

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

String DefaultRepository::_getInstanceDataFilePath(
    const CIMNamespaceName& nameSpace,
    const CIMName& className) const
{
    PEG_METHOD_ENTER(
        TRC_REPOSITORY, "DefaultRepository::_getInstanceDataFilePath");

    String tmp = _nameSpaceManager.getInstanceDataFileBase(
        nameSpace, className);
    tmp.append(".instances");

    PEG_METHOD_EXIT();
    return tmp;
}

Boolean DefaultRepository::_loadInstance(
    const String& path,
    CIMInstance& object,
    Uint32 index,
    Uint32 size)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "DefaultRepository::_loadInstance");

    //
    // Load instance (in XML) from instance file into memory:
    //

    Buffer data;

    if (!InstanceDataFile::loadInstance(path, index, size, data))
    {
        PEG_METHOD_EXIT();
        return false;
    }

    //
    // Convert XML into an actual object:
    //

    streamer->decode(data, 0, object);
    //XmlParser parser((char*)data.getData());
    //XmlReader::getObject(parser, object);

    PEG_METHOD_EXIT();
    return true;
}

void DefaultRepository::setDeclContext(
    bool lock,
    RepositoryDeclContext* context)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "DefaultRepository::setDeclContext");

    WriteLock wlock(_lock, lock);
    AutoFileLock fileLock(_lockFile, lock);
    _context = context;

    PEG_METHOD_EXIT();
}

#ifdef PEGASUS_DEBUG
void DefaultRepository::DisplayCacheStatistics(
    bool lock)
{
#ifdef PEGASUS_USE_CLASS_CACHE
    cout << "Repository Class Cache Statistics:" << endl;
    _classCache.DisplayCacheStatistics();
#endif
    cout << "Repository Qualifier Cache Statistics:" << endl;
    _qualifierCache.DisplayCacheStatistics();
}
#endif

void DefaultRepository::getSubClassNames(
    bool lock,
    const CIMNamespaceName& nameSpaceName,
    const CIMName& className,
    Boolean deepInheritance,
    Array<CIMName>& subClassNames) const
{
    ReadLock rlock(const_cast<ReadWriteSem&>(_lock), lock);
    _nameSpaceManager.getSubClassNames(nameSpaceName,
                                       className,
                                       deepInheritance,
                                       subClassNames);
}

/** Get the names of all superclasses (direct and indirect) of this
    class.
*/
void DefaultRepository::getSuperClassNames(
    bool lock,
    const CIMNamespaceName& nameSpaceName,
    const CIMName& className,
    Array<CIMName>& superClassNames) const
{
    ReadLock rlock(const_cast<ReadWriteSem&>(_lock), lock);
    _nameSpaceManager.getSuperClassNames(
        nameSpaceName, className, superClassNames);
}

PEGASUS_NAMESPACE_END
