//BEGIN_LICENSE
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
//END_LICENSE
//BEGIN_HISTORY
//
// Author:
//
// $Log: CIMRepository.cpp,v $
// Revision 1.5  2001/02/26 04:33:30  mike
// Fixed many places where cim names were be compared with operator==(String,String).
// Changed all of these to use CIMName::equal()
//
// Revision 1.4  2001/02/20 07:25:57  mike
// Added basic create-instance in repository and in client.
//
// Revision 1.3  2001/02/19 01:47:17  mike
// Renamed names of the form CIMConst to ConstCIM.
//
// Revision 1.2  2001/02/18 19:02:17  mike
// Fixed CIM debacle
//
// Revision 1.1  2001/02/18 18:50:50  mike
// new
//
// Revision 1.8  2001/02/17 00:35:57  bob
// Addec <cstdio> for sprintf on linux
//
// Revision 1.7  2001/02/16 02:06:09  mike
// Renamed many classes and headers.
//
// Revision 1.6  2001/02/13 07:00:18  mike
// Added partial createInstance() method to repository.
//
// Revision 1.5  2001/02/11 05:45:33  mike
// Added case insensitive logic for files in CIMRepository
//
// Revision 1.4  2001/01/31 08:20:51  mike
// Added dispatcher framework.
// Added enumerateInstanceNames.
//
// Revision 1.3  2001/01/28 04:11:03  mike
// fixed qualifier resolution
//
// Revision 1.2  2001/01/25 02:12:05  mike
// Added meta-qualifiers to LoadRepository program.
//
// Revision 1.1.1.1  2001/01/14 19:53:55  mike
// Pegasus import
//
//
//END_HISTORY

#include <cctype>
#include <cstdio>  // for sprintf on linux
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

#define INDENT_XML_FILES

PEGASUS_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
//
// Local functions
//
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
//
// This routine matches files in the classes directory (given by path)
// that match the second and third arguments, which are eight class names
// or asterisks (which are wild cards). All the files in the ./classes
// directory are of the form <className.superClassName>. For classes
// with no superClass, the superClassName is "#". We consider a couple of
// examples. To find all direct subclasses of "MyClass", we invoke it
// as follows:
//
//	_GlobClassesDir(path, "*", "MyClass");
//
// To find the file which contains the class called "MyClass", we invoke
// it like this.
//
//	_GlobClassesDir(path, "MyClass", "*");
//
// Since base classes are of the form "<ClassName>.#", all baseclasses may
// be found with:
//
//	_GlobClassesDir(path, "*", "#");
//
// Note that the results (the array of filenames which are returned) must
// be processed further to get the actual class names. The name of the
// class is the filename less the extension. Or this:
//
//	String className = fileName.subString(fileName.find(0, '.'));
//
//------------------------------------------------------------------------------

Array<String> _GlobClassesDir(
    const String& path, 
    const String& className,
    const String& superClassName)
{
    Array<String> fileNames;

    if (!FileSystem::getDirectoryContents(path, fileNames))
	throw NoSuchDirectory(path);

    Array<String> result;

    for (Uint32 i = 0; i < fileNames.getSize(); i++)
    {
	const String& tmp = fileNames[i];

	Uint32 dot = tmp.find('.');

	// Ignore files that do not contain a dot:

	if (dot == Uint32(-1))
	    continue;

	String first = tmp.subString(0, dot);
	String second = tmp.subString(dot + 1);

	if ((String::equal(className, "*") || 
	    CIMName::equal(first, className)) &&
	    (String::equal(superClassName, "*") || 
	    CIMName::equal(second, superClassName)))
	{
	    result.append(tmp);
	}
    }

    return result;
}

static Boolean _SkipIdentifier(Char16*& p)
{
    if (!*p || !(isalpha(*p) || *p == '_'))
	return false;

    for (p++; *p; p++)
    {
	if (!(isalnum(*p) || *p == '_'))
	    return true;
    }

    return true;
}

static void _MakeNameSpacePath(
    const String& root,
    const String& nameSpace,
    String& path)
{
    path = root;
    path.append('/');

    path.append(nameSpace);

    Char16* p = (Char16*)(path.getData() + root.getLength() + 1);

    while (*p)
    {
	// Either we will encounter a slash or an identifier:

	if (*p == '/')
	{
	    if (p[1] == '/')
		throw CimException(CimException::INVALID_NAMESPACE);
	    
	    *p++ = '#';
	}
	else if (!_SkipIdentifier(p))
	    throw CimException(CimException::INVALID_NAMESPACE);
    }

    // The last element may NOT be a slash (slashes are translated to
    // #'s above).

    if (p[-1] == '#')
	throw CimException(CimException::INVALID_NAMESPACE);
}

