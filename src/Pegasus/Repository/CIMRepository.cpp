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
//              Karl Schopmeyer(k.schopmeyer@opengroup.org) - extend ref function.
//              Robert Kieninger, IBM (kieningr@de.ibm.com) - Bugzilla 383,1508,1813,667
//              Seema Gupta (gseema@in.ibm.com) - Bugzilla 281, Bugzilla 1313
//              Adrian Schuur (schuur@de.ibm.com) - PEP 129 & 164
//              Amit K Arora, IBM (amita@in.ibm.com) for PEP#101
//              Dave Sudlik, IBM (dsudlik@us.ibm.com)
//              David Dillard, VERITAS Software Corp.
//                   (david.dillard@veritas.com)
//              Josephine Eskaline Joyce, IBM (jojustin@in.ibm.com) for Bug#3347
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
#include <Pegasus/Common/MessageLoader.h> //l10n
#include <Pegasus/Common/CommonUTF.h>

#include <Pegasus/Common/XmlStreamer.h>
#include <Pegasus/Common/BinaryStreamer.h>
#include <Pegasus/Common/AutoStreamer.h>

#include "CIMRepository.h"
#include "RepositoryDeclContext.h"
#include "InstanceIndexFile.h"
#include "InstanceDataFile.h"
#include "AssocInstTable.h"
#include "AssocClassTable.h"

#ifdef PEGASUS_ENABLE_COMPRESSED_REPOSITORY
// #define win32
# include <zlib.h>
# include <sstream>
#endif

#if  defined(PEGASUS_OS_OS400)
#include "OS400ConvertChar.h"
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
static int binaryMode = -1; // PEP 164

#ifdef PEGASUS_ENABLE_COMPRESSED_REPOSITORY
static int compressMode = 0; // PEP214
#endif

// #define TEST_OUTPUT

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

void _LoadFileToMemory(Array<char>& data, const String& path)
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
Boolean _containsProperty(CIMProperty& property, const CIMPropertyList& propertyList)
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
   NOTE: This would be logical to be moved to CIMClass since it may be more general
   than this usage.
