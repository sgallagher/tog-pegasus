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
#include <Pegasus/Common/InternalException.h>

#include <Pegasus/Common/DeclContext.h>
#include <Pegasus/Common/Resolver.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/PegasusVersion.h>
#include <Pegasus/Common/MessageLoader.h>
#include <Pegasus/Common/ReadWriteSem.h>

#include <Pegasus/Common/XmlStreamer.h>
#include <Pegasus/Common/BinaryStreamer.h>
#include <Pegasus/Common/AutoStreamer.h>

#include "CIMRepository.h"
#include "RepositoryDeclContext.h"
#include "ObjectCache.h"

#include "FileBasedStore.h"

#if 0
#undef PEG_METHOD_ENTER
#undef PEG_METHOD_EXIT
#define PEG_METHOD_ENTER(x,y)  cout<<"--- Enter: "<<y<<endl;
#define PEG_METHOD_EXIT()
#endif


PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

class CIMRepositoryRep
{
public:

    CIMRepositoryRep()
        : _resolveInstance(true)
    {
    }

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

    /** Used by getInstance(); indicates whether instance should be resolved
        after it is retrieved from the file.
    */
    Boolean _resolveInstance;

    // This must be initialized in the constructor using values from the
    // ConfigManager.
    Boolean _isDefaultInstanceProvider;

    AutoPtr<ObjectStreamer> _streamer;

    AutoPtr<FileBasedStore> _persistentStore;

    AutoPtr<NameSpaceManager> _nameSpaceManager;

    ReadWriteSem _lock;

    RepositoryDeclContext* _context;

    CString _lockFile;
};

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

static String _getCacheKey(
    const CIMNamespaceName& nameSpace,
    const CIMName& entryName)
{
    return nameSpace.getString() + ":" + entryName.getString();
}


//
//  The following _xx functions are local to the repository implementation
//
////////////////////////////////////////////////////////////////////////////////
//
//   _containsProperty
//
////////////////////////////////////////////////////////////////////////////////

/** Check to see if the specified property is in the property list
    @param property the specified property
    @param propertyList the property list
    @return true if the property is in the list otherwise false.
*/
static Boolean _containsProperty(
    CIMProperty& property,
    const CIMPropertyList& propertyList)
{
    //  For each property in the propertly list
    for (Uint32 p=0; p<propertyList.size(); p++)
    {
        if (propertyList[p].equal(property.getName()))
            return true;
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////
//
// removeAllQualifiers - Remove all of the qualifiers from a class
//
////////////////////////////////////////////////////////////////////////////////

/* removes all Qualifiers from a CIMClass.  This function removes all
   of the qualifiers from the class, from all of the properties,
   from the methods, and from the parameters attached to the methods.
   @param cimClass reference to the class from which qualifiers are to
   be removed.
   NOTE: This would be logical to be moved to CIMClass since it may be
   more general than this usage.
*/
static void _removeAllQualifiers(CIMClass& cimClass)
{
    // remove qualifiers of the class
    Uint32 count = 0;
    while ((count = cimClass.getQualifierCount()) > 0)
        cimClass.removeQualifier(count - 1);

    // remove qualifiers from the properties
    for (Uint32 i = 0; i < cimClass.getPropertyCount(); i++)
    {
        CIMProperty p = cimClass.getProperty(i);
        count = 0;
        while ((count = p.getQualifierCount()) > 0)
            p.removeQualifier(count - 1);
    }

    // remove qualifiers from the methods
    for (Uint32 i = 0; i < cimClass.getMethodCount(); i++)
    {
        CIMMethod m = cimClass.getMethod(i);
        for (Uint32 j = 0 ; j < m.getParameterCount(); j++)
        {
            CIMParameter p = m.getParameter(j);
            count = 0;
            while ((count = p.getQualifierCount()) > 0)
                p.removeQualifier(count - 1);
        }
        count = 0;
        while ((count = m.getQualifierCount()) > 0)
            m.removeQualifier(count - 1);
    }
}

/////////////////////////////////////////////////////////////////////////
//
// _removePropagatedQualifiers - Removes all qualifiers from the class
// that are marked propagated
//
/////////////////////////////////////////////////////////////////////////

/* removes propagatedQualifiers from the defined CIMClass.
   This function removes the qualifiers from the class,
   from each of the properties, from the methods and
   the parameters if the qualifiers are marked propagated.
   NOTE: This could be logical to be moved to CIMClass since it may be
   more general than the usage here.
*/
static void _removePropagatedQualifiers(CIMClass& cimClass)
{
    Uint32 count = cimClass.getQualifierCount();
    // Remove nonlocal qualifiers from Class
    for (Sint32 i = (count - 1); i >= 0; i--)
    {
        CIMQualifier q = cimClass.getQualifier(i);
        if (q.getPropagated())
        {
            cimClass.removeQualifier(i);
        }
    }

    // remove  non localOnly qualifiers from the properties
    for (Uint32 i = 0; i < cimClass.getPropertyCount(); i++)
    {
        CIMProperty p = cimClass.getProperty(i);
        // loop to search qualifiers for nonlocal parameters
        count = p.getQualifierCount();
        for (Sint32 j = (count - 1); j >= 0; j--)
        {
            CIMQualifier q = p.getQualifier(j);
            if (q.getPropagated())
            {
                p.removeQualifier(j);
            }
        }
    }

    // remove non LocalOnly qualifiers from the methods and parameters
    for (Uint32 i = 0; i < cimClass.getMethodCount(); i++)
    {
        CIMMethod m = cimClass.getMethod(i);
        // Remove  nonlocal qualifiers from all parameters
        for (Uint32 j = 0 ; j < m.getParameterCount(); j++)
        {
            CIMParameter p = m.getParameter(j);
            count = p.getQualifierCount();
            for (Sint32 k = (count - 1); k >= 0; k--)
            {
                CIMQualifier q = p.getQualifier(k);
                if (q.getPropagated())
                {
                    p.removeQualifier(k);
                }
            }
        }

        // remove nonlocal qualifiers from the method
        count = m.getQualifierCount();
        for (Sint32 j = (count - 1); j >= 0; j--)
        {
            CIMQualifier q = m.getQualifier(j);
            if (q.getPropagated())
            {
                m.removeQualifier(j);
            }
        }
    }
}

/* remove the properties from an instance based on attributes.
    @param Instance from which properties will be removed.
    @param propertyList PropertyList is used in the removal algorithm
    @param localOnly - Boolean used in the removal.
    NOTE: This could be logical to move to CIMInstance since the
    usage is more general than just in the repository
*/
static void _removeProperties(
    CIMInstance& cimInstance,
    const CIMPropertyList& propertyList,
    Boolean localOnly)
{
    Boolean propertyListNull = propertyList.isNull();
    if ((!propertyListNull) || localOnly)
    {
        // Loop through properties to remove those that do not filter through
        // local only attribute and are not in the property list.
        Uint32 count = cimInstance.getPropertyCount();
        // Work backwards because removal may be cheaper. Sint32 covers count=0
        for (Sint32 i = (count - 1); i >= 0; i--)
        {
            CIMProperty p = cimInstance.getProperty(i);

            // if localOnly == true, ignore properties defined in super class
            if (localOnly && (p.getPropagated()))
            {
                cimInstance.removeProperty(i);
                continue;
            }

            // propertyList NULL means deliver properties.  PropertyList
            // empty, none.
            // Test for removal if propertyList not NULL. The empty list option
            // is covered by fact that property is not in the list.
            if (!propertyListNull)
                if (!_containsProperty(p, propertyList))
                    cimInstance.removeProperty(i);
        }
    }
}

/* remove all Qualifiers from a single CIMInstance. Removes
    all of the qualifiers from the instance and from properties
    within the instance.
    @param instance from which parameters are removed.
    NOTE: This could be logical to be moved to CIMInstance since
    the usage may be more general than just in the repository.
*/
static void _removeAllQualifiers(CIMInstance& cimInstance)
{
    // remove qualifiers from the instance
    Uint32 count = 0;
    while ((count = cimInstance.getQualifierCount()) > 0)
        cimInstance.removeQualifier(count - 1);

    // remove qualifiers from the properties
    for (Uint32 i = 0; i < cimInstance.getPropertyCount(); i++)
    {
        CIMProperty p = cimInstance.getProperty(i);
        count = 0;
        while ((count = p.getQualifierCount()) > 0)
            p.removeQualifier(count - 1);
    }
}

/* removes all ClassOrigin attributes from a single CIMInstance. Removes
    the classOrigin attribute from each property in the Instance.
   @param Instance from which the ClassOrigin Properties will be removed.
   NOTE: Logical to be moved to CIMInstance since it may be more general
   than just the repositoryl
*/
static void _removeClassOrigins(CIMInstance& cimInstance)
{
    PEG_TRACE_CSTRING(TRC_REPOSITORY, Tracer::LEVEL4, "Remove Class Origins");

    Uint32 propertyCount = cimInstance.getPropertyCount();
    for (Uint32 i = 0; i < propertyCount ; i++)
        cimInstance.getProperty(i).setClassOrigin(CIMName());
}

/* Filters the properties, qualifiers, and classorigin out of a single instance.
    Based on the parameters provided for localOnly, includeQualifiers,
    and includeClassOrigin, this function simply filters the properties
    qualifiers, and classOrigins out of a single instance.  This function
    was created to have a single piece of code that processes getinstance
    and enumerateInstances returns.
    @param cimInstance reference to instance to be processed.
    @param localOnly defines if request is for localOnly parameters.
    @param includeQualifiers Boolean defining if qualifiers to be returned.
    @param includeClassOrigin Boolean defining if ClassOrigin attribute to
    be removed from properties.
*/
static void _filterInstance(
    CIMInstance& cimInstance,
    const CIMPropertyList& propertyList,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin)
{
    // Remove properties based on propertyList and localOnly flag
    _removeProperties(cimInstance, propertyList, localOnly);

    // If includequalifiers false, remove all qualifiers from
    // properties.

    if (!includeQualifiers)
    {
        _removeAllQualifiers(cimInstance);
    }

    // if ClassOrigin Flag false, remove classOrigin info from Instance object
    // by setting the classOrigin to Null.

    if (!includeClassOrigin)
    {
        _removeClassOrigins(cimInstance);
    }
}

static Array<ClassAssociation> _buildClassAssociationEntries(
    const CIMConstClass& assocClass)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "_buildClassAssociationEntries");

    Array<ClassAssociation> classAssocEntries;

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
                    (!fromProp.getName().equal(toProp.getName())))
                {
                    classAssocEntries.append(ClassAssociation(
                        assocClassName,
                        fromProp.getReferenceClassName(),
                        fromProp.getName(),
                        toProp.getReferenceClassName(),
                        toProp.getName()));
                }
            }
        }
    }

    PEG_METHOD_EXIT();
    return classAssocEntries;
}

