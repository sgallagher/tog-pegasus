//%/////////////////////////////////////////////////////////////////////////////
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
//==============================================================================
//
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By:
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
#include "CIMRepository.h"
#include "InstanceIndexFile.h"

// #define INDENT_XML_FILES

using namespace std;

PEGASUS_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
//
// _LoadObject()
//
////////////////////////////////////////////////////////////////////////////////

template<class Object>
void _LoadObject(
    const String& path,
    Object& object)
{
    // Get the real path of the file:

    String realPath;

    if (!FileSystem::existsIgnoreCase(path, realPath))
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
////////////////////////////////////////////////////////////////////////////////

template<class Object>
void _SaveObject(const String& path, Object& object)
{
    Array<Sint8> out;
    object.toXml(out);

    ArrayDestroyer<char> destroyer(path.allocateCString());
    std::ofstream os(destroyer.getPointer() PEGASUS_IOS_BINARY);

    if (!os)
	throw CannotOpenFile(path);

#ifdef INDENT_XML_FILES
    XmlWriter::indentedPrint(os, out.getData(), 2);
#else
    os.write((char*)out.getData(), out.size());
#endif
}

////////////////////////////////////////////////////////////////////////////////
//
// RepositoryDeclContext
//
////////////////////////////////////////////////////////////////////////////////

class RepositoryDeclContext : public DeclContext
{
public:

    RepositoryDeclContext(CIMRepository* repository);

    virtual ~RepositoryDeclContext();

    virtual CIMQualifierDecl lookupQualifierDecl(
	const String& nameSpace, 
	const String& qualifierName) const;

    virtual CIMClass lookupClass(
	const String& nameSpace, 
	const String& className) const;

private:

    CIMRepository* _repository;
};

RepositoryDeclContext::RepositoryDeclContext(CIMRepository* repository) 
    : _repository(repository)
{

}

RepositoryDeclContext::~RepositoryDeclContext()
{

}

CIMQualifierDecl RepositoryDeclContext::lookupQualifierDecl(
    const String& nameSpace,
    const String& qualifierName) const
{
    // Ignore the exception since this routine is only supposed report
    // whether it can be found:

    try
    {
	return _repository->getQualifier(nameSpace, qualifierName);
    }
    catch (Exception&)
    {
	return CIMQualifierDecl();
    }
}

CIMClass RepositoryDeclContext::lookupClass(
    const String& nameSpace,
    const String& className) const
{
    // Ignore the exception since this routine is only supposed report
    // whether it can be found:

    try
    {
	return _repository->getClass(nameSpace, className, false, true, true);
    }
    catch (Exception&)
    {
	return CIMClass();
    }
}

////////////////////////////////////////////////////////////////////////////////
//
// CIMRepository
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
    _LoadObject(classFilePath, cimClass);

    return cimClass;
}