*/
void _removeAllQualifiers(CIMClass & cimClass)
{
    // remove qualifiers of the class
    Uint32 count = 0;
    while((count = cimClass.getQualifierCount()) > 0)
        cimClass.removeQualifier(count - 1);

    // remove qualifiers from the properties
    for (Uint32 i = 0; i < cimClass.getPropertyCount(); i++)
    {
        CIMProperty p = cimClass.getProperty(i);
        count=0;
        while((count = p.getQualifierCount()) > 0)
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
        count=0;
        while((count = m.getQualifierCount()) > 0)
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
   NOTE: This could be logical to be moved to CIMClass since it may be more general
   than the usage here.
*/
void _removePropagatedQualifiers(CIMClass & cimClass)
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
        for (Sint32 j = (count - 1); j > 0; j--)
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
            for (Sint32 k = (count - 1); k > 0; k--)
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
        for (Sint32 j = (count - 1); j > 0; j--)
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
void _removeProperties(CIMInstance& cimInstance,
    const CIMPropertyList& propertyList, Boolean localOnly)
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

            // if localOnly==true, ignore properties defined in super class
            if (localOnly && (p.getPropagated()))
            {
                cimInstance.removeProperty(i);
                continue;
            }

            // propertyList NULL means deliver properties.  PropertyList empty, none.
            // Test for removal if propertyList not NULL. The empty list option
            // is covered by fact that property is not in the list.
            if (!propertyListNull)
                if(!_containsProperty(p, propertyList))
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
void _removeAllQualifiers(CIMInstance& cimInstance)
{
        // remove qualifiers from the instance
        Uint32 count = 0;
        while((count = cimInstance.getQualifierCount()) > 0)
            cimInstance.removeQualifier(count - 1);

        // remove qualifiers from the properties
        for (Uint32 i = 0; i < cimInstance.getPropertyCount(); i++)
        {
            CIMProperty p = cimInstance.getProperty(i);
            count=0;
            while((count = p.getQualifierCount()) > 0)
                p.removeQualifier(count - 1);
        }
}
/* removes all ClassOrigin attributes from a single CIMInstance. Removes
    the classOrigin attribute from each property in the Instance.
   @param Instance from which the ClassOrigin Properties will be removed.
   NOTE: Logical to be moved to CIMInstance since it may be more general
   than just the repositoryl
*/
void _removeClassOrigins(CIMInstance& cimInstance)
{
        PEG_TRACE_STRING(TRC_REPOSITORY, Tracer::LEVEL4, "Remove Class Origins");

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
void _filterInstance(CIMInstance& cimInstance,
                const CIMPropertyList& propertyList,
                Boolean localOnly,
                Boolean includeQualifiers,
                Boolean includeClassOrigin)
{
    // Remove properties based on propertylist and localOnly flag
#ifdef PEGASUS_ENABLE_REPOSITORY_INSTANCE_FILTER
    _removeProperties(cimInstance, propertyList, localOnly);
#endif
    // If includequalifiers false, remove all qualifiers from
    // properties.

    if(!includeQualifiers)
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
    ObjectStreamer *streamer)
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

    Array<char> data;

    _LoadFileToMemory(data, realPath);    // PEP214

    data.append('\0');

    streamer->decode(data, 0, object);

    //XmlParser parser((char*)data.getData());

    //XmlReader::getObject(parser, object);

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

void _SaveObject(const String& path, Array<char>& objectXml,
    ObjectStreamer *streamer)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::_SaveObject");

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
////////////////////////////////////////////////////////////////////////////////

CIMRepository::CIMRepository(const String& repositoryRoot, const CIMRepository_Mode mode)
   : _repositoryRoot(repositoryRoot), _nameSpaceManager(repositoryRoot),
     _lock(), _resolveInstance(true)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::CIMRepository");

    binaryMode = mode.flag & CIMRepository_Mode::BIN;

#ifdef PEGASUS_ENABLE_COMPRESSED_REPOSITORY    // PEP214
    // FUTURE?? -  compressMode = mode.flag & CIMRepository_Mode::COMPRESSED;
    compressMode=1;

    char *s = getenv("PEGASUS_ENABLE_COMPRESSED_REPOSITORY");
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
    cout << "CIMRepository: binaryMode="  << binaryMode << "mode.flag=" << mode.flag << "\n";
    cout << "CIMRepository: compressMode= " << compressMode << endl;
#endif /* TEST_OUTPUT */

    if (binaryMode>0) { // PEP 164
      // BUILD BINARY
       streamer=new AutoStreamer(new BinaryStreamer(),BINREP_MARKER);
       ((AutoStreamer*)streamer)->addReader(new XmlStreamer(),0);
    }
    else { // streamer=new XmlStreamer();
      // BUILD XML
       streamer=new AutoStreamer(new XmlStreamer(),0xff);
       ((AutoStreamer*)streamer)->addReader(new BinaryStreamer(),BINREP_MARKER);
       ((AutoStreamer*)streamer)->addReader(new XmlStreamer(),0);
    }

    _context = new RepositoryDeclContext(this);
    _isDefaultInstanceProvider = (ConfigManager::getInstance()->getCurrentValue(
        "repositoryIsDefaultInstanceProvider") == "true");

    PEG_METHOD_EXIT();
}

CIMRepository::CIMRepository(const String& repositoryRoot)
   : _repositoryRoot(repositoryRoot), _nameSpaceManager(repositoryRoot),
     _lock(), _resolveInstance(true)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::CIMRepository");

    if (binaryMode==-1) { // PEP 164
       binaryMode = (ConfigManager::getInstance()->getCurrentValue(
        "enableBinaryRepository") == "true");
    }


#ifdef PEGASUS_ENABLE_COMPRESSED_REPOSITORY    // PEP214
    // FUTURE?? -  compressMode = mode.flag & CIMRepository_Mode::COMPRESSED;
    compressMode=1;

    char *s = getenv("PEGASUS_ENABLE_COMPRESSED_REPOSITORY");
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
    cout << "CIMRepository: binaryMode="  << binaryMode << endl;
    cout << "CIMRepository: compressMode= " << compressMode << endl;
#endif /* TEST_OUTPUT */

    if (binaryMode>0) { // PEP 164
      // BUILD BINARY
       streamer=new AutoStreamer(new BinaryStreamer(),BINREP_MARKER);
       ((AutoStreamer*)streamer)->addReader(new XmlStreamer(),0);
    }
    else { // streamer=new XmlStreamer();
      // BUILD XML
       streamer=new AutoStreamer(new XmlStreamer(),0xff);
       ((AutoStreamer*)streamer)->addReader(new BinaryStreamer(),BINREP_MARKER);
       ((AutoStreamer*)streamer)->addReader(new XmlStreamer(),0);
    }

    _context = new RepositoryDeclContext(this);
    _isDefaultInstanceProvider = (ConfigManager::getInstance()->getCurrentValue(
        "repositoryIsDefaultInstanceProvider") == "true");

    PEG_METHOD_EXIT();
}


CIMRepository::~CIMRepository()
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::~CIMRepository");

    delete streamer;
    delete _context;

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

    ReadLock lock(_lock);
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
    String classFilePath;
    classFilePath = _nameSpaceManager.getClassFilePath(nameSpace, className, NameSpaceRead);

    CIMClass cimClass;

    try
    {
        _LoadObject(classFilePath, cimClass, streamer);
    }
    catch (Exception& e)
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_FOUND, className.getString());
    }

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

            // propertyList NULL means all properties.  PropertyList empty, none.
            // Test for removal if propertyList not NULL. The empty list option
            // is covered by fact that property is not in the list.
            if (!propertyListNull)
                if(!_containsProperty(p, propertyList))
                    cimClass.removeProperty(i);
        }
    }

    // remove methods based on localOnly flag
    if (localOnly)
    {
        Uint32 count = cimClass.getMethodCount();
        // Work backwards because removal may be cheaper.
        for (Sint32 i = (count - 1); i > 0; i--)
        {
            CIMMethod m = cimClass.getMethod(i);

            // if localOnly==true, ignore properties defined in super class
            if (localOnly && (m.getPropagated()))
                cimClass.removeMethod(i);
        }

    }
    // If includequalifiers false, remove all qualifiers from
    // properties, methods and parameters.
    if(!includeQualifiers)
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
        PEG_TRACE_STRING(TRC_REPOSITORY, Tracer::LEVEL4, "Remove Class Origins");

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