/*
    This routine does the following:

        1.  Creates two entries in the association file for each relationship
            formed by this new association instance. A binary association
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

            So for an N-ary relationship, there will be N! entries created.

        2.  Verifies that the association instance refers to real objects.
            (note that an association reference may refer to either an instance
            or a class). Throws an exception if one of the references does not
            refer to a valid object.
*/
static Array<InstanceAssociation> _buildInstanceAssociationEntries(
    const CIMNamespaceName& nameSpace,
    const CIMConstClass& cimClass,
    const CIMInstance& cimInstance,
    const CIMObjectPath& instanceName)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "_buildInstanceAssociationEntries");

    Array<InstanceAssociation> instanceAssocEntries;

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

            for (Uint32 j = 0, m = cimInstance.getPropertyCount(); j < m; j++)
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
                            "Stripping off local hostName from fromRef");
                        fromRef.setHost(String::EMPTY);
                    }

                    // Strip off the namespace when it is the same as the
                    // one this instance is created in.
                    if ((fromRef.getHost() == String::EMPTY) &&
                        (fromRef.getNameSpace() == nameSpace))
                    {
                        PEG_TRACE_CSTRING(TRC_REPOSITORY, Tracer::LEVEL4,
                            "Stripping off local nameSpace from fromRef");
                        fromRef.setNameSpace(CIMNamespaceName());
                    }

                    // Strip off the hostname if it is the same as the
                    // local host
                    if ((toRef.getHost() != String::EMPTY) &&
                        (System::isLocalHost(toRef.getHost())))
                    {
                        PEG_TRACE_CSTRING(TRC_REPOSITORY, Tracer::LEVEL4,
                            "Stripping off local hostName from toRef");
                        toRef.setHost(String::EMPTY);
                    }

                    // Strip off the namespace when it is the same as the
                    // one this instance is created in.
                    if ((toRef.getHost() == String::EMPTY) &&
                        (toRef.getNameSpace() == nameSpace))
                    {
                        PEG_TRACE_CSTRING(TRC_REPOSITORY, Tracer::LEVEL4,
                            "Stripping off local nameSpace from toRef");
                        toRef.setNameSpace(CIMNamespaceName());
                    }

                    instanceAssocEntries.append(InstanceAssociation(
                        assocInstanceName,
                        assocClassName,
                        fromRef.toString(),
                        fromRef.getClassName(),
                        fromProp.getName(),
                        toRef.toString(),
                        toRef.getClassName(),
                        toProp.getName()));
                }
            }
        }
    }

    PEG_METHOD_EXIT();
    return instanceAssocEntries;
}