void _FindClass(
    const String& root, 
    const String& nameSpace, 
    const String& className,
    String& path)
{
    const char CLASSES[] = "/classes/";
    _MakeNameSpacePath(root, nameSpace, path);

    if (!FileSystem::isDirectory(path))
	throw CimException(CimException::INVALID_NAMESPACE);

    path.append(CLASSES);

    Array<String> fileNames = _GlobClassesDir(path, className, "*");

    Uint32 size = fileNames.getSize();

    if (size == 0)
	throw CimException(CimException::INVALID_CLASS);

    PEGASUS_ASSERT(size == 1);
    path.append(fileNames[0]);
}

inline Uint32 _min(Uint32 x, Uint32 y)
{
    return x < y ? x : y;
}

static void _MakeNewClassPath(
    const String& root,
    const String& nameSpace,
    const String& className,
    const String& superClassName,
    String& path)
{
    const char CLASSES[] = "/classes/";
    _MakeNameSpacePath(root, nameSpace, path);

    if (!FileSystem::isDirectory(path))
	throw CimException(CimException::INVALID_NAMESPACE);

    path.append(CLASSES);
    path.append(className);
    path.append('.');

    if (superClassName.getLength() == 0)
	path.append("#");
    else
	path.append(superClassName);
}

static void _MakeQualfierPath(
    const String& root,
    const String& nameSpace,
    const String& qualifierName,
    String& path)
{
    const char QUALIFIERS[] = "/qualifiers/";
    _MakeNameSpacePath(root, nameSpace, path);

    if (!FileSystem::isDirectory(path))
	throw CimException(CimException::INVALID_NAMESPACE);

    path.append(QUALIFIERS);
    path.append(qualifierName);
}

static void _MakeInstanceIndexPath(
    const String& root,
    const String& nameSpace,
    const String& className,
    String& path)
{
    const char INSTANCES[] = "/instances/";
    _MakeNameSpacePath(root, nameSpace, path);

    if (!FileSystem::isDirectory(path))
	throw CimException(CimException::INVALID_NAMESPACE);

    path.append(INSTANCES);
    path.append(className);
    path.append(".idx");
}

static void _MakeInstancePath(
    const String& root,
    const String& nameSpace,
    const String& className,
    Uint32 index,
    String& path)
{
    const char INSTANCES[] = "/instances/";
    _MakeNameSpacePath(root, nameSpace, path);

    if (!FileSystem::isDirectory(path))
	throw CimException(CimException::INVALID_NAMESPACE);

    path.append(INSTANCES);
    path.append(className);
    path.append('.');

    char buffer[32];
    sprintf(buffer, "%d", index);
    path.append(buffer);
}

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

template<class Object>
void _SaveObject(const String& path, Object& object)
{
    Array<Sint8> out;
    object.toXml(out);
    out.append('\0');

    Destroyer<char> destroyer(path.allocateCString());

#ifdef PEGASUS_OS_TYPE_WINDOWS
    std::ofstream os(destroyer.getPointer(), std::ios::binary);
#else
    std::ofstream os(destroyer.getPointer());
#endif

    if (!os)
	throw CannotOpenFile(path);

#ifdef INDENT_XML_FILES
    XmlWriter::indentedPrint(os, out.getData(), 2);
#else
    os.write((char*)out.getData(), out.getSize());
#endif
}

static void _AppendClassNames(
    const String& path,
    const String& className,
    const String& superClassName,
    Array<String>& classNames)
{
    Array<String> allFiles;

    if (!FileSystem::getDirectoryContents(path, allFiles))
	throw NoSuchDirectory(path);

    // Append all the direct sublclasses of the class to the output argument:

    Array<String> fileNames = 
	_GlobClassesDir(path, className, superClassName);

    for (Uint32 i = 0, n = fileNames.getSize(); i < n; i++)
    {
	String& tmp = fileNames[i];
	Uint32 pos = tmp.find('.');

	PEGASUS_ASSERT(pos != Uint32(-1));

	if (pos != Uint32(-1))
	    tmp.remove(pos);

	classNames.append(tmp);
    }
}

typedef Pair<String,String> Node;

static void _AppendSubclassesDeepAux(
    const Array<Node>& table,
    const String& className,
    Array<String>& classNames)
{
    for (Uint32 i = 0, n = table.getSize(); i < n; i++)
    {
	if (CIMName::equal(className, table[i].second))
	{
	    classNames.append(table[i].first);
	    _AppendSubclassesDeepAux(table, table[i].first, classNames);
	}
    }
}

static void _AppendSubclassesDeep(
    const String& path,
    const String& className,
    Array<String>& classNames)
{
    Array<String> allFiles;

    if (!FileSystem::getDirectoryContents(path, allFiles))
	throw NoSuchDirectory(path);

    Array<Node> table;
    table.reserve(allFiles.getSize());

    for (Uint32 i = 0, n = allFiles.getSize(); i < n; i++)
    {
	const String& fileName = allFiles[i];

	Uint32 dot = fileName.find('.');

	if (dot == Uint32(-1))
	    continue;

	String first = fileName.subString(0, dot);
	String second = fileName.subString(dot + 1);

	if (String::equal(second, "#"))
	    table.append(Node(first, String()));
	else
	    table.append(Node(first, second));
    }

    _AppendSubclassesDeepAux(table, className, classNames);
}