Boolean CIMRepository::_getInstanceIndex(
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& instanceName,
    CIMName& className,
    Uint32& index,
    Uint32& size,
    Boolean searchSuperClasses) const
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::_getInstanceIndex");

    //
    // Get all descendent classes of this class:
    //

    className = instanceName.getClassName();

    Array<CIMName> classNames;
    classNames.append(className);
    _nameSpaceManager.getSubClassNames(nameSpace, className, true, classNames);

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
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& instanceName,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::getInstance");

    ReadLock lock(_lock);
    CIMInstance cimInstance = _getInstance(nameSpace,
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

    //
    // Get the index for this instance:
    //

    CIMName className;
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

    _filterInstance(cimInstance, propertyList, localOnly, includeQualifiers, includeClassOrigin);

    PEG_METHOD_EXIT();
    return cimInstance;
}

void CIMRepository::deleteClass(
    const CIMNamespaceName& nameSpace,
    const CIMName& className)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY,"CIMRepository::deleteClass");

    WriteLock lock(_lock);

    //
    // Get the class and check to see if it is an association class:
    //

    CIMClass cimClass = _getClass(
        nameSpace, className, false, true, false, CIMPropertyList());
    Boolean isAssociation = cimClass.isAssociation();

    //
    // Delete the class. The NameSpaceManager::deleteClass() method throws
    // an exception if the class has subclasses.
    //
    try
    {
        _nameSpaceManager.deleteClass(nameSpace, className);
    }
    catch (const CIMException&)
    {
        PEG_METHOD_EXIT();
        throw;
    }

    //
    // Remove associations:
    //

    if (isAssociation)
    {
        Array<String> assocFileName = _nameSpaceManager.getAssocClassPath(nameSpace,NameSpaceDelete);

        if (FileSystem::exists(assocFileName[0]))
            AssocClassTable::deleteAssociation(assocFileName[0], className);
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
        //l10n
        //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "compact failed");
        throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
            MessageLoaderParms("Repository.CIMRepository.COMPACT_FAILED",
                "compact failed"));
        //l10n end
    }

    if (!InstanceDataFile::compact(dataFilePath, freeFlags, indices, sizes))
    {
        PEG_METHOD_EXIT();
        //l10n
        //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "compact failed");
        throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
            MessageLoaderParms("Repository.CIMRepository.COMPACT_FAILED",
                "compact failed"));
        //l10n end
    }

    //
    // Now compact the index file:
    //

    if (!InstanceIndexFile::compact(indexFilePath))
    {
        PEG_METHOD_EXIT();
        //l10n
        //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "compact failed");
        throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
            MessageLoaderParms("Repository.CIMRepository.COMPACT_FAILED",
                "compact failed"));
        //l10n end
    }

    PEG_METHOD_EXIT();
}

void CIMRepository::deleteInstance(
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& instanceName)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::deleteInstance");

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

    WriteLock lock(_lock);

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
        //l10n
        //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "rollback failed");
        throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
            MessageLoaderParms("Repository.CIMRepository.ROLLBACK_FAILED",
                "rollback failed"));
        //l10n end
    }

    if (!InstanceDataFile::rollbackTransaction(dataFilePath))
    {
        PEG_METHOD_EXIT();
        //l10n
        //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "rollback failed");
        throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
            MessageLoaderParms("Repository.CIMRepository.ROLLBACK_FAILED",
                "rollback failed"));
        //l10n end
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
        //l10n
        //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "begin failed");
        throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
            MessageLoaderParms("Repository.CIMRepository.BEGIN_FAILED",
                "begin failed"));
        //l10n end
    }

    if (!InstanceDataFile::beginTransaction(dataFilePath))
    {
        PEG_METHOD_EXIT();
        //l10n
        //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "begin failed");
        throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
            MessageLoaderParms("Repository.CIMRepository.BEGIN_FAILED",
                "begin failed"));
        //l10n end
    }

    //
    // Remove entry from index file.
    //

    Uint32 freeCount;

    if (!InstanceIndexFile::deleteEntry(indexFilePath, instanceName, freeCount))
    {
        //l10n
        //errMessage.append("Failed to delete instance: ");
        //errMessage.append(instanceName.toString());
        PEG_METHOD_EXIT();
        //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, errMessage);
        throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
            MessageLoaderParms(
                "Repository.CIMRepository.FAILED_TO_DELETE_INSTANCE",
                "Failed to delete instance: $0",
                instanceName.toString()));
        //l10n end
    }

    //
    // Commit the transaction:
    //

    if (!InstanceIndexFile::commitTransaction(indexFilePath))
    {
        PEG_METHOD_EXIT();
        //l10n
        //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "commit failed");
        throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
            MessageLoaderParms("Repository.CIMRepository.COMMIT_FAILED",
                "commit failed"));
        //l10n end
    }

    if (!InstanceDataFile::commitTransaction(dataFilePath))
    {
        PEG_METHOD_EXIT();
        //l10n
        //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "commit failed");
        throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
            MessageLoaderParms("Repository.CIMRepository.COMMIT_FAILED",
                "commit failed"));
        //l10n end
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

    String assocFileName = _nameSpaceManager.getAssocInstPath(nameSpace);

    if (FileSystem::exists(assocFileName))
        AssocInstTable::deleteAssociation(assocFileName, instanceName);

    PEG_METHOD_EXIT();
}