/**
    Converts an object path to an instance name.  The host name is set to the
    empty string.  The namespace is set to null if it matches the specified
    namespace.  Otherwise, if it is not null, a CIM_ERR_NOT_FOUND exception is
    thrown.

    This function allows the repository to store instance names with
    consistent contents to facilitate equality tests.  (See Bug 1508.)

*/
static CIMObjectPath _stripInstanceName(
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& instanceName)
{
    CIMObjectPath normalizedInstanceName = instanceName;
    normalizedInstanceName.setHost(String::EMPTY);

    if (instanceName.getNameSpace() == nameSpace)
    {
        normalizedInstanceName.setNameSpace(CIMNamespaceName());
    }
    else if (!instanceName.getNameSpace().isNull())
    {
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_FOUND, instanceName.toString());
    }

    return normalizedInstanceName;
}

////////////////////////////////////////////////////////////////////////////////
//
// CIMRepository
//
//     The following are not implemented:
//
//         CIMRepository::execQuery()
//         CIMRepository::invokeMethod()
//
//     Note that invokeMethod() will not never implemented since it is not
//     meaningful for a repository.
//
//     Note that if declContext is passed to the CIMRepository constructor,
//     the repository object will own it and will delete it when appropriate.
//
////////////////////////////////////////////////////////////////////////////////

CIMRepository::CIMRepository(
    const String& repositoryRoot,
    Uint32 mode,
    RepositoryDeclContext* declContext)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::CIMRepository");

    Boolean binaryMode = mode & CIMRepository::MODE_BIN;

    if (mode == CIMRepository::MODE_DEFAULT)
    {
        binaryMode = ConfigManager::parseBooleanValue(
            ConfigManager::getInstance()->getCurrentValue(
                "enableBinaryRepository"));
    }

    // FUTURE?? -  compressMode = mode & CIMRepository::MODE_COMPRESSED;
    Boolean compressMode = false;

#ifdef PEGASUS_ENABLE_COMPRESSED_REPOSITORY    // PEP214
    Boolean compressMode = true;
    char* s = getenv("PEGASUS_ENABLE_COMPRESSED_REPOSITORY");
    if (s && (strcmp(s, "build_non_compressed") == 0))
    {
        compressMode = false;
#ifdef TEST_OUTPUT
        cout << "In Compress mode: build_non_compresed found" << endl;
#endif /* TEST_OUTPUT */
    }
#endif /* PEGASUS_ENABLE_COMPRESSED_REPOSITORY */

#ifdef TEST_OUTPUT
    cout << "repositoryRoot = " << repositoryRoot << endl;
    cout << "CIMRepository: binaryMode="  << binaryMode <<
        ", mode=" << mode << endl;
    cout << "CIMRepository: compressMode= " << compressMode << endl;
#endif /* TEST_OUTPUT */

    _rep = new CIMRepositoryRep();

    if (binaryMode)
    {
        // BUILD BINARY
        _rep->_streamer.reset(
            new AutoStreamer(new BinaryStreamer(), BINREP_MARKER));
        ((AutoStreamer*)_rep->_streamer.get())->addReader(new XmlStreamer(), 0);
    }
    else
    {
        // BUILD XML
        _rep->_streamer.reset(new AutoStreamer(new XmlStreamer(), 0xff));
        ((AutoStreamer*)_rep->_streamer.get())->addReader(
            new BinaryStreamer(), BINREP_MARKER);
        ((AutoStreamer*)_rep->_streamer.get())->addReader(new XmlStreamer(), 0);
    }

    // If declContext is supplied by the caller, don't allocate it.
    // CIMRepository will take ownership and will be responsible for 
    // deleting it.
    if (declContext)
        _rep->_context = declContext;
    else
        _rep->_context = new RepositoryDeclContext();
    _rep->_context->setRepository(this);

    _rep->_isDefaultInstanceProvider = ConfigManager::parseBooleanValue(
        ConfigManager::getInstance()->getCurrentValue(
            "repositoryIsDefaultInstanceProvider"));

    _rep->_lockFile = ConfigManager::getInstance()->getHomedPath(
        PEGASUS_REPOSITORY_LOCK_FILE).getCString();

    {
        AutoFileLock fileLock(_rep->_lockFile);

        _rep->_persistentStore.reset(new FileBasedStore(
            repositoryRoot,
            _rep->_streamer.get(),
            compressMode));

        _rep->_nameSpaceManager.reset(
            new NameSpaceManager(_rep->_persistentStore.get()));
    }

    if (!_rep->_nameSpaceManager->nameSpaceExists("root"))
    {
        // Create a root namespace per ...
        // Specification for CIM Operations over HTTP
        // Version 1.0
        // 2.5 Namespace Manipulation
        //
        // There are no intrinsic methods defined specifically for the
        // purpose of manipulating CIM Namespaces.  However, the
        // modelling of the a CIM Namespace using the class
        // __Namespace, together with the requirement that that
        // root Namespace MUST be supported by all CIM Servers,
        // implies that all Namespace operations can be supported.

        createNameSpace("root");
    }

    PEG_METHOD_EXIT();
}

CIMRepository::~CIMRepository()
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::~CIMRepository");

    delete _rep->_context;

    delete _rep;

    PEG_METHOD_EXIT();
}

String _toString(Boolean x)
{
    return(x ? "true" : "false");
}

CIMClass CIMRepository::getClass(
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::getClass");

    ReadLock lock(_rep->_lock);
    CIMClass cimClass = _getClass(nameSpace,
                                  className,
                                  localOnly,
                                  includeQualifiers,
                                  includeClassOrigin,
                                  propertyList);

    PEG_METHOD_EXIT();
    return cimClass;
}