static Boolean _HasSubclasses(
    const String& root,
    const String& nameSpace,
    const String& className)	
{
    const char CLASSES[] = "/classes";

    String path;
    _MakeNameSpacePath(root, nameSpace, path);
    path.append(CLASSES);
    Array<String> fileNames = _GlobClassesDir(path, "*", className);

    return fileNames.getSize() != 0;
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

    virtual CIMClass lookupClassDecl(
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

CIMClass RepositoryDeclContext::lookupClassDecl(
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

CIMRepository::CIMRepository(const String& path)
{
    const char REPOSITORY[] = "/repository";
    _root = path;
    _root.append(REPOSITORY);

    if (!FileSystem::isDirectory(_root))
    {
	if (!FileSystem::makeDirectory(_root))
	    throw CannotCreateDirectory(_root);
    }

    _context = new RepositoryDeclContext(this);
}

CIMRepository::~CIMRepository()
{

}

CIMClass CIMRepository::getClass(
    const String& nameSpace,
    const String& className,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const Array<String>& propertyList)
{
    // Form the path to the class:

    String path;
    _FindClass(_root, nameSpace, className, path);

    // Load the class:

    CIMClass cimClass;
    _LoadObject(path, cimClass);

    return CIMClass(cimClass);
}

CIMInstance CIMRepository::getInstance(
    const String& nameSpace,
    const CIMReference& instanceName,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const Array<String>& propertyList)
{
    // Get path of index file:

    String indexPath;

    _MakeInstanceIndexPath(
	_root, nameSpace, instanceName.getClassName(), indexPath);

    // Lookup the index of the instance:

    Uint32 index;

    // ATTN-A: Need to put instance name in standard form:

    String tmp;
    CIMReference::referenceToInstanceName(instanceName, tmp);

    if (!InstanceIndexFile::lookup(indexPath, tmp, index))
	throw CimException(CimException::FAILED);

    // Form the path to the instance fiel:

    String path;

    _MakeInstancePath(
	_root, nameSpace, instanceName.getClassName(), index, path);

    CIMInstance cimInstance;
    _LoadObject(path, cimInstance);
    return cimInstance;
}

void CIMRepository::deleteClass(
    const String& nameSpace,
    const String& className)
{
    // Get path of class file:

    String path;
    _FindClass(_root, nameSpace, className, path);

    // Disallow if the class has subclasses:

    if (_HasSubclasses(_root, nameSpace, className))
	throw CimException(CimException::CLASS_HAS_CHILDREN);

    // ATTN-C: check to see if the class has instances:

    // Remove the class:

    if (!FileSystem::removeFile(path))
	throw FailedToRemoveFile(path);
}

void CIMRepository::deleteInstance(
    const String& nameSpace,
    const CIMReference& instanceName) 
{ 
    throw CimException(CimException::NOT_SUPPORTED);
}

void CIMRepository::createClass(
    const String& nameSpace,
    CIMClass& newClass) 
{
    // Form the path to the class:

    String path;
    const String& className = newClass.getClassName();
    const String& superClassName = newClass.getSuperClassName();
    _MakeNewClassPath(
	_root, nameSpace, className, superClassName, path);

    String realPath;

    if (FileSystem::existsIgnoreCase(path, realPath))
	throw CimException(CimException::ALREADY_EXISTS);
    else
	realPath = path;

    // Validate the new class:

    newClass.resolve(_context, nameSpace);

    // Save the class:

    _SaveObject(realPath, newClass);
}

void CIMRepository::createInstance(
    const String& nameSpace,
    CIMInstance& newInstance) 
{
    // Lookup the class and resolve the instance:

    CIMClass cimClass = getClass(nameSpace, newInstance.getClassName());

    // ATTN-B: the class is looked up twice here!

    newInstance.resolve(_context, nameSpace);

    // Get the instance name from the new instance:
    // ATTN-1: Why is this necessary?

    String instanceName = newInstance.getInstanceName(cimClass);

    // Get the instance-name and create an entry

    String indexPath;

    _MakeInstanceIndexPath(
	_root, nameSpace, newInstance.getClassName(), indexPath);

    Uint32 index;

    if (!InstanceIndexFile::insert(indexPath, instanceName, index))
	throw CimException(CimException::FAILED);

    // Save the instance to file:

    String path;

    _MakeInstancePath(
	_root, nameSpace, newInstance.getClassName(), index, path);

    newInstance.resolve(_context, nameSpace);

    _SaveObject(path, newInstance);
}

void CIMRepository::modifyClass(
    const String& nameSpace,
    CIMClass& modifiedClass) 
{
    // ATTN: need lots of semantic checking here:

    // Get the old class:

    CIMClass oldClass = getClass(
	nameSpace, modifiedClass.getClassName(), false, true, true);

    // Disallow changing the name of the super-class:

    if (modifiedClass.getSuperClassName() != oldClass.getSuperClassName())
	throw CimException(CimException::INVALID_SUPERCLASS);

    // Delete the old class:

    deleteClass(nameSpace, modifiedClass.getClassName());

    // Create the class again:

    createClass(nameSpace, modifiedClass);
}

void CIMRepository::modifyInstance(
    const String& nameSpace,
    const CIMInstance& modifiedInstance) 
{
    throw CimException(CimException::NOT_SUPPORTED);
}

Array<CIMClass> CIMRepository::enumerateClasses(
    const String& nameSpace,
    const String& className,
    Boolean deepInheritance,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin)
{
    Array<String> classNames = 
	enumerateClassNames(nameSpace, className, deepInheritance);

    Array<CIMClass> result;

    for (Uint32 i = 0; i < classNames.getSize(); i++)
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
    // Build the path to the classes directory:

    const char CLASSES[] = "/classes/";
    String path;
    _MakeNameSpacePath(_root, nameSpace, path);
    path.append(CLASSES);

    if (!FileSystem::isDirectory(path))
	throw CimException(CimException::INVALID_NAMESPACE);

    if (deepInheritance)
    {
	if (className.getLength() == 0)
	{
	    Array<String> classNames;
	    _AppendSubclassesDeep(path, String(), classNames);
	    return classNames;
	}
	else
	{
	    Array<String> classNames;
	    _AppendSubclassesDeep(path, className, classNames);
	    return classNames;
	}
    }
    else
    {
	if (className.getLength() == 0)
	{
	    Array<String> classNames;
	    _AppendClassNames(path, "*", "#", classNames);
	    return classNames;
	}
	else
	{
	    Array<String> classNames;
	    _AppendClassNames(path, "*", className, classNames);
	    return classNames;
	}
    }

    // Unreachable:
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
    throw CimException(CimException::NOT_SUPPORTED);
    return Array<CIMInstance>();
}

Array<CIMReference> CIMRepository::enumerateInstanceNames(
    const String& nameSpace,
    const String& className) 
{ 
    throw CimException(CimException::NOT_SUPPORTED);
    return Array<CIMReference>();
}

Array<CIMInstance> CIMRepository::execQuery(
    const String& queryLanguage,
    const String& query) 
{ 
    throw CimException(CimException::NOT_SUPPORTED);
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
    throw CimException(CimException::NOT_SUPPORTED);
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
    throw CimException(CimException::NOT_SUPPORTED);
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
    throw CimException(CimException::NOT_SUPPORTED);
    return Array<CIMInstance>();
}

Array<CIMReference> CIMRepository::referenceNames(
    const String& nameSpace,
    const CIMReference& objectName,
    const String& resultClass,
    const String& role)
{ 
    throw CimException(CimException::NOT_SUPPORTED);
    return Array<CIMReference>();
}

CIMValue CIMRepository::getProperty(
    const String& nameSpace,
    const CIMReference& instanceName,
    const String& propertyName) 
{ 
    throw CimException(CimException::NOT_SUPPORTED);
    return CIMValue();
}

void CIMRepository::setProperty(
    const String& nameSpace,
    const CIMReference& instanceName,
    const String& propertyName,
    const CIMValue& newValue)
{ 
    throw CimException(CimException::NOT_SUPPORTED);
}

CIMQualifierDecl CIMRepository::getQualifier(
    const String& nameSpace,
    const String& qualifierName) 
{
    // Form the path of the qualifier file:

    String path;
    _MakeQualfierPath(_root, nameSpace, qualifierName, path);

    // If it does not exist:

    String realPath;

    if (!FileSystem::existsIgnoreCase(path, realPath))
	throw CimException(CimException::NOT_FOUND);

    // Load the qualifier:

    CIMQualifierDecl qualifierDecl;
    _LoadObject(realPath, qualifierDecl);

    // Return the qualifier:

    return CIMQualifierDecl(qualifierDecl);
}

void CIMRepository::setQualifier(
    const String& nameSpace,
    const CIMQualifierDecl& qualifierDecl) 
{
    // Form the path of the qualifier:

    String path;
    _MakeQualfierPath(_root, nameSpace, qualifierDecl.getName(), path);

    // If the qualifier already exists, delete it:

    String realPath;

    if (FileSystem::existsIgnoreCase(path, realPath))
    {
	if (!FileSystem::removeFile(realPath))
	    throw FailedToRemoveDirectory(path);
    }
    else
	realPath = path;

    // Write the qualifier to file:

    _SaveObject(realPath, qualifierDecl);
}

void CIMRepository::deleteQualifier(
    const String& nameSpace,
    const String& qualifierName) 
{
    String path;
    _MakeQualfierPath(_root, nameSpace, qualifierName, path);

    String realPath;

    if (!FileSystem::existsIgnoreCase(path, realPath))
	throw CimException(CimException::NOT_FOUND);

    if (!FileSystem::removeFile(realPath))
	throw FailedToRemoveFile(path);
}

Array<CIMQualifierDecl> CIMRepository::enumerateQualifiers(
    const String& nameSpace)
{
    // Build the path to the qualifiers directory:

    const char QUALIFIERS[] = "/qualifiers/";
    String path;
    _MakeNameSpacePath(_root, nameSpace, path);
    path.append(QUALIFIERS);

    if (!FileSystem::isDirectory(path))
	throw CimException(CimException::INVALID_NAMESPACE);

    // Get the names of the qualifiers:

    Array<String> qualifierNames;

    if (!FileSystem::getDirectoryContents(path, qualifierNames))
	throw NoSuchDirectory(path);

    // Load each qualifier into the result array:

    Array<CIMQualifierDecl> result;

    for (Uint32 i = 0, n = qualifierNames.getSize(); i < n; i++)
    {
	CIMQualifierDecl tmp = getQualifier(nameSpace, qualifierNames[i]);
	result.append(tmp);
    }

    return result;
}

CIMValue CIMRepository::invokeMethod(
    const String& nameSpace,
    const CIMReference& instanceName,
    const String& methodName,
    const Array<CIMValue>& inParameters,
    Array<CIMValue>& outParameters) 
{
    throw CimException(CimException::NOT_SUPPORTED);
    return CIMValue();
}

////////////////////////////////////////////////////////////////////////////////
//
// New methods
//
////////////////////////////////////////////////////////////////////////////////

void CIMRepository::createNameSpace(const String& nameSpace)
{
    String path;
    _MakeNameSpacePath(_root, nameSpace, path);

    if (FileSystem::exists(path))
	throw AlreadyExists(nameSpace);

    if (!FileSystem::makeDirectory(path))
	throw CannotCreateDirectory(path);

    // Create "./qualifiers" directory:

    String qualifiersDir = path;
    qualifiersDir.append("/qualifiers");

    if (!FileSystem::makeDirectory(qualifiersDir))
	throw CannotCreateDirectory(qualifiersDir);

    // Create "./classes" directory:

    String classesDir = path;
    classesDir.append("/classes");

    if (!FileSystem::makeDirectory(classesDir))
	throw CannotCreateDirectory(classesDir);

    // Create "./instances" directory:

    String instancesDir = path;
    instancesDir.append("/instances");

    if (!FileSystem::makeDirectory(instancesDir))
	throw CannotCreateDirectory(instancesDir);
}

Array<String> CIMRepository::enumerateNameSpaces() const
{
    Array<String> result;

    if (!FileSystem::getDirectoryContents(_root, result))
	throw NoSuchDirectory(_root);

    for (Uint32 i = 0, n = result.getSize(); i < n; i++)
    {
	const String& tmp = result[i];

	for (Char16* p = (Char16*)tmp.getData(); *p; p++)
	{
	    if (*p == '#')
		*p = '/';
	}
    }

    return result;
}

// Recall flavor defaults: TOSUBCLASS | OVERRIDABLE

void CIMRepository::createMetaQualifiers(const String& nameSpace)
{
    // CIMQualifier CimType : string = null, 
    //     CIMScope(property, parameter)

    setQualifier(nameSpace, CIMQualifierDecl("cimtype", String(),
	CIMScope::PROPERTY | CIMScope::REFERENCE | CIMScope::PARAMETER));

    // CIMQualifier id : sint32 = null, 
    //     CIMScope(any), 
    //     CIMFlavor(toinstance)

    setQualifier(nameSpace, CIMQualifierDecl("id", Sint32(0),
	CIMScope::ANY,
	CIMFlavor::TOINSTANCE));

    // CIMQualifier OctetString : boolean = false, CIMScope(property)

    setQualifier(nameSpace, CIMQualifierDecl("octetstring", false,
	CIMScope::PROPERTY));
    
    // CIMQualifier Abstract : boolean = false, 
    //     CIMScope(class, association, indication),
    //     CIMFlavor(disableoverride, restricted);

    setQualifier(nameSpace, CIMQualifierDecl("abstract", false, 
	CIMScope::CLASS | CIMScope::ASSOCIATION | CIMScope::INDICATION,
	CIMFlavor::NONE));

    // CIMQualifier Aggregate : boolean = false, 
    //    CIMScope(reference),
    //    CIMFlavor(disableoverride, tosubclass);

    setQualifier(nameSpace, CIMQualifierDecl("aggregate", false, 
	CIMScope::REFERENCE, CIMFlavor::TOSUBCLASS));

    // CIMQualifier Aggregation : boolean = false, 
    //     CIMScope(association),
    //     CIMFlavor(disableoverride, tosubclass);

    setQualifier(nameSpace, CIMQualifierDecl("aggregation", false, 
	CIMScope::ASSOCIATION, CIMFlavor::TOSUBCLASS));

    // CIMQualifier Alias : string = null, 
    //     CIMScope(property, reference, method), 
    //     CIMFlavor(translatable);

    setQualifier(nameSpace, CIMQualifierDecl("alias", String(),
	CIMScope::PROPERTY | CIMScope::REFERENCE | CIMScope::METHOD,
	CIMFlavor::DEFAULTS | CIMFlavor::TRANSLATABLE));

    // CIMQualifier ArrayType : string = "Bag", 
    //     CIMScope(property, parameter);

    setQualifier(nameSpace, CIMQualifierDecl("arraytype", "Bag",
	CIMScope::PROPERTY | CIMScope::PARAMETER));

    // CIMQualifier Association : boolean = false, 
    //     CIMScope(class, association),
    //     CIMFlavor(disableoverride);

    setQualifier(nameSpace, CIMQualifierDecl("association", false,
	CIMScope::CLASS | CIMScope::ASSOCIATION,
	CIMFlavor::TOSUBCLASS));

    // CIMQualifier BitMap : string[], 
    //     CIMScope(property, method, parameter);

    setQualifier(nameSpace, CIMQualifierDecl("bitmap", Array<String>(),
	CIMScope::PROPERTY | CIMScope::METHOD | CIMScope::PARAMETER));

    // CIMQualifier BitValues : string[], 
    //     CIMScope(property, method, parameter),
    //     CIMFlavor(Translatable);

    setQualifier(nameSpace, CIMQualifierDecl("bitvalues", Array<String>(),
	CIMScope::PROPERTY | CIMScope::METHOD | CIMScope::PARAMETER,
	CIMFlavor::DEFAULTS | CIMFlavor::TRANSLATABLE));

    // CIMQualifier Counter : boolean = false, 
    //     CIMScope(property, method, parameter);

    setQualifier(nameSpace, CIMQualifierDecl("counter", false,
	CIMScope::PROPERTY | CIMScope::METHOD | CIMScope::PARAMETER));

    // CIMQualifier Delete : boolean = false, 
    //     CIMScope(association, reference);

    setQualifier(nameSpace, CIMQualifierDecl("delete", false,
	CIMScope::ASSOCIATION | CIMScope::REFERENCE));

    // CIMQualifier Description : string = null, 
    //     CIMScope(any), 
    //     CIMFlavor(translatable);

    setQualifier(nameSpace, CIMQualifierDecl("description", String(),
	CIMScope::ANY, 
	CIMFlavor::TRANSLATABLE));

    // CIMQualifier DisplayName : string = null, 
    //     CIMScope(any), 
    //     CIMFlavor(translatable);

    setQualifier(nameSpace, CIMQualifierDecl("displayname", String(),
	CIMScope::ANY,
	CIMFlavor::DEFAULTS | CIMFlavor::TRANSLATABLE));

    // CIMQualifier Expensive : boolean = false,
    //     CIMScope(property, reference, method, class, association);

    setQualifier(nameSpace, CIMQualifierDecl("expensive", false,
	CIMScope::PROPERTY | CIMScope::REFERENCE | CIMScope::METHOD | CIMScope::CLASS |
	CIMScope::ASSOCIATION));

    // CIMQualifier Gauge : boolean = false, 
    //     CIMScope(property, method, parameter);

    setQualifier(nameSpace, CIMQualifierDecl("gauge", false,
	CIMScope::PROPERTY | CIMScope::METHOD | CIMScope::PARAMETER));

    // CIMQualifier Ifdeleted : boolean = false, 
    //     CIMScope(association, reference);

    setQualifier(nameSpace, CIMQualifierDecl("ifdeleted", false,
	CIMScope::ASSOCIATION | CIMScope::REFERENCE));

    // CIMQualifier In : boolean = true, 
    //     CIMScope(parameter), 
    //     CIMFlavor(disableoverride);

    setQualifier(nameSpace, CIMQualifierDecl("in", true,
	CIMScope::PARAMETER,
	CIMFlavor::TOSUBCLASS));

    // CIMQualifier Indication : boolean = false, 
    //     CIMScope(class, indication),
    //     CIMFlavor(disableoverride);

    setQualifier(nameSpace, CIMQualifierDecl("indication", false,
	CIMScope::CLASS | CIMScope::INDICATION,
	CIMFlavor::TOSUBCLASS));

    // CIMQualifier Invisible : boolean = false,
    //     CIMScope(reference, association, class, property, method);

    setQualifier(nameSpace, CIMQualifierDecl("invisible", false,
	CIMScope::REFERENCE | CIMScope::ASSOCIATION | CIMScope::CLASS | CIMScope::PROPERTY |
	CIMScope::METHOD));

    // CIMQualifier Key : boolean = false, 
    //     CIMScope(property, reference),
    //     CIMFlavor(disableoverride);

    setQualifier(nameSpace, CIMQualifierDecl("key", false,
	CIMScope::PROPERTY | CIMScope::REFERENCE,
	CIMFlavor::TOSUBCLASS));

    // CIMQualifier Large : boolean = false, 
    //     CIMScope(property, class);

    setQualifier(nameSpace, CIMQualifierDecl("large", false,
	CIMScope::PROPERTY | CIMScope::CLASS));

    // CIMQualifier MappingStrings : string[],
    //     CIMScope(class, property, association, indication, reference);

    setQualifier(nameSpace, CIMQualifierDecl("mappingstrings", Array<String>(),
	CIMScope::CLASS | CIMScope::PROPERTY | CIMScope::ASSOCIATION | 
	CIMScope::INDICATION | CIMScope::REFERENCE));

    // CIMQualifier Max : uint32 = null, CIMScope(reference);

    setQualifier(nameSpace, CIMQualifierDecl("max", Sint32(0),
	CIMScope::PROPERTY | CIMScope::REFERENCE));

    // CIMQualifier MaxLen : uint32 = null, 
    //     CIMScope(property, method, parameter);

#if 0
    setQualifier(nameSpace, CIMQualifierDecl("maxlen", Uint32(0),
	CIMScope::PROPERTY | CIMScope::METHOD | CIMScope::PARAMETER));
#else
    setQualifier(nameSpace, CIMQualifierDecl("maxlen", Sint32(0),
	CIMScope::PROPERTY | CIMScope::METHOD | CIMScope::PARAMETER));
#endif

    // CIMQualifier MaxValue : sint64 = null, 
    //     CIMScope(property, method, parameter);
    // ATTN: XML schema requires sint32!

    setQualifier(nameSpace, CIMQualifierDecl("maxvalue", Sint32(0),
	CIMScope::PROPERTY | CIMScope::METHOD | CIMScope::PARAMETER));
    
    // CIMQualifier Min : sint32 = null, CIMScope(reference);

    setQualifier(nameSpace, CIMQualifierDecl("min", Sint32(0),
	CIMScope::REFERENCE));

    // CIMQualifier MinValue : sint64 = null, 
    //     CIMScope(property, method, parameter);
    // ATTN: CIMType expected by XML spec is sint32!

    setQualifier(nameSpace, CIMQualifierDecl("minvalue", Sint32(0),
	CIMScope::PROPERTY | CIMScope::METHOD | CIMScope::PARAMETER));

    // CIMQualifier ModelCorrespondence : string[], 
    //     CIMScope(property);

    setQualifier(nameSpace, CIMQualifierDecl("modelcorrespondence", 
	Array<String>(),
	CIMScope::PROPERTY));

    // CIMQualifier NonLocal : string = null, 
    //     CIMScope(reference);

    setQualifier(nameSpace, CIMQualifierDecl("nonlocal", String(),
	CIMScope::REFERENCE));

    // CIMQualifier NullValue : string = null, 
    //     CIMScope(property),
    //     CIMFlavor(tosubclass, disableoverride);

    setQualifier(nameSpace, CIMQualifierDecl("nullvalue", String(),
	CIMScope::PROPERTY,
	CIMFlavor::TOSUBCLASS));

    // CIMQualifier Out : boolean = false, 
    //     CIMScope(parameter), 
    //     CIMFlavor(disableoverride);

    setQualifier(nameSpace, CIMQualifierDecl("out", false,
	CIMScope::PARAMETER,
	CIMFlavor::TOSUBCLASS));

    // CIMQualifier Override : string = null, 
    //     CIMScope(property, method, reference),
    //     CIMFlavor(disableoverride);

    setQualifier(nameSpace, CIMQualifierDecl("override", String(),
	CIMScope::PROPERTY | CIMScope::METHOD | CIMScope::REFERENCE,
	CIMFlavor::TOSUBCLASS));

    // CIMQualifier Propagated : string = null, 
    //     CIMScope(property, reference),
    //     CIMFlavor(disableoverride);

    setQualifier(nameSpace, CIMQualifierDecl("propagated", String(),
	CIMScope::PROPERTY | CIMScope::REFERENCE,
	CIMFlavor::TOSUBCLASS));

    // CIMQualifier Provider : string = null, 
    //     CIMScope(any);

    setQualifier(nameSpace, CIMQualifierDecl("provider", String(),
	CIMScope::ANY));

    // CIMQualifier Read : boolean = true, CIMScope(property);

    setQualifier(nameSpace, CIMQualifierDecl("read", true,
	CIMScope::PROPERTY));

    // CIMQualifier Required : boolean = false, 
    //     CIMScope(property);

    setQualifier(nameSpace, CIMQualifierDecl("required", false,
	CIMScope::PROPERTY));

    // CIMQualifier Revision : string = null,
    //     CIMScope(schema, class, association, indication),
    //     CIMFlavor(translatable);
    // ATTN: No such scope as CIMScope::SCHEMA

    setQualifier(nameSpace, CIMQualifierDecl("revision", String(),
	CIMScope::CLASS | CIMScope::ASSOCIATION | CIMScope::INDICATION,
	CIMFlavor::DEFAULTS | CIMFlavor::TRANSLATABLE));

    // CIMQualifier Schema : string = null, 
    //     CIMScope(property, method),
    //     CIMFlavor(disableoverride, translatable);

    setQualifier(nameSpace, CIMQualifierDecl("schema", String(),
	CIMScope::PROPERTY | CIMScope::METHOD,
	CIMFlavor::TOSUBCLASS | CIMFlavor::TRANSLATABLE));

    // CIMQualifier Source : string = null, 
    //     CIMScope(class, association, indication);

    setQualifier(nameSpace, CIMQualifierDecl("source", String(),
	CIMScope::CLASS | CIMScope::ASSOCIATION | CIMScope::INDICATION));

    // CIMQualifier SourceType : string = null,
    //     CIMScope(class, association, indication,reference);

    setQualifier(nameSpace, CIMQualifierDecl("sourcetype", String(),
	CIMScope::CLASS | CIMScope::ASSOCIATION | CIMScope:: INDICATION |
	CIMScope::REFERENCE));
    
    // CIMQualifier Static : boolean = false,
    //     CIMScope(property, method), CIMFlavor(disableoverride);

    setQualifier(nameSpace, CIMQualifierDecl("static", false,
	CIMScope::PROPERTY | CIMScope::METHOD,
	CIMFlavor::TOSUBCLASS));

    // CIMQualifier Syntax : string = null,
    //     CIMScope(property, reference, method, parameter);

    setQualifier(nameSpace, CIMQualifierDecl("syntax", String(),
	CIMScope::PROPERTY | CIMScope::REFERENCE | CIMScope::METHOD | CIMScope::PARAMETER));

    // CIMQualifier SyntaxType : string = null,
    //     CIMScope(property, reference, method, parameter);

    setQualifier(nameSpace, CIMQualifierDecl("syntaxtype", String(),
	CIMScope::PROPERTY | CIMScope::REFERENCE | CIMScope::METHOD | CIMScope::PARAMETER));

    // CIMQualifier Terminal : boolean = false, 
    //     CIMScope(class);

    setQualifier(nameSpace, CIMQualifierDecl("terminal", false,
	CIMScope::CLASS));

    // CIMQualifier TriggerType : string = null,
    //     CIMScope(class, property, reference, method, association, indication);

    setQualifier(nameSpace, CIMQualifierDecl("triggertype", String(),
	CIMScope::CLASS | CIMScope::PROPERTY | CIMScope::REFERENCE | CIMScope::METHOD |
	CIMScope::ASSOCIATION | CIMScope::INDICATION));

    // CIMQualifier Units : string = null, 
    //     CIMScope(property, method, parameter),
    //     CIMFlavor(translatable);

    setQualifier(nameSpace, CIMQualifierDecl("units", String(),
	CIMScope::PROPERTY | CIMScope::METHOD | CIMScope::PARAMETER,
	CIMFlavor::DEFAULTS | CIMFlavor::TRANSLATABLE));

    // CIMQualifier UnknownValues : string[], 
    //     CIMScope(property), 
    //     CIMFlavor(disableoverride, tosubclass);

    setQualifier(nameSpace, CIMQualifierDecl("unknownvalues", Array<String>(),
	CIMScope::PROPERTY,
	CIMFlavor::TOSUBCLASS));

    // CIMQualifier UnsupportedValues : string[], 
    //     CIMScope(property),
    //     CIMFlavor(disableoverride, tosubclass);

    setQualifier(nameSpace, CIMQualifierDecl("unsupportedvalues", Array<String>(),
	CIMScope::PROPERTY,
	CIMFlavor::TOSUBCLASS));

    // CIMQualifier ValueMap : string[], 
    //     CIMScope(property, method, parameter);

    setQualifier(nameSpace, CIMQualifierDecl("valuemap", Array<String>(),
	CIMScope::PROPERTY | CIMScope::METHOD | CIMScope::PARAMETER));

    // CIMQualifier Values : string[], 
    //     CIMScope(property, method, parameter),
    //     CIMFlavor(translatable);

    setQualifier(nameSpace, CIMQualifierDecl("values", Array<String>(),
	CIMScope::PROPERTY | CIMScope::METHOD | CIMScope::PARAMETER,
	CIMFlavor::DEFAULTS | CIMFlavor::TRANSLATABLE));

    // CIMQualifier Version : string = null,
    //     CIMScope(schema, class, association, indication), 
    //     CIMFlavor(translatable);
    // ATTN: No such scope as CIMScope::SCHEMA

    setQualifier(nameSpace, CIMQualifierDecl("version", String(),
	CIMScope::CLASS | CIMScope::ASSOCIATION | CIMScope::INDICATION,
	CIMFlavor::DEFAULTS | CIMFlavor::TRANSLATABLE));

    // CIMQualifier Weak : boolean = false, 
    //     CIMScope(reference),
    //     CIMFlavor(disableoverride, tosubclass);

    setQualifier(nameSpace, CIMQualifierDecl("weak", false,
	CIMScope::REFERENCE,
	CIMFlavor::TOSUBCLASS));

    // CIMQualifier Write : boolean = false, 
    //     CIMScope(property);

    setQualifier(nameSpace, CIMQualifierDecl("write", false,
	CIMScope::PROPERTY));
}

PEGASUS_NAMESPACE_END