void CIMRepository::_createAssocClassEntries(
    const CIMNamespaceName& nameSpace,
    const CIMConstClass& assocClass)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::_createAssocClassEntries");

    // Open input file:


    Array<String> assocFileName = _nameSpaceManager.getAssocClassPath(nameSpace,NameSpaceWrite);
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
    const CIMNamespaceName& nameSpace,
    const CIMClass& newClass,
    const ContentLanguages& contentLangs)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::createClass");

    WriteLock lock(_lock);
    _createClass(nameSpace, newClass);

    PEG_METHOD_EXIT();
}

void CIMRepository::_createClass(
    const CIMNamespaceName& nameSpace,
    const CIMClass& newClass)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::_createClass");

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

    Array<char> classXml;
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


/*------------------------------------------------------------------------------
  Checks whether a hostname points to the local CIMOM
------------------------------------------------------------------------------*/
static Boolean isLocalCIMOM( const String& hostname )
{
   PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::isLocalCIMOM");

   // First do a brute force check for equality of the strings
   if( String::equalNoCase(hostname,System::getHostName()))
   {
      PEG_METHOD_EXIT();
      return true;
   }

   // ------------------------------------------------------------------------
   // Strip of the port numbers
   // ------------------------------------------------------------------------
   String hn = hostname;

   Uint32 index = hostname.find(':');
   if ( index != PEG_NOT_FOUND )
   {
      hn = hostname.subString(0,index);
   }

   // ------------------------------------------------------------------------
   // Normalize hostname into a a single host unique integer representation
   // and compare against the equivalent for the local CIMOM, if not localhost.
   // ------------------------------------------------------------------------
   Uint32 hostIP = System::_acquireIP((const char*) hn.getCString());

   // Check if localhost or 127.0.0.x
   if (hostIP == 0x7F000001)
   {
      PEG_METHOD_EXIT();
      return true;
   }

   Uint32 localHostIP = System::_acquireIP((const char*) System::getHostName().getCString());

   PEG_METHOD_EXIT();
   return hostIP == localHostIP;
}


void CIMRepository::_createAssocInstEntries(
    const CIMNamespaceName& nameSpace,
    const CIMConstClass& cimClass,
    const CIMInstance& cimInstance,
    const CIMObjectPath& instanceName)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::_createAssocInstEntries");

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
                    // Strip off the hostname if it is the same as the local host
                    if ((fromRef.getHost() != String::EMPTY) &&
                        (isLocalCIMOM(fromRef.getHost())))
                    {
                       PEG_TRACE_STRING(TRC_REPOSITORY, Tracer::LEVEL4, "_createAssocInstEntries() - Stripping of local hostName from fromRef");
                       fromRef.setHost(String::EMPTY);
                    }

                    // Strip off the namespace when it is the same as the
                    // one this instance is created in.
                    if ((fromRef.getHost() == String::EMPTY) &&
                        (fromRef.getNameSpace() == nameSpace))
                    {
                       PEG_TRACE_STRING(TRC_REPOSITORY, Tracer::LEVEL4, "_createAssocInstEntries() - Stripping of local nameSpace from fromRef");
                       fromRef.setNameSpace(CIMNamespaceName());
                    }

                    // Strip off the hostname if it is the same as the local host
                    if ((toRef.getHost() != String::EMPTY) &&
                        (isLocalCIMOM(toRef.getHost())))
                    {
                       PEG_TRACE_STRING(TRC_REPOSITORY, Tracer::LEVEL4, "_createAssocInstEntries() - Stripping of local hostName from toRef");
                       toRef.setHost(String::EMPTY);
                    }

                    // Strip off the namespace when it is the same as the
                    // one this instance is created in.
                    if ((toRef.getHost() == String::EMPTY) &&
                        (toRef.getNameSpace() == nameSpace))
                    {
                       PEG_TRACE_STRING(TRC_REPOSITORY, Tracer::LEVEL4, "_createAssocInstEntries() - Stripping of local nameSpace from toRef");
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

CIMObjectPath CIMRepository::createInstance(
    const CIMNamespaceName& nameSpace,
    const CIMInstance& newInstance,
    const ContentLanguages& contentLangs)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::createInstance");

    WriteLock lock(_lock);
    CIMObjectPath instanceName = _createInstance(nameSpace, newInstance);

    PEG_METHOD_EXIT();
    return instanceName;
}