CIMClass CIMRepository::_getClass(
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::_getClass");

    PEG_TRACE_STRING(TRC_REPOSITORY, Tracer::LEVEL4, "nameSpace= " +
                     nameSpace.getString() + ", className= " +
                     className.getString() +
                     ", localOnly= " + _toString(localOnly) +
                     ", includeQualifiers= " + _toString(includeQualifiers) +
                     ", includeClassOrigin= " + _toString(includeClassOrigin));

    CIMClass cimClass;

#ifdef PEGASUS_USE_CLASS_CACHE
    // Check the cache first:

    String cacheKey = _getCacheKey(nameSpace, className);

    if (!_classCache.get(cacheKey, cimClass))
    {
        // Not in cache so load from disk:
#endif

        CIMNamespaceName actualNameSpaceName;
        CIMName superClassName;
        _rep->_nameSpaceManager->locateClass(
            nameSpace, className, actualNameSpaceName, superClassName);

        cimClass = _rep->_persistentStore->getClass(
            actualNameSpaceName, className, superClassName);

#ifdef PEGASUS_USE_CLASS_CACHE
        // Put in cache:

        _classCache.put(cacheKey, cimClass);
    }
#endif

    // Remove properties based on propertylist and localOnly flag (Bug 565)
    Boolean propertyListNull = propertyList.isNull();

    // if localOnly OR there is a property list, process properties
    if ((!propertyListNull) || localOnly)
    {
        // Loop through properties to remove those that do not filter through
        // local only attribute and are not in the property list.
        Uint32 count = cimClass.getPropertyCount();
        // Work backwards because removal may be cheaper. Sint32 covers count=0
        for (Sint32 i = (count - 1); i >= 0; i--)
        {
            CIMProperty p = cimClass.getProperty(i);
            // if localOnly==true, ignore properties defined in super class
            if (localOnly && (p.getPropagated()))
            {
                cimClass.removeProperty(i);
                continue;
            }

            // propertyList NULL means all properties.  PropertyList
            // empty, none.
            // Test for removal if propertyList not NULL. The empty list option
            // is covered by fact that property is not in the list.
            if (!propertyListNull)
                if (!_containsProperty(p, propertyList))
                    cimClass.removeProperty(i);
        }
    }

    // remove methods based on localOnly flag
    if (localOnly)
    {
        Uint32 count = cimClass.getMethodCount();
        // Work backwards because removal may be cheaper.
        for (Sint32 i = (count - 1); i >= 0; i--)
        {
            CIMMethod m = cimClass.getMethod(i);

            // if localOnly==true, ignore properties defined in super class
            if (localOnly && (m.getPropagated()))
                cimClass.removeMethod(i);
        }

    }
    // If includequalifiers false, remove all qualifiers from
    // properties, methods and parameters.
    if (!includeQualifiers)
    {

        _removeAllQualifiers(cimClass);
    }
    else
    {
        // if includequalifiers and localOnly, remove nonLocal qualifiers
        if (localOnly)
        {
            _removePropagatedQualifiers(cimClass);
        }

    }

    // if ClassOrigin Flag false, remove classOrigin info from class object
    // by setting the property to Null.
    if (!includeClassOrigin)
    {
        PEG_TRACE_CSTRING(TRC_REPOSITORY, Tracer::LEVEL4,
            "Remove Class Origins");

        Uint32 propertyCount = cimClass.getPropertyCount();
        for (Uint32 i = 0; i < propertyCount ; i++)
            cimClass.getProperty(i).setClassOrigin(CIMName());

        Uint32 methodCount =  cimClass.getMethodCount();
        for (Uint32 i=0; i < methodCount ; i++)
            cimClass.getMethod(i).setClassOrigin(CIMName());
    }

    PEG_METHOD_EXIT();
    return cimClass;
}

Boolean CIMRepositoryRep::_checkInstanceAlreadyExists(
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& instanceName) const
{
    PEG_METHOD_ENTER(TRC_REPOSITORY,
        "CIMRepository::_checkInstanceAlreadyExists");

    //
    // Get the names of all superclasses and subclasses of this class
    //

    Array<CIMName> classNames;
    CIMName className = instanceName.getClassName();
    classNames.append(className);
    _nameSpaceManager->getSubClassNames(nameSpace, className, true, classNames);
    _nameSpaceManager->getSuperClassNames(nameSpace, className, classNames);

    //
    // Search for an instance with the specified key values
    //

    for (Uint32 i = 0; i < classNames.size(); i++)
    {
        CIMObjectPath tmpInstanceName = CIMObjectPath(
            String::EMPTY,
            CIMNamespaceName(),
            classNames[i],
            instanceName.getKeyBindings());

        if (_persistentStore->instanceExists(nameSpace, tmpInstanceName))
        {
            PEG_METHOD_EXIT();
            return true;
        }
    }

    PEG_METHOD_EXIT();
    return false;
}

CIMInstance CIMRepository::getInstance(
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& instanceName,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::getInstance");

    ReadLock lock(_rep->_lock);

    CIMInstance cimInstance = _getInstance(
        nameSpace,
        instanceName,
        localOnly,
        includeQualifiers,
        includeClassOrigin,
        propertyList);

    PEG_METHOD_EXIT();
    return cimInstance;
}

CIMInstance CIMRepository::_getInstance(
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& instanceName,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::_getInstance");

    CIMObjectPath normalizedInstanceName =
        _stripInstanceName(nameSpace, instanceName);

    if (!_rep->_nameSpaceManager->classExists(
        nameSpace, instanceName.getClassName()))
    {
        throw PEGASUS_CIM_EXCEPTION(
            CIM_ERR_INVALID_CLASS, instanceName.getClassName().getString());
    }

    CIMInstance cimInstance =
        _rep->_persistentStore->getInstance(nameSpace, normalizedInstanceName);

    //
    // Resolve the instance (if requested):
    //

    if (_rep->_resolveInstance)
    {
        CIMConstClass cimClass;
        Resolver::resolveInstance (
            cimInstance, _rep->_context, nameSpace, cimClass, true);
    }

    _filterInstance(
        cimInstance,
        propertyList,
        localOnly,
        includeQualifiers,
        includeClassOrigin);

    PEG_METHOD_EXIT();
    return cimInstance;
}

void CIMRepository::deleteClass(
    const CIMNamespaceName& nameSpace,
    const CIMName& className)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY,"CIMRepository::deleteClass");

    WriteLock lock(_rep->_lock);
    AutoFileLock fileLock(_rep->_lockFile);

    //
    // Get the class and check to see if it is an association class.
    //

    CIMClass cimClass = _getClass(
        nameSpace, className, false, true, false, CIMPropertyList());
    Boolean isAssociation = cimClass.isAssociation();

    _rep->_nameSpaceManager->checkDeleteClass(nameSpace, className);

    Array<CIMNamespaceName> dependentNameSpaceNames =
        _rep->_nameSpaceManager->getDependentSchemaNameSpaceNames(nameSpace);

    //
    // Ensure no instances of this class exist in the repository.
    //

    for (Uint32 i = 0; i < dependentNameSpaceNames.size(); i++)
    {
        Array<CIMObjectPath> instanceNames =
            _rep->_persistentStore->enumerateInstanceNamesForClass(
                dependentNameSpaceNames[i], className);

        if (instanceNames.size())
        {
            throw PEGASUS_CIM_EXCEPTION(
                CIM_ERR_CLASS_HAS_INSTANCES, className.getString());
        }
    }

#ifdef PEGASUS_USE_CLASS_CACHE

    _classCache.evict(_getCacheKey(nameSpace, className));

#endif /* PEGASUS_USE_CLASS_CACHE */

    //
    // Delete the class. The NameSpaceManager::deleteClass() method throws
    // an exception if the class has subclasses.
    //

    CIMName superClassName =
        _rep->_nameSpaceManager->getSuperClassName(nameSpace, className);

    _rep->_nameSpaceManager->deleteClass(nameSpace, className);

    _rep->_persistentStore->deleteClass(
        nameSpace,
        className,
        superClassName,
        isAssociation,
        dependentNameSpaceNames);

    PEG_METHOD_EXIT();
}

void CIMRepository::deleteInstance(
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& instanceName)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::deleteInstance");

    _rep->_nameSpaceManager->validateClass(
        nameSpace, instanceName.getClassName());

    CIMObjectPath normalizedInstanceName =
        _stripInstanceName(nameSpace, instanceName);

    WriteLock lock(_rep->_lock);
    AutoFileLock fileLock(_rep->_lockFile);

    _rep->_persistentStore->deleteInstance(nameSpace, normalizedInstanceName);

    PEG_METHOD_EXIT();
}