static Boolean _GetInstanceIndex(
    NameSpaceManager& nameSpaceManager,
    const String& nameSpace,
    const CIMReference& instanceName,
    String& instanceFileBase,
    String& className,
    Uint32& index)
{
    // -- Get all descendent classes of this class:

    className = instanceName.getClassName();
    Array<String> classNames;
    nameSpaceManager.getSubClassNames(nameSpace, className, true, classNames);
    classNames.prepend(className);

    // -- Search each qualifying instance file for the instance:

    for (Uint32 i = 0; i < classNames.size(); i++)
    {
	CIMReference tmpInstanceName = instanceName;
	tmpInstanceName.setClassName(classNames[i]);

	// -- Get common base (of instance file and index file):

	instanceFileBase = 
	    nameSpaceManager.getInstanceFileBase(nameSpace, classNames[i]);

	// -- Lookup index of instance:

	String indexFilePath = instanceFileBase + ".idx";
    
	if (InstanceIndexFile::lookup(indexFilePath, tmpInstanceName, index))
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
    // -- Get the index for this instances:

    String instanceFileBase;
    String className;
    Uint32 index;

    if (!_GetInstanceIndex(_nameSpaceManager,
	nameSpace, instanceName, instanceFileBase, className, index))
    {
	throw PEGASUS_CIM_EXCEPTION(NOT_FOUND, "getInstance()");
    }

    // Load the instance file:

    char extension[32];
    sprintf(extension, ".%d", index);
    String instanceFilePath = instanceFileBase + extension;
    CIMInstance cimInstance;
    _LoadObject(instanceFilePath, cimInstance);
    return cimInstance;
}

void CIMRepository::deleteClass(
    const String& nameSpace,
    const String& className)
{
    _nameSpaceManager.deleteClass(nameSpace, className);
}

void CIMRepository::deleteInstance(
    const String& nameSpace,
    const CIMReference& instanceName) 
{
    throw PEGASUS_CIM_EXCEPTION(NOT_SUPPORTED, "deleteInstance()");
}

void CIMRepository::createClass(
    const String& nameSpace,
    CIMClass& newClass) 
{
    // -- Resolve the class:

    newClass.resolve(_context, nameSpace);

    // -- Create namespace manager entry:

    String classFilePath;

    _nameSpaceManager.createClass(nameSpace, newClass.getClassName(), 
	newClass.getSuperClassName(), classFilePath);

    // -- Create the class file:

    _SaveObject(classFilePath, newClass);
}

void CIMRepository::createInstance(
    const String& nameSpace,
    CIMInstance& newInstance) 
{
    // -- Resolve the instance (looks up class):

    CIMConstClass cimClass;
    newInstance.resolve(_context, nameSpace, cimClass);
    CIMReference instanceName = newInstance.getInstanceName(cimClass);

    // -- Be sure instance does not already exist:

    String dummyInstanceFileBase;
    String className;
    Uint32 dummyIndex;

    if (_GetInstanceIndex(_nameSpaceManager,
	nameSpace, instanceName, dummyInstanceFileBase, className, dummyIndex))
    {
	throw PEGASUS_CIM_EXCEPTION(ALREADY_EXISTS, instanceName.toString());
    }

    // -- Get common base (of instance file and index file):

    String instanceFileBase = _nameSpaceManager.getInstanceFileBase(nameSpace, 
	newInstance.getClassName());

    // -- Make index file entry:

    String indexFilePath = instanceFileBase + ".idx";
    Uint32 index;

    if (!InstanceIndexFile::insert(indexFilePath, instanceName, index))
	throw PEGASUS_CIM_EXCEPTION(ALREADY_EXISTS, instanceName.toString());

    // -- Save instance to file:

    char extension[32];
    sprintf(extension, ".%d", index);
    String instanceFilePath = instanceFileBase + extension;
    _SaveObject(instanceFilePath, newInstance);
}

void CIMRepository::modifyClass(
    const String& nameSpace,
    CIMClass& modifiedClass) 
{
    // -- Resolve the class:

    modifiedClass.resolve(_context, nameSpace);

    // -- Check to see if it is okay to modify this class:

    String classFilePath;

    _nameSpaceManager.checkModify(nameSpace, modifiedClass.getClassName(),
	modifiedClass.getSuperClassName(), classFilePath);

    // -- Delete the old file containing the class:

    if (!FileSystem::removeFile(classFilePath))
    {
	throw PEGASUS_CIM_EXCEPTION(FAILED, 
	    "failed to remove file in CIMRepository::modifyClass()");
    }

    // -- Create new class file:

    _SaveObject(classFilePath, modifiedClass);
}

void CIMRepository::modifyInstance(
    const String& nameSpace,
    const CIMInstance& modifiedInstance) 
{
    throw PEGASUS_CIM_EXCEPTION(NOT_SUPPORTED, "modifiedInstance()");
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
    return Array<String>();
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
    throw PEGASUS_CIM_EXCEPTION(NOT_SUPPORTED, "enumerateInstances()");

    return Array<CIMInstance>();
}

Array<CIMReference> CIMRepository::enumerateInstanceNames(
    const String& nameSpace,
    const String& className) 
{ 
    throw PEGASUS_CIM_EXCEPTION(NOT_SUPPORTED, "enumerateInstanceNames()");
    return Array<CIMReference>();
}

Array<CIMInstance> CIMRepository::execQuery(
    const String& queryLanguage,
    const String& query) 
{ 
    throw PEGASUS_CIM_EXCEPTION(NOT_SUPPORTED, "execQuery()");

    return Array<CIMInstance>();
}

Array<CIMInstance> CIMRepository::associators(
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
    throw PEGASUS_CIM_EXCEPTION(NOT_SUPPORTED, "associators()");
    return Array<CIMInstance>();
}

Array<CIMReference> CIMRepository::associatorNames(
    const String& nameSpace,
    const CIMReference& objectName,
    const String& assocClass,
    const String& resultClass,
    const String& role,
    const String& resultRole)
{ 
    throw PEGASUS_CIM_EXCEPTION(NOT_SUPPORTED, "associatorNames()");
    return Array<CIMReference>();
}

Array<CIMInstance> CIMRepository::references(
    const String& nameSpace,
    const CIMReference& objectName,
    const String& resultClass,
    const String& role,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const Array<String>& propertyList)
{
    throw PEGASUS_CIM_EXCEPTION(NOT_SUPPORTED, "references()");
    return Array<CIMInstance>();
}

Array<CIMReference> CIMRepository::referenceNames(
    const String& nameSpace,
    const CIMReference& objectName,
    const String& resultClass,
    const String& role)
{ 
    throw PEGASUS_CIM_EXCEPTION(NOT_SUPPORTED, "referenceNames()");
    return Array<CIMReference>();
}

CIMValue CIMRepository::getProperty(
    const String& nameSpace,
    const CIMReference& instanceName,
    const String& propertyName) 
{
    throw PEGASUS_CIM_EXCEPTION(NOT_SUPPORTED, "getProperty()");
    return CIMValue();
}

void CIMRepository::setProperty(
    const String& nameSpace,
    const CIMReference& instanceName,
    const String& propertyName,
    const CIMValue& newValue)
{ 
    throw PEGASUS_CIM_EXCEPTION(NOT_SUPPORTED, "setProperty()");
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
	return CIMQualifierDecl();
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

    if (FileSystem::exists(qualifierFilePath))
	throw PEGASUS_CIM_EXCEPTION(ALREADY_EXISTS, qualifierDecl.getName());

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

    if (!FileSystem::removeFile(qualifierFilePath))
	throw PEGASUS_CIM_EXCEPTION(NOT_FOUND, qualifierName);
}

Array<CIMQualifierDecl> CIMRepository::enumerateQualifiers(
    const String& nameSpace)
{
    String qualifiersRoot = _nameSpaceManager.getQualifiersRoot(nameSpace);

    Array<String> qualifierNames;

    if (!FileSystem::getDirectoryContents(qualifiersRoot, qualifierNames))
    {
	throw PEGASUS_CIM_EXCEPTION(FAILED, 
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
    throw PEGASUS_CIM_EXCEPTION(NOT_SUPPORTED, "invokeMethod()");
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

PEGASUS_NAMESPACE_END