CIMObjectPath CIMRepository::_createInstance(
    const CIMNamespaceName& nameSpace,
    const CIMInstance& newInstance)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::_createInstance");

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
        //l10n
        //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "rollback failed");
        throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
            MessageLoaderParms("Repository.CIMRepository.ROLLBACK_FAILED",
                "rollback failed"));
        //l10n end
    }

    if (!InstanceDataFile::rollbackTransaction(dataFilePath))
    {
        PEG_METHOD_EXIT();
        //l10n
        //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "rollback failed");
        throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
            MessageLoaderParms(
                "Repository.CIMRepository.ROLLBACK_FAILED",
                "rollback failed"));
        //l10n end
    }

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
        //l10n
        //errMessage = "class has no keys: " +
            //cimClass.getClassName().getString();
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
            MessageLoaderParms("Repository.CIMRepository.CLASS_HAS_NO_KEYS",
                "class has no keys: $0",
                cimClass.getClassName().getString()));
        //l10n end
    }

    //
    // Be sure instance does not already exist:
    //

    CIMName className;
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
        //l10n
        //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "begin failed");
        throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
            MessageLoaderParms("Repository.CIMRepository.BEGIN_FAILED",
                "begin failed"));
        //l10n end
    }

    if (!InstanceDataFile::beginTransaction(dataFilePath))
    {
        PEG_METHOD_EXIT();
        //l10n
        //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "begin failed");
        throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
            MessageLoaderParms("Repository.CIMRepository.BEGIN_FAILED",
                "begin failed"));
        //l10n end
    }

    //
    // Save instance to file:
    //

    Uint32 index;
    Uint32 size;

    {
        Array<char> data;
        streamer->encode(data, cimInstance);
        // XmlWriter::appendInstanceElement(data, cimInstance);
        size = data.size();

        if (!InstanceDataFile::appendInstance(dataFilePath, data, index))
        {
            //l10n
            //errMessage.append("Failed to create instance: ");
            //errMessage.append(instanceName.toString());
            PEG_METHOD_EXIT();
            //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, errMessage);
            throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
                MessageLoaderParms(
                    "Repository.CIMRepository.FAILED_TO_CREATE_INSTANCE",
                    "Failed to create instance: $0",
                    instanceName.toString()));
            //l10n end
        }
    }

    //
    // Create entry in index file:
    //

    if (!InstanceIndexFile::createEntry(
            indexFilePath, instanceName, index, size))
    {
        //l10n
        //errMessage.append("Failed to create instance: ");
        //errMessage.append(instanceName.toString());
        PEG_METHOD_EXIT();
        //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, errMessage);
        throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
            MessageLoaderParms(
                "Repository.CIMRepository.FAILED_TO_CREATE_INSTANCE",
                "Failed to create instance: $0",
                instanceName.toString()));
        //l10n end
    }

    //
    // Commit the changes:
    //

    if (!InstanceIndexFile::commitTransaction(indexFilePath))
    {
        PEG_METHOD_EXIT();
        //l10n
        //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "commit failed");
        throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
            MessageLoaderParms("Repository.CIMRepository.COMMIT_FAILED",
                "commit failed"));
        //l10n end
    }

    if (!InstanceDataFile::commitTransaction(dataFilePath))
    {
        PEG_METHOD_EXIT();
        //l10n
        //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "commit failed");
        throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
            MessageLoaderParms("Repository.CIMRepository.COMMIT_FAILED",
                "commit failed"));
        //l10n end
    }

    Resolver::resolveInstance (cimInstance, _context, nameSpace, cimClass,
        true);

    PEG_METHOD_EXIT();
    return instanceName;
}

void CIMRepository::modifyClass(
    const CIMNamespaceName& nameSpace,
    const CIMClass& modifiedClass,
    const ContentLanguages& contentLangs)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::modifyClass");

    WriteLock lock(_lock);
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
        //l10n
        String str = "CIMRepository::modifyClass()";
        //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
            //"failed to remove file in CIMRepository::modifyClass()");
        throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
            MessageLoaderParms(
                "Repository.CIMRepository.FAILED_TO_REMOVE_FILE",
                "failed to remove file in $0", str));
        //l10n end
    }

    //
    // Create new class file:
    //

    Array<char> classXml;
    streamer->encode(classXml, cimClass);
    //XmlWriter::appendClassElement(classXml, cimClass);
    _SaveObject(classFilePath, classXml,streamer);

    if (cimClass.isAssociation()) {
      // Remove from Association
      Array<String> assocFileName =
         _nameSpaceManager.getAssocClassPath(nameSpace,NameSpaceDelete);
      if (FileSystem::exists(assocFileName[0])) {
        AssocClassTable::deleteAssociation(assocFileName[0], cimClass.getClassName());
        // Create the association again.
        _createAssocClassEntries(nameSpace, cimClass);
      }
      else
      {
          PEG_METHOD_EXIT();
          throw CannotOpenFile(assocFileName[0]);
      }
    }

    PEG_METHOD_EXIT();
}