void CIMRepository::createClass(
    const CIMNamespaceName& nameSpace,
    const CIMClass& newClass,
    const ContentLanguageList& contentLangs)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::createClass");

    WriteLock lock(_rep->_lock);
    AutoFileLock fileLock(_rep->_lockFile);
    _createClass(nameSpace, newClass);

    PEG_METHOD_EXIT();
}

void CIMRepository::_createClass(
    const CIMNamespaceName& nameSpace,
    const CIMClass& newClass)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::_createClass");

    // -- Resolve the class:
    // ATTN: This casts away constness
    CIMClass cimClass(newClass);

    Resolver::resolveClass (cimClass, _rep->_context, nameSpace);

    // -- Check whether the class may be created:

    _rep->_nameSpaceManager->checkCreateClass(
        nameSpace, cimClass.getClassName(), cimClass.getSuperClassName());

    // -- If an association class, build association entries:

    Array<ClassAssociation> classAssocEntries;

    if (cimClass.isAssociation())
    {
        classAssocEntries = _buildClassAssociationEntries(cimClass);
    }

    // -- Create the class declaration:

    _rep->_persistentStore->createClass(nameSpace, cimClass, classAssocEntries);

    // -- Create namespace manager entry:

    _rep->_nameSpaceManager->createClass(
        nameSpace, cimClass.getClassName(), cimClass.getSuperClassName());

    PEG_METHOD_EXIT();
}

CIMObjectPath CIMRepository::createInstance(
    const CIMNamespaceName& nameSpace,
    const CIMInstance& newInstance,
    const ContentLanguageList& contentLangs)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::createInstance");

    WriteLock lock(_rep->_lock);
    AutoFileLock fileLock(_rep->_lockFile);
    CIMObjectPath instanceName = _createInstance(nameSpace, newInstance);

    PEG_METHOD_EXIT();
    return instanceName;
}

CIMObjectPath CIMRepository::_createInstance(
    const CIMNamespaceName& nameSpace,
    const CIMInstance& newInstance)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::_createInstance");

    //
    // Resolve the instance. Looks up class and fills out properties but
    // not the qualifiers.
    //

    CIMInstance cimInstance(newInstance);
    CIMConstClass cimClass;
    Resolver::resolveInstance (cimInstance, _rep->_context, nameSpace, cimClass,
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

    if (_rep->_checkInstanceAlreadyExists(nameSpace, instanceName))
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_ALREADY_EXISTS,
            instanceName.toString());
    }

    //
    // Build association entries if an association instance.
    //

    Array<InstanceAssociation> instAssocEntries;

    if (cimClass.isAssociation())
    {
        instAssocEntries = _buildInstanceAssociationEntries(
            nameSpace, cimClass, cimInstance, instanceName);
    }

    //
    // Create the instance
    //

    _rep->_persistentStore->createInstance(
        nameSpace, instanceName, cimInstance, instAssocEntries);

    // ATTN: Why does this come after the create has completed?
    Resolver::resolveInstance (
        cimInstance, _rep->_context, nameSpace, cimClass, true);

    PEG_METHOD_EXIT();
    return instanceName;
}

void CIMRepository::modifyClass(
    const CIMNamespaceName& nameSpace,
    const CIMClass& modifiedClass,
    const ContentLanguageList& contentLangs)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::modifyClass");

    WriteLock lock(_rep->_lock);
    AutoFileLock fileLock(_rep->_lockFile);
    _modifyClass(nameSpace, modifiedClass);

    PEG_METHOD_EXIT();
}

void CIMRepository::_modifyClass(
    const CIMNamespaceName& nameSpace,
    const CIMClass& modifiedClass)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::_modifyClass");

    //
    // Resolve the class:
    //

    CIMClass cimClass(modifiedClass);
    Resolver::resolveClass(cimClass, _rep->_context, nameSpace);

    //
    // Check to see if it is okay to modify this class:
    //

    _rep->_nameSpaceManager->checkModifyClass(
        nameSpace, cimClass.getClassName(), cimClass.getSuperClassName());

    //
    // ATTN: KS
    // Disallow modification of classes which have instances (that are
    // in the repository). And we have no idea whether the class has
    // instances in other repositories or in providers. We should do
    // an enumerate instance names at a higher level (above the repository).
    //

#ifdef PEGASUS_USE_CLASS_CACHE

    String cacheKey = _getCacheKey(nameSpace, cimClass.getClassName());
    _classCache.evict(cacheKey);

#endif /* PEGASUS_USE_CLASS_CACHE */

    Boolean isAssociation = cimClass.isAssociation();
    Array<ClassAssociation> classAssocEntries;

    if (isAssociation)
    {
        classAssocEntries = _buildClassAssociationEntries(cimClass);
    }

    _rep->_persistentStore->modifyClass(
        nameSpace, cimClass, isAssociation, classAssocEntries);

    //
    // Cache this class:
    //

#ifdef PEGASUS_USE_CLASS_CACHE

    _classCache.put(cacheKey, cimClass);

#endif /* PEGASUS_USE_CLASS_CACHE */

    PEG_METHOD_EXIT();
}

void CIMRepository::modifyInstance(
    const CIMNamespaceName& nameSpace,
    const CIMInstance& modifiedInstance,
    Boolean includeQualifiers,
    const CIMPropertyList& propertyList,
    const ContentLanguageList& contentLangs)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::modifyInstance");

    WriteLock lock(_rep->_lock);
    AutoFileLock fileLock(_rep->_lockFile);

    //
    // Do this:
    //

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

            _rep->_resolveInstance = false;

            cimInstance = _getInstance(
                nameSpace,
                modifiedInstance.getPath (),
                false,
                true,
                true,
                CIMPropertyList());

            _rep->_resolveInstance = true;

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

        _rep->_resolveInstance = false;

        cimInstance = _getInstance(nameSpace,
            modifiedInstance.getPath (), false, true, true, CIMPropertyList());

        _rep->_resolveInstance = true;

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

    CIMObjectPath normalizedInstanceName =
        _stripInstanceName(nameSpace, modifiedInstance.getPath());

    //
    // Resolve the instance (do not propagate qualifiers from class since
    // this will bloat the instance).
    //

    CIMConstClass cimClass;
    Resolver::resolveInstance(
        cimInstance, _rep->_context, nameSpace, cimClass, false);

    //
    // Disallow operation if the instance name was changed:
    //

    if (cimInstance.buildPath(cimClass) != normalizedInstanceName)
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
            MessageLoaderParms(
                "Repository.CIMRepository.ATTEMPT_TO_MODIFY_KEY_PROPERTY",
                "Attempted to modify a key property"));
    }

    _rep->_persistentStore->modifyInstance(
        nameSpace, normalizedInstanceName, cimInstance);

    //
    // Resolve the instance:
    //

    // ATTN: Why does this come after the modify has completed?
    Resolver::resolveInstance (
        cimInstance, _rep->_context, nameSpace, cimClass, true);

    PEG_METHOD_EXIT();
}