void CIMRepository::modifyInstance(
    const CIMNamespaceName& nameSpace,
    const CIMInstance& modifiedInstance,
    Boolean includeQualifiers,
    const CIMPropertyList& propertyList,
    const ContentLanguages& contentLangs)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::modifyInstance");

    WriteLock lock(_lock);

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
        //l10n
        //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "rollback failed");
        throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
            MessageLoaderParms("Repository.CIMRepository.ROLLBACK_FAILED",
                "rollback failed"));
        //l10n end
    }

    if (!InstanceDataFile::rollbackTransaction(dataFilePath))
    {
        PEG_METHOD_EXIT();
        //l10n
        //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "rollback failed");
        throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
            MessageLoaderParms("Repository.CIMRepository.ROLLBACK_FAILED",
                "rollback failed"));
        //l10n end
    }

    //
    // Begin the transaction:
    //

    if (!InstanceIndexFile::beginTransaction(indexFilePath))
    {
        PEG_METHOD_EXIT();
        //l10n
        //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "begin failed");
        throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
            MessageLoaderParms("Repository.CIMRepository.BEGIN_FAILED",
                "begin failed"));
        //l10n end
    }

    if (!InstanceDataFile::beginTransaction(dataFilePath))
    {
        PEG_METHOD_EXIT();
        //l10n
        //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "begin failed");
        throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
            MessageLoaderParms("Repository.CIMRepository.BEGIN_FAILED",
                "begin failed"));
        //l10n end
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

            cimInstance = _getInstance(
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

        cimInstance = _getInstance(nameSpace,
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

    CIMObjectPath instanceName = cimInstance.buildPath(cimClass);

    //
    // Disallow operation if the instance name was changed:
    //

    // For for bugzilla 1508. Hostname and namespace are not included
    // in the comparison here.
    CIMObjectPath modifiedInstancePath = modifiedInstance.getPath();
    modifiedInstancePath.setNameSpace(CIMNamespaceName());
    modifiedInstancePath.setHost(String::EMPTY);
    if (instanceName != modifiedInstancePath)
    {
        PEG_METHOD_EXIT();
        //l10n
        //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
            //"Attempted to modify a key property");
        throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
            MessageLoaderParms(
                "Repository.CIMRepository.ATTEMPT_TO_MODIFY_KEY_PROPERTY",
                "Attempted to modify a key property"));
        //l10n end
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
        Array<char> out;
        streamer->encode(out, cimInstance);
        //XmlWriter::appendInstanceElement(out, cimInstance);

        newSize = out.size();

        if (!InstanceDataFile::appendInstance(dataFilePath, out, newIndex))
        {
            //l10n
            //errMessage.append("Failed to modify instance ");
            //errMessage.append(instanceName.toString());
            PEG_METHOD_EXIT();
            //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, errMessage);
            throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
                MessageLoaderParms(
                    "Repository.CIMRepository.FAILED_TO_MODIFY_INSTANCE",
                    "Failed to modify instance $0",
                    instanceName.toString()));
            //l10n end
        }
    }

    //
    // Modify the index file:
    //

    Uint32 freeCount;

    if (!InstanceIndexFile::modifyEntry(indexFilePath, instanceName, newIndex,
        newSize, freeCount))
    {
        //l10n
        //errMessage.append("Failed to modify instance ");
        //errMessage.append(instanceName.toString());
        PEG_METHOD_EXIT();
        //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, errMessage);
        throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
            MessageLoaderParms(
                "Repository.CIMRepository.FAILED_TO_MODIFY_INSTANCE",
                "Failed to modify instance $0",
                instanceName.toString()));
        //l10n end
    }

    //
    // Commit the transaction:
    //

    if (!InstanceIndexFile::commitTransaction(indexFilePath))
    {
        PEG_METHOD_EXIT();
        //l10n
        //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "commit failed");
        throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
            MessageLoaderParms("Repository.CIMRepository.COMMIT_FAILED",
                "commit failed"));
        //l10n end
    }

    if (!InstanceDataFile::commitTransaction(dataFilePath))
    {
        PEG_METHOD_EXIT();
        //l10n
        //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "commit failed");
        throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
            MessageLoaderParms("Repository.CIMRepository.COMMIT_FAILED",
                "commit failed"));
        //l10n end
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
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    Boolean deepInheritance,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::enumerateClasses");

    ReadLock lock(_lock);

    Array<CIMName> classNames;

    _nameSpaceManager.getSubClassNames(
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

    ReadLock lock(_lock);

    Array<CIMName> classNames;

    _nameSpaceManager.getSubClassNames(
        nameSpace, className, deepInheritance, classNames,true);

    PEG_METHOD_EXIT();
    return classNames;
}

Boolean CIMRepository::_loadAllInstances(
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    Array<CIMInstance>& namedInstances)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::_loadAllInstances");

    Array<CIMObjectPath> instanceNames;
    Array<char> data;
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
                Uint32 pos=(&(buffer[indices[i]]))-buffer;
                streamer->decode(data, pos, tmpInstance);

//                XmlParser parser(&(buffer[indices[i]]));
//
//                XmlReader::getObject(parser, tmpInstance);

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
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    Boolean deepInheritance,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::enumerateInstances");

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
                enumerateInstancesForClass(nameSpace, classNames[i],
                deepInheritance, localOnly,
                includeQualifiers, includeClassOrigin, false, propertyList);
        /* Code from before we changed to call once per class.
        if (!_loadAllInstances(nameSpace, classNames[i], localNamedInstances))
        {
            //l10n
            //String errMessage = "Failed to load instances in class ";
            //errMessage.append(classNames[i].getString ());
            PEG_METHOD_EXIT();
            //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, errMessage);
            throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
                MessageLoaderParms(
                    "Repository.CIMRepository.FAILED_TO_LOAD_INSTANCES",
                    "Failed to load instances in class $0",
                    classNames[i].getString()));
            //10n end
        }
        */
        // ATTN: Handles everything but deepInheritance.
        for (Uint32 i = 0 ; i < localNamedInstances.size(); i++)
        {
            _filterInstance(localNamedInstances[i],
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
    Boolean deepInheritance,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    Boolean includeInheritance,
    const CIMPropertyList& propertyList)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY,
                     "CIMRepository::enumerateInstancesForClass");

    ReadLock lock(_lock);

    //
    // Get all descendent classes of this class:
    //

    Array<CIMName> classNames;
    classNames.append(className);
    // If includeInheritance is true, get all subclasses.
    // ATTN: P3 KS Look at whether the subclassNames requires an empty array.
    if(includeInheritance)
    {
        try
        {
            _nameSpaceManager.getSubClassNames(nameSpace, className, true, classNames);
        }
        catch(const CIMException&)
        {
            PEG_METHOD_EXIT();
            throw;
        }
    }

    //
    // Get all instances for this class and all its descendent classes
    //

    Array<CIMInstance> namedInstances;

    for (Uint32 i = 0; i < classNames.size(); i++)
    {
        if (!_loadAllInstances(nameSpace, classNames[i], namedInstances))
        {
            //l10n
            //String errMessage = "Failed to load instances in class ";
            //errMessage.append(classNames[i].getString());
            PEG_METHOD_EXIT();
            //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, errMessage);
            throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
                MessageLoaderParms(
                    "Repository.CIMRepository.FAILED_TO_LOAD_INSTANCES",
                    "Failed to load instances in class $0",
                    classNames[i].getString()));
            //l10n end
        }
        // Do any required filtering of properties, qualifiers, classorigin
        // on the returned instances.