Array<CIMClass> CIMRepository::enumerateClasses(
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    Boolean deepInheritance,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::enumerateClasses");

    ReadLock lock(_rep->_lock);

    Array<CIMName> classNames;

    _rep->_nameSpaceManager->getSubClassNames(
        nameSpace, className, deepInheritance, classNames);

    Array<CIMClass> result;

    for (Uint32 i = 0; i < classNames.size(); i++)
    {
        result.append(_getClass(nameSpace, classNames[i], localOnly,
            includeQualifiers, includeClassOrigin, CIMPropertyList()));
    }

    PEG_METHOD_EXIT();
    return result;
}

Array<CIMName> CIMRepository::enumerateClassNames(
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    Boolean deepInheritance)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::enumerateClassNames");

    ReadLock lock(_rep->_lock);

    Array<CIMName> classNames;

    _rep->_nameSpaceManager->getSubClassNames(
        nameSpace, className, deepInheritance, classNames,true);

    PEG_METHOD_EXIT();
    return classNames;
}

Array<CIMInstance> CIMRepository::enumerateInstancesForSubtree(
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    Boolean deepInheritance,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY,
        "CIMRepository::enumerateInstancesForSubtree");

    // It is not necessary to control access to the ReadWriteSem lock here.
    // This method calls enumerateInstancesForClass, which does its own
    // access control.

    //
    // Get all descendent classes of this class:
    //

    Array<CIMName> classNames;
    classNames.append(className);
    _rep->_nameSpaceManager->getSubClassNames(
        nameSpace, className, true, classNames);

    //
    // Get all instances for this class and all its descendent classes
    //

    Array<CIMInstance> namedInstances;

    for (Uint32 i = 0; i < classNames.size(); i++)
    {
        Array<CIMInstance> localNamedInstances =
            enumerateInstancesForClass(nameSpace, classNames[i],
                false, includeQualifiers, includeClassOrigin, propertyList);

        // ATTN: Handles everything but deepInheritance.
        for (Uint32 j = 0 ; j < localNamedInstances.size(); j++)
        {
            _filterInstance(localNamedInstances[j],
                propertyList,
                localOnly,
                includeQualifiers,
                includeClassOrigin);
        }
        namedInstances.appendArray(localNamedInstances);
    }

    PEG_METHOD_EXIT();
    return namedInstances;
}

Array<CIMInstance> CIMRepository::enumerateInstancesForClass(
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY,
        "CIMRepository::enumerateInstancesForClass");

    ReadLock lock(_rep->_lock);

    _rep->_nameSpaceManager->validateClass(nameSpace, className);

    //
    // Get all instances for this class
    //

    Array<CIMInstance> namedInstances =
        _rep->_persistentStore->enumerateInstancesForClass(
            nameSpace, className);

    // Do any required filtering of properties, qualifiers, classorigin
    // on the returned instances.
    for (Uint32 i = 0 ; i < namedInstances.size(); i++)
    {
        Resolver::resolveInstance(
            namedInstances[i], _rep->_context, nameSpace, true);

        _filterInstance(namedInstances[i],
            propertyList,
            localOnly,
            includeQualifiers,
            includeClassOrigin);
    }

    PEG_METHOD_EXIT();
    return namedInstances;
}

Array<CIMObjectPath> CIMRepository::enumerateInstanceNamesForSubtree(
    const CIMNamespaceName& nameSpace,
    const CIMName& className)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY,
        "CIMRepository::enumerateInstanceNamesForSubtree");

    // It is not necessary to control access to the ReadWriteSem lock here.
    // This method calls enumerateInstanceNamesForClass, which does its own
    // access control.

    //
    // Get names of descendent classes:
    //

    Array<CIMName> classNames;
    classNames.append(className);
    _rep->_nameSpaceManager->getSubClassNames(
        nameSpace, className, true, classNames);

    //
    // Enumerate instance names for each of the subclasses
    //
    Array<CIMObjectPath> instanceNames;

    for (Uint32 i = 0; i < classNames.size(); i++)
    {
        instanceNames.appendArray(
            enumerateInstanceNamesForClass(nameSpace, classNames[i]));
    }

    PEG_METHOD_EXIT();
    return instanceNames;
}

Array<CIMObjectPath> CIMRepository::enumerateInstanceNamesForClass(
    const CIMNamespaceName& nameSpace,
    const CIMName& className)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY,
        "CIMRepository::enumerateInstanceNamesForClass");

    ReadLock lock(_rep->_lock);

    _rep->_nameSpaceManager->validateClass(nameSpace, className);

    Array<CIMObjectPath> instanceNames =
        _rep->_persistentStore->enumerateInstanceNamesForClass(
            nameSpace, className);

    PEG_METHOD_EXIT();
    return instanceNames;
}