// Turn off this function for the moment since it changes client behavior
#ifdef PEGASUS_ENABLE_REPOSITORY_INSTANCE_FILTER
        for (Uint32 i = 0 ; i < namedInstances.size(); i++)
        {
            _filterInstance(namedInstances[i],
                propertyList,
                localOnly,
                includeQualifiers,
                includeClassOrigin);
        }
#endif
    }
    PEG_METHOD_EXIT();
    return namedInstances;
}

Array<CIMObjectPath> CIMRepository::enumerateInstanceNames(
    const CIMNamespaceName& nameSpace,
    const CIMName& className)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::enumerateInstanceNames");

    ReadLock lock(_lock);

    //
    // Get names of descendent classes:
    //
    Array<CIMName> classNames;

    classNames.append(className);

    try
    {
        _nameSpaceManager.getSubClassNames(nameSpace, className, true, classNames);
    }
    catch(const CIMException&)
    {
        PEG_METHOD_EXIT();
        throw;
    }

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
            //l10n
            //String errMessage = "Failed to load instance names in class ";
            //errMessage.append(classNames[i].getString());
            PEG_METHOD_EXIT();
            //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, errMessage);
            throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
                MessageLoaderParms(
                    "Repository.CIMRepository.FAILED_TO_LOAD_INSTANCE_NAMES",
                    "Failed to load instance names in class $0",
                    classNames[i].getString()));
            //l10n end
        }
    }

    PEG_METHOD_EXIT();
    return instanceNames;
}

Array<CIMObjectPath> CIMRepository::enumerateInstanceNamesForClass(
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    const Boolean includeInheritance)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY,
                     "CIMRepository::enumerateInstanceNamesForClass");

    ReadLock lock(_lock);

    //
    // Get names of descendent classes:
    //
    Array<CIMName> classNames;

    classNames.append(className);

    // If includeInheritance is true, get all subclasses.
    if(includeInheritance)
    {
        try
        {
            _nameSpaceManager.getSubClassNames(nameSpace, className, true, classNames);
        }
        catch(const CIMException&)
        {
            PEG_METHOD_EXIT();
            throw;
        }
    }

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
            //l10n
            //String errMessage = "Failed to load instance names in class ";
            //errMessage.append(classNames[i].getString());
            PEG_METHOD_EXIT();
            //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, errMessage);
            throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
                MessageLoaderParms(
                    "Repository.CIMRepository.FAILED_TO_LOAD_INSTANCE_NAMES",
                    "Failed to load instance names in class $0",
                    classNames[i].getString()));
            //l10n end
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

    ReadLock lock(_lock);

    PEG_METHOD_EXIT();
    throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, "execQuery()");

    PEGASUS_UNREACHABLE(PEG_METHOD_EXIT();
    return Array<CIMInstance>();)
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

    ReadLock lock(_lock);

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

    ReadLock lock(_lock);
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

        Array<String> assocFileName = _nameSpaceManager.getAssocClassPath(nameSpace,NameSpaceRead);

        for (int i=0,m=assocFileName.size(); i<m; i++) {
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

    else {
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

    ReadLock lock(_lock);

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

    ReadLock lock(_lock);
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

  try {
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
        _nameSpaceManager.getSuperClassNames(nameSpace, className, classList);
        classList.append(className);

        Array<String> assocFileName = _nameSpaceManager.getAssocClassPath(nameSpace,NameSpaceRead);

        Boolean refs=false;
        for (int i=0,m=assocFileName.size(); !refs && i<m; i++)
           if (AssocClassTable::getReferenceNames(
                  assocFileName[i],
            classList,
            resultClassList,
            role,
                  tmpReferenceNames)) refs|=true;

        if (refs==false) {
            // Ignore error! It's okay not to have references.
        }
    }

    else {
        String assocFileName = _nameSpaceManager.getAssocInstPath(nameSpace);

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

CIMValue CIMRepository::getProperty(
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& instanceName,
    const CIMName& propertyName)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::getProperty");

    ReadLock lock(_lock);

    //
    // Get the index for this instance:
    //

    CIMName className;
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
    if (pos == PEG_NOT_FOUND)
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
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& instanceName,
    const CIMName& propertyName,
    const CIMValue& newValue,
    const ContentLanguages& contentLangs)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::setProperty");

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
    modifyInstance(nameSpace, instance, false, propertyList);

    PEG_METHOD_EXIT();
}

CIMQualifierDecl CIMRepository::getQualifier(
    const CIMNamespaceName& nameSpace,
    const CIMName& qualifierName)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::getQualifier");

    ReadLock lock(_lock);
    CIMQualifierDecl qualifierDecl = _getQualifier(nameSpace, qualifierName);

    PEG_METHOD_EXIT();
    return qualifierDecl;
}