Array<CIMObject> CIMRepository::associators(
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
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::associators");

    ReadLock lock(_rep->_lock);

    Array<CIMObjectPath> names = _associatorNames(
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

            CIMClass cimClass = _getClass(
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

            CIMInstance cimInstance = _getInstance(
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
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& objectName,
    const CIMName& assocClass,
    const CIMName& resultClass,
    const String& role,
    const String& resultRole)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::associatorNames");

    ReadLock lock(_rep->_lock);
    Array<CIMObjectPath> result = _associatorNames(
        nameSpace, objectName, assocClass, resultClass, role, resultRole);

    PEG_METHOD_EXIT();
    return result;
}

Array<CIMObjectPath> CIMRepository::_associatorNames(
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& objectName,
    const CIMName& assocClass,
    const CIMName& resultClass,
    const String& role,
    const String& resultRole)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::_associatorNames");

    Array<String> associatorNames;

    // The assocClass parameter implies subclasses, so retrieve them
    Array<CIMName> assocClassList;
    if (!assocClass.isNull())
    {
        _rep->_nameSpaceManager->getSubClassNames(
            nameSpace, assocClass, true, assocClassList);
        assocClassList.append(assocClass);
    }

    // The resultClass parameter implies subclasses, so retrieve them
    Array<CIMName> resultClassList;
    if (!resultClass.isNull())
    {
        _rep->_nameSpaceManager->getSubClassNames(
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
        _rep->_nameSpaceManager->getSuperClassNames(
            nameSpace, className, classList);
        classList.append(className);

        Array<CIMNamespaceName> nameSpaceList =
            _rep->_nameSpaceManager->getSchemaNameSpaceNames(nameSpace);

        for (Uint32 i = 0; i < nameSpaceList.size(); i++)
        {
            Array<String> associatorNamesForNameSpace;

            _rep->_persistentStore->getClassAssociatorNames(
                nameSpaceList[i],
                classList,
                assocClassList,
                resultClassList,
                role,
                resultRole,
                associatorNamesForNameSpace);

            associatorNames.appendArray(associatorNamesForNameSpace);
        }
    }
    else
    {
        _rep->_nameSpaceManager->validateClass(
            nameSpace, objectName.getClassName());

        _rep->_persistentStore->getInstanceAssociatorNames(
            nameSpace,
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

Array<CIMObject> CIMRepository::references(
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& objectName,
    const CIMName& resultClass,
    const String& role,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::references");

    ReadLock lock(_rep->_lock);

    Array<CIMObjectPath> names = _referenceNames(
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
            CIMClass cimClass = _getClass(
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
            CIMInstance instance = _getInstance(
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
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& objectName,
    const CIMName& resultClass,
    const String& role)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::referenceNames");

    ReadLock lock(_rep->_lock);
    Array<CIMObjectPath> result = _referenceNames(
        nameSpace, objectName, resultClass, role);

    PEG_METHOD_EXIT();
    return result;
}

Array<CIMObjectPath> CIMRepository::_referenceNames(
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& objectName,
    const CIMName& resultClass,
    const String& role)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::_referenceNames");

    Array<String> tmpReferenceNames;

    // The resultClass parameter implies subclasses, so retrieve them
    Array<CIMName> resultClassList;

    try
    {
        if (!resultClass.isNull())
        {
            _rep->_nameSpaceManager->getSubClassNames(
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
            _rep->_nameSpaceManager->getSuperClassNames(
                nameSpace, className, classList);
            classList.append(className);

            Array<CIMNamespaceName> nameSpaceList =
                _rep->_nameSpaceManager->getSchemaNameSpaceNames(nameSpace);

            for (Uint32 i = 0; i < nameSpaceList.size(); i++)
            {
                Array<String> referenceNamesForNameSpace;

                _rep->_persistentStore->getClassReferenceNames(
                    nameSpaceList[i],
                    classList,
                    resultClassList,
                    role,
                    referenceNamesForNameSpace);

                tmpReferenceNames.appendArray(referenceNamesForNameSpace);
            }
        }
        else
        {
            _rep->_nameSpaceManager->validateClass(
                nameSpace, objectName.getClassName());

            _rep->_persistentStore->getInstanceReferenceNames(
                nameSpace,
                objectName,
                resultClassList,
                role,
                tmpReferenceNames);
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

CIMValue CIMRepository::getProperty(
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& instanceName,
    const CIMName& propertyName)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::getProperty");

    ReadLock lock(_rep->_lock);

    //
    // Retrieve the specified instance
    //

    CIMInstance cimInstance = _getInstance(
        nameSpace, instanceName, false, true, true, CIMPropertyList());

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

void CIMRepository::setProperty(
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& instanceName,
    const CIMName& propertyName,
    const CIMValue& newValue,
    const ContentLanguageList& contentLangs)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::setProperty");

    // It is not necessary to control access to the ReadWriteSem lock here.
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
    modifyInstance(nameSpace, instance, false, propertyList);

    PEG_METHOD_EXIT();
}

CIMQualifierDecl CIMRepository::getQualifier(
    const CIMNamespaceName& nameSpace,
    const CIMName& qualifierName)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::getQualifier");

    ReadLock lock(_rep->_lock);
    CIMQualifierDecl qualifierDecl = _getQualifier(nameSpace, qualifierName);

    PEG_METHOD_EXIT();
    return qualifierDecl;
}

CIMQualifierDecl CIMRepository::_getQualifier(
    const CIMNamespaceName& nameSpace,
    const CIMName& qualifierName)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::_getQualifier");

    CIMQualifierDecl qualifierDecl;

    String qualifierCacheKey = _getCacheKey(nameSpace, qualifierName);

    // Check the cache first:

    if (!_qualifierCache.get(qualifierCacheKey, qualifierDecl))
    {
        // Not in cache so load from disk:

        Array<CIMNamespaceName> nameSpaceList =
            _rep->_nameSpaceManager->getSchemaNameSpaceNames(nameSpace);

        for (Uint32 i = 0; i < nameSpaceList.size(); i++)
        {
            qualifierDecl = _rep->_persistentStore->getQualifier(
                nameSpaceList[i], qualifierName);

            if (!qualifierDecl.isUninitialized())
            {
                // Put in cache
                _qualifierCache.put(qualifierCacheKey, qualifierDecl);

                PEG_METHOD_EXIT();
                return qualifierDecl;
            }
        }

        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(
            CIM_ERR_NOT_FOUND, qualifierName.getString());
    }

    PEG_METHOD_EXIT();
    return qualifierDecl;
}

void CIMRepository::setQualifier(
    const CIMNamespaceName& nameSpace,
    const CIMQualifierDecl& qualifierDecl,
    const ContentLanguageList& contentLangs)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::setQualifier");

    WriteLock lock(_rep->_lock);
    AutoFileLock fileLock(_rep->_lockFile);
    _setQualifier(nameSpace, qualifierDecl);

    PEG_METHOD_EXIT();
}

void CIMRepository::_setQualifier(
    const CIMNamespaceName& nameSpace,
    const CIMQualifierDecl& qualifierDecl)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::_setQualifier");

    _rep->_nameSpaceManager->checkSetOrDeleteQualifier(
        nameSpace, qualifierDecl.getName());

    _rep->_persistentStore->setQualifier(nameSpace, qualifierDecl);

    String qualifierCacheKey =
        _getCacheKey(nameSpace, qualifierDecl.getName());
    _qualifierCache.put(qualifierCacheKey, (CIMQualifierDecl&)qualifierDecl);

    PEG_METHOD_EXIT();
}

void CIMRepository::deleteQualifier(
    const CIMNamespaceName& nameSpace,
    const CIMName& qualifierName)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::deleteQualifier");

    WriteLock lock(_rep->_lock);
    AutoFileLock fileLock(_rep->_lockFile);

    _rep->_nameSpaceManager->checkSetOrDeleteQualifier(
        nameSpace, qualifierName);

    _rep->_persistentStore->deleteQualifier(nameSpace, qualifierName);

    String qualifierCacheKey = _getCacheKey(nameSpace, qualifierName);
    _qualifierCache.evict(qualifierCacheKey);

    PEG_METHOD_EXIT();
}

Array<CIMQualifierDecl> CIMRepository::enumerateQualifiers(
    const CIMNamespaceName& nameSpace)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::enumerateQualifiers");

    ReadLock lock(_rep->_lock);

    Array<CIMQualifierDecl> qualifiers;

    _rep->_nameSpaceManager->validateNameSpace(nameSpace);

    qualifiers = _rep->_persistentStore->enumerateQualifiers(nameSpace);

    PEG_METHOD_EXIT();
    return qualifiers;
}

void CIMRepository::createNameSpace(
    const CIMNamespaceName& nameSpace,
    const NameSpaceAttributes& attributes)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::createNameSpace");

    WriteLock lock(_rep->_lock);
    AutoFileLock fileLock(_rep->_lockFile);

    Boolean shareable = false;
    Boolean updatesAllowed = true;
    String parentNameSpace;

    for (NameSpaceAttributes::Iterator i = attributes.start(); i; i++)
    {
        String key = i.key();
        if (String::equalNoCase(key, "shareable"))
        {
            if (String::equalNoCase(i.value(), "true"))
                shareable = true;
        }
        else if (String::equalNoCase(key, "updatesAllowed"))
        {
            if (String::equalNoCase(i.value(), "false"))
                updatesAllowed = false;
        }
        else if (String::equalNoCase(key, "parent"))
        {
            parentNameSpace = i.value();
        }
        else
        {
            PEG_METHOD_EXIT();
            throw PEGASUS_CIM_EXCEPTION(
                CIM_ERR_NOT_SUPPORTED,
                nameSpace.getString() + " option not supported: " + key);
        }
    }

    _rep->_nameSpaceManager->createNameSpace(
        nameSpace, shareable, updatesAllowed, parentNameSpace);

    try
    {
        _rep->_persistentStore->createNameSpace(
            nameSpace, shareable, updatesAllowed, parentNameSpace);
    }
    catch (...)
    {
        _rep->_nameSpaceManager->deleteNameSpace(nameSpace);
        throw;
    }

    PEG_METHOD_EXIT();
}

void CIMRepository::modifyNameSpace(
    const CIMNamespaceName& nameSpace,
    const NameSpaceAttributes& attributes)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::modifyNameSpace");

    WriteLock lock(_rep->_lock);
    AutoFileLock fileLock(_rep->_lockFile);

    Boolean shareable = false;
    Boolean updatesAllowed = true;

    for (NameSpaceAttributes::Iterator i = attributes.start(); i; i++)
    {
        String key = i.key();
        if (String::equalNoCase(key, "shareable"))
        {
            if (String::equalNoCase(i.value(), "true"))
                shareable = true;
        }
        else if (String::equalNoCase(key, "updatesAllowed"))
        {
            if (String::equalNoCase(i.value(),"false"))
                updatesAllowed = false;
        }
        else
        {
            PEG_METHOD_EXIT();
            throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED,
                nameSpace.getString() + " option not supported: " + key);
        }
    }

    _rep->_nameSpaceManager->validateNameSpace(nameSpace);

    if (!shareable)
    {
        // Check for dependent namespaces

        CIMNamespaceName dependentNameSpaceName;

        if (_rep->_nameSpaceManager->hasDependentNameSpace(
                nameSpace, dependentNameSpaceName))
        {
            PEG_METHOD_EXIT();
            throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
                "Namespace " + nameSpace.getString() +
                    " has dependent namespace " +
                    dependentNameSpaceName.getString());
        }
    }

    _rep->_persistentStore->modifyNameSpace(
        nameSpace, shareable, updatesAllowed);

    _rep->_nameSpaceManager->modifyNameSpace(
        nameSpace, shareable, updatesAllowed);

    PEG_METHOD_EXIT();
}