CIMQualifierDecl CIMRepository::_getQualifier(
    const CIMNamespaceName& nameSpace,
    const CIMName& qualifierName)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::_getQualifier");

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
        _LoadObject(qualifierFilePath, qualifierDecl, streamer);
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

void CIMRepository::setQualifier(
    const CIMNamespaceName& nameSpace,
    const CIMQualifierDecl& qualifierDecl,
    const ContentLanguages& contentLangs)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::setQualifier");

    WriteLock lock(_lock);
    _setQualifier(nameSpace, qualifierDecl);

    PEG_METHOD_EXIT();
}

void CIMRepository::_setQualifier(
    const CIMNamespaceName& nameSpace,
    const CIMQualifierDecl& qualifierDecl)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::_setQualifier");

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

    Array<char> qualifierDeclXml;
    streamer->encode(qualifierDeclXml, qualifierDecl);
     //XmlWriter::appendQualifierDeclElement(qualifierDeclXml, qualifierDecl);
    _SaveObject(qualifierFilePath, qualifierDeclXml,streamer);


    PEG_METHOD_EXIT();
}

void CIMRepository::deleteQualifier(
    const CIMNamespaceName& nameSpace,
    const CIMName& qualifierName)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::deleteQualifier");

    WriteLock lock(_lock);

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

    PEG_METHOD_EXIT();
}

Array<CIMQualifierDecl> CIMRepository::enumerateQualifiers(
    const CIMNamespaceName& nameSpace)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::enumerateQualifiers");

    ReadLock lock(_lock);

    String qualifiersRoot = _nameSpaceManager.getQualifiersRoot(nameSpace);

    Array<String> qualifierNames;

    if (!FileSystem::getDirectoryContents(qualifiersRoot, qualifierNames))
    {
        PEG_METHOD_EXIT();
        //l10n
        String str ="enumerateQualifiers()";
        //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
            //"enumerateQualifiers(): internal error");
        throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
            MessageLoaderParms("Repository.CIMRepository.INTERNAL_ERROR",
                "$0: internal error",
                str));
        //l10n end
    }

    Array<CIMQualifierDecl> qualifiers;

    for (Uint32 i = 0; i < qualifierNames.size(); i++)
    {
#ifdef PEGASUS_REPOSITORY_ESCAPE_UTF8
    // All chars above 0x7F will be escape.
      CIMQualifierDecl qualifier =
            _getQualifier(nameSpace, escapeStringDecoder(qualifierNames[i]));
#else
      CIMQualifierDecl qualifier =
            _getQualifier(nameSpace, qualifierNames[i]);
#endif
      qualifiers.append(qualifier);
    }

    PEG_METHOD_EXIT();
    return qualifiers;
}

void CIMRepository::createNameSpace(const CIMNamespaceName& nameSpace,
        const NameSpaceAttributes &attributes)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::createNameSpace");

    WriteLock lock(_lock);
    _nameSpaceManager.createNameSpace(nameSpace, attributes);

    PEG_METHOD_EXIT();
}

void CIMRepository::modifyNameSpace(const CIMNamespaceName& nameSpace,
        const NameSpaceAttributes &attributes)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::modifyNameSpace");

    WriteLock lock(_lock);
    _nameSpaceManager.modifyNameSpace(nameSpace, attributes);

    PEG_METHOD_EXIT();
}

Array<CIMNamespaceName> CIMRepository::enumerateNameSpaces() const
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::enumerateNameSpaces");

    ReadLock lock(const_cast<ReadWriteSem&>(_lock));

    Array<CIMNamespaceName> nameSpaceNames;
    _nameSpaceManager.getNameSpaceNames(nameSpaceNames);

    PEG_METHOD_EXIT();
    return nameSpaceNames;
}

void CIMRepository::deleteNameSpace(const CIMNamespaceName& nameSpace)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::deleteNameSpace");

    WriteLock lock(_lock);
    _nameSpaceManager.deleteNameSpace(nameSpace);

    PEG_METHOD_EXIT();
}

Boolean CIMRepository::getNameSpaceAttributes(const CIMNamespaceName& nameSpace,
        NameSpaceAttributes &attributes)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::deleteNameSpace");

    ReadLock lock(const_cast<ReadWriteSem&>(_lock));
    attributes.clear();
    PEG_METHOD_EXIT();
    return _nameSpaceManager.getNameSpaceAttributes(nameSpace, attributes);
}

Boolean CIMRepository::isRemoteNameSpace(const CIMNamespaceName& nameSpaceName,
        String & remoteInfo)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::isRemoteNamespace");
    ReadLock lock(const_cast<ReadWriteSem&>(_lock));
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

String CIMRepository::_getInstanceIndexFilePath(
    const CIMNamespaceName& nameSpace,
    const CIMName& className) const
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
    const CIMNamespaceName& nameSpace,
    const CIMName& className) const
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

    Array<char> data;

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

void CIMRepository::setDeclContext(RepositoryDeclContext *context)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "CIMRepository::setDeclContext");

    WriteLock lock(_lock);
    _context = context;

    PEG_METHOD_EXIT();
}

PEGASUS_NAMESPACE_END