Array<CIMNamespaceName> CIMRepository::enumerateNameSpaces() const
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::enumerateNameSpaces");

    ReadLock lock(const_cast<ReadWriteSem&>(_rep->_lock));

    Array<CIMNamespaceName> nameSpaceNames;
    _rep->_nameSpaceManager->getNameSpaceNames(nameSpaceNames);

    PEG_METHOD_EXIT();
    return nameSpaceNames;
}

void CIMRepository::deleteNameSpace(const CIMNamespaceName& nameSpace)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::deleteNameSpace");

    WriteLock lock(_rep->_lock);
    AutoFileLock fileLock(_rep->_lockFile);

    // Check for dependent namespaces

    CIMNamespaceName dependentNameSpaceName;

    if (_rep->_nameSpaceManager->hasDependentNameSpace(
            nameSpace, dependentNameSpaceName))
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
            "Namespace " + nameSpace.getString() +
                " has dependent namespace " +
                dependentNameSpaceName.getString());
    }

    // Make sure the namespace is empty

    if (!_rep->_persistentStore->isNameSpaceEmpty(nameSpace))
    {
        PEG_METHOD_EXIT();
        throw NonEmptyNameSpace(nameSpace.getString());
    }

    _rep->_persistentStore->deleteNameSpace(nameSpace);

    _rep->_nameSpaceManager->deleteNameSpace(nameSpace);

    PEG_METHOD_EXIT();
}

Boolean CIMRepository::getNameSpaceAttributes(const CIMNamespaceName& nameSpace,
        NameSpaceAttributes& attributes)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::deleteNameSpace");

    ReadLock lock(const_cast<ReadWriteSem&>(_rep->_lock));
    attributes.clear();

    Boolean shareable;
    Boolean updatesAllowed;
    String parent;

    if (!_rep->_nameSpaceManager->getNameSpaceAttributes(
        nameSpace, shareable, updatesAllowed, parent))
    {
        PEG_METHOD_EXIT();
        return false;
    }

    attributes.insert("name", nameSpace.getString());

    if (shareable)
        attributes.insert("shareable", "true");
    else
        attributes.insert("shareable", "false");

    if (updatesAllowed)
        attributes.insert("updatesAllowed", "true");
    else
        attributes.insert("updatesAllowed", "false");

    if (parent.size())
        attributes.insert("parent", parent);

    PEG_METHOD_EXIT();
    return true;
}

Boolean CIMRepository::isRemoteNameSpace(
    const CIMNamespaceName& nameSpaceName,
    String& remoteInfo)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::isRemoteNamespace");
    ReadLock lock(const_cast<ReadWriteSem&>(_rep->_lock));
    PEG_METHOD_EXIT();
    return _rep->_nameSpaceManager->isRemoteNameSpace(
        nameSpaceName, remoteInfo);
}

#ifdef PEGASUS_DEBUG
    void CIMRepository::DisplayCacheStatistics()
    {
#ifdef PEGASUS_USE_CLASS_CACHE
        cout << "Repository Class Cache Statistics:" << endl;
        _classCache.DisplayCacheStatistics();
#endif
        cout << "Repository Qualifier Cache Statistics:" << endl;
        _qualifierCache.DisplayCacheStatistics();
    }
#endif

void CIMRepository::getSubClassNames(
    const CIMNamespaceName& nameSpaceName,
    const CIMName& className,
    Boolean deepInheritance,
    Array<CIMName>& subClassNames) const
{
    ReadLock lock(const_cast<ReadWriteSem&>(_rep->_lock));
    _rep->_nameSpaceManager->getSubClassNames(
        nameSpaceName, className, deepInheritance, subClassNames);
}

void CIMRepository::getSuperClassNames(
    const CIMNamespaceName& nameSpaceName,
    const CIMName& className,
    Array<CIMName>& subClassNames) const
{
    ReadLock lock(const_cast<ReadWriteSem&>(_rep->_lock));
    _rep->_nameSpaceManager->getSuperClassNames(
        nameSpaceName, className, subClassNames);
}

Boolean CIMRepository::isDefaultInstanceProvider()
{
    return _rep->_isDefaultInstanceProvider;
}

PEGASUS_NAMESPACE_END
