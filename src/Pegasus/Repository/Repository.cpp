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
// $Log: Repository.cpp,v $
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
#include <fstream>
#include <Pegasus/Common/Pair.h>
#include <Pegasus/Common/Destroyer.h>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/XmlReader.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/DeclContext.h>
#include "Repository.h"

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

	if ((className == "*" || first == className) &&
	    (superClassName == "*" || second == superClassName))
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

    path.append('/');
    path.append(QUALIFIERS);
    path.append(qualifierName);
}

template<class Object>
void _LoadObject(
    const String& path,
    Object& object)
{
    // Open the file:

    Destroyer<char> destroyer(path.allocateCString());
    std::ifstream is(destroyer.getPointer());

    if (!is)
	throw CannotOpenFile(path);

    // Load file into memory:

    Array<Sint8> data;
    FileSystem::loadFileToMemory(data, path);
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
	if (className == table[i].second)
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

	if (second == "#")
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

    RepositoryDeclContext(Repository* repository);

    virtual ~RepositoryDeclContext();

    virtual QualifierDecl lookupQualifierDecl(
	const String& nameSpace, 
	const String& qualifierName) const;

    virtual ClassDecl lookupClassDecl(
	const String& nameSpace, 
	const String& className) const;

private:

    Repository* _repository;
};

RepositoryDeclContext::RepositoryDeclContext(Repository* repository) 
    : _repository(repository)
{

}

RepositoryDeclContext::~RepositoryDeclContext()
{

}

QualifierDecl RepositoryDeclContext::lookupQualifierDecl(
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
	return QualifierDecl();
    }
}

ClassDecl RepositoryDeclContext::lookupClassDecl(
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
	return ClassDecl();
    }
}

////////////////////////////////////////////////////////////////////////////////
//
// Repository
//
////////////////////////////////////////////////////////////////////////////////

Repository::Repository(const String& path)
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

Repository::~Repository()
{

}

ClassDecl Repository::getClass(
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

    ClassDecl classDecl;
    _LoadObject(path, classDecl);

    return ClassDecl(classDecl);
}

InstanceDecl Repository::getInstance(
    const String& nameSpace,
    const Reference& instanceName,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const Array<String>& propertyList)
{
    throw CimException(CimException::NOT_SUPPORTED);
    return InstanceDecl();
}

void Repository::deleteClass(
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

void Repository::deleteInstance(
    const String& nameSpace,
    const Reference& instanceName) 
{ 
    throw CimException(CimException::NOT_SUPPORTED);
}

void Repository::createClass(
    const String& nameSpace,
    ClassDecl& newClass) 
{
    // Form the path to the class:

    String path;
    const String& className = newClass.getClassName();
    const String& superClassName = newClass.getSuperClassName();
    _MakeNewClassPath(_root, nameSpace, className, superClassName, path);

    if (FileSystem::exists(path))
	throw CimException(CimException::ALREADY_EXISTS);

    // Validate the new class:

    newClass.resolve(_context, nameSpace);

    // Save the class:

    _SaveObject(path, newClass);
}

void Repository::createInstance(
    const String& nameSpace,
    const InstanceDecl& newInstance) 
{ 
    throw CimException(CimException::NOT_SUPPORTED);
}

void Repository::modifyClass(
    const String& nameSpace,
    ClassDecl& modifiedClass) 
{
    // ATTN: need lots of semantic checking here:

    // Get the old class:

    ClassDecl oldClass = getClass(
	nameSpace, modifiedClass.getClassName(), false, true, true);

    // Disallow changing the name of the super-class:

    if (modifiedClass.getSuperClassName() != oldClass.getSuperClassName())
	throw CimException(CimException::INVALID_SUPERCLASS);

    // Delete the old class:

    deleteClass(nameSpace, modifiedClass.getClassName());

    // Create the class again:

    createClass(nameSpace, modifiedClass);
}

void Repository::modifyInstance(
    const String& nameSpace,
    const InstanceDecl& modifiedInstance) 
{
    throw CimException(CimException::NOT_SUPPORTED);
}

Array<ClassDecl> Repository::enumerateClasses(
    const String& nameSpace,
    const String& className,
    Boolean deepInheritance,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin)
{
    Array<String> classNames = 
	enumerateClassNames(nameSpace, className, deepInheritance);

    Array<ClassDecl> result;

    for (Uint32 i = 0; i < classNames.getSize(); i++)
    {
	result.append(getClass(nameSpace, classNames[i], localOnly, 
	    includeQualifiers, includeClassOrigin));
    }

    return result;
}

Array<String> Repository::enumerateClassNames(
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
	if (className == String::EMPTY)
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
	if (className == String::EMPTY)
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

Array<InstanceDecl> Repository::enumerateInstances(
    const String& nameSpace,
    const String& className,
    Boolean deepInheritance,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const Array<String>& propertyList)
{ 
    throw CimException(CimException::NOT_SUPPORTED);
    return Array<InstanceDecl>();
}

Array<Reference> Repository::enumerateInstanceNames(
    const String& nameSpace,
    const String& className) 
{ 
    throw CimException(CimException::NOT_SUPPORTED);
    return Array<Reference>();
}

Array<InstanceDecl> Repository::execQuery(
    const String& queryLanguage,
    const String& query) 
{ 
    throw CimException(CimException::NOT_SUPPORTED);
    return Array<InstanceDecl>();
}

Array<InstanceDecl> Repository::associators(
    const String& nameSpace,
    const Reference& objectName,
    const String& assocClass,
    const String& resultClass,
    const String& role,
    const String& resultRole,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const Array<String>& propertyList)
{
    throw CimException(CimException::NOT_SUPPORTED);
    return Array<InstanceDecl>();
}

Array<Reference> Repository::associatorNames(
    const String& nameSpace,
    const Reference& objectName,
    const String& assocClass,
    const String& resultClass,
    const String& role,
    const String& resultRole)
{ 
    throw CimException(CimException::NOT_SUPPORTED);
    return Array<Reference>();
}

Array<InstanceDecl> Repository::references(
    const String& nameSpace,
    const Reference& objectName,
    const String& resultClass,
    const String& role,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const Array<String>& propertyList)
{
    throw CimException(CimException::NOT_SUPPORTED);
    return Array<InstanceDecl>();
}

Array<Reference> Repository::referenceNames(
    const String& nameSpace,
    const Reference& objectName,
    const String& resultClass,
    const String& role)
{ 
    throw CimException(CimException::NOT_SUPPORTED);
    return Array<Reference>();
}

Value Repository::getProperty(
    const String& nameSpace,
    const Reference& instanceName,
    const String& propertyName) 
{ 
    throw CimException(CimException::NOT_SUPPORTED);
    return Value();
}

void Repository::setProperty(
    const String& nameSpace,
    const Reference& instanceName,
    const String& propertyName,
    const Value& newValue)
{ 
    throw CimException(CimException::NOT_SUPPORTED);
}

QualifierDecl Repository::getQualifier(
    const String& nameSpace,
    const String& qualifierName) 
{
    // Form the path of the qualifier file:

    String path;
    _MakeQualfierPath(_root, nameSpace, qualifierName, path);

    // If it does not exist:

    if (!FileSystem::exists(path))
	throw CimException(CimException::NOT_FOUND);

    // Load the qualifier:

    QualifierDecl qualifierDecl;
    _LoadObject(path, qualifierDecl);

    return QualifierDecl(qualifierDecl);
}

void Repository::setQualifier(
    const String& nameSpace,
    const QualifierDecl& qualifierDecl) 
{
    // Form the path of the qualifier:

    String path;
    _MakeQualfierPath(_root, nameSpace, qualifierDecl.getName(), path);

    // If the qualifier already exists, delete it:

    if (FileSystem::exists(path))
    {
	if (!FileSystem::removeFile(path))
	    throw FailedToRemoveDirectory(path);
    }

    // Write the qualifier to file:

    _SaveObject(path, qualifierDecl);
}

void Repository::deleteQualifier(
    const String& nameSpace,
    const String& qualifierName) 
{
    String path;
    _MakeQualfierPath(_root, nameSpace, qualifierName, path);

    if (!FileSystem::exists(path))
	throw CimException(CimException::NOT_FOUND);

    if (!FileSystem::removeFile(path))
	throw FailedToRemoveFile(path);
}

Array<QualifierDecl> Repository::enumerateQualifiers(
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

    Array<QualifierDecl> result;

    for (Uint32 i = 0, n = qualifierNames.getSize(); i < n; i++)
    {
	QualifierDecl tmp = getQualifier(nameSpace, qualifierNames[i]);
	result.append(tmp);
    }

    return result;
}

Value Repository::invokeMethod(
    const String& nameSpace,
    const Reference& instanceName,
    const String& methodName,
    const Array<Value>& inParameters,
    Array<Value>& outParameters) 
{
    throw CimException(CimException::NOT_SUPPORTED);
    return Value();
}

////////////////////////////////////////////////////////////////////////////////
//
// New methods
//
////////////////////////////////////////////////////////////////////////////////

void Repository::createNameSpace(const String& nameSpace)
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

Array<String> Repository::enumerateNameSpaces() const
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

void Repository::createMetaQualifiers(const String& nameSpace)
{
    // Qualifier CimType : string = null, 
    //     Scope(property, parameter)

    setQualifier(nameSpace, QualifierDecl("cimtype", String(),
	Scope::PROPERTY | Scope::REFERENCE | Scope::PARAMETER));

    // Qualifier id : sint32 = null, 
    //     Scope(any), 
    //     Flavor(toinstance)

    setQualifier(nameSpace, QualifierDecl("id", Sint32(0),
	Scope::ANY,
	Flavor::TOINSTANCE));

    // Qualifier OctetString : boolean = false, Scope(property)

    setQualifier(nameSpace, QualifierDecl("octetstring", false,
	Scope::PROPERTY));
    
    // Qualifier Abstract : boolean = false, 
    //     Scope(class, association, indication),
    //     Flavor(disableoverride, restricted);

    setQualifier(nameSpace, QualifierDecl("abstract", false, 
	Scope::CLASS | Scope::ASSOCIATION | Scope::INDICATION,
	Flavor::NONE));

    // Qualifier Aggregate : boolean = false, 
    //    Scope(reference),
    //    Flavor(disableoverride, tosubclass);

    setQualifier(nameSpace, QualifierDecl("aggregate", false, 
	Scope::REFERENCE, Flavor::TOSUBCLASS));

    // Qualifier Aggregation : boolean = false, 
    //     Scope(association),
    //     Flavor(disableoverride, tosubclass);

    setQualifier(nameSpace, QualifierDecl("aggregation", false, 
	Scope::ASSOCIATION, Flavor::TOSUBCLASS));

    // Qualifier Alias : string = null, 
    //     Scope(property, reference, method), 
    //     Flavor(translatable);

    setQualifier(nameSpace, QualifierDecl("alias", String(),
	Scope::PROPERTY | Scope::REFERENCE | Scope::METHOD,
	Flavor::DEFAULTS | Flavor::TRANSLATABLE));

    // Qualifier ArrayType : string = "Bag", 
    //     Scope(property, parameter);

    setQualifier(nameSpace, QualifierDecl("arraytype", "Bag",
	Scope::PROPERTY | Scope::PARAMETER));

    // Qualifier Association : boolean = false, 
    //     Scope(class, association),
    //     Flavor(disableoverride);

    setQualifier(nameSpace, QualifierDecl("association", false,
	Scope::CLASS | Scope::ASSOCIATION,
	Flavor::TOSUBCLASS));

    // Qualifier BitMap : string[], 
    //     Scope(property, method, parameter);

    setQualifier(nameSpace, QualifierDecl("bitmap", Array<String>(),
	Scope::PROPERTY | Scope::METHOD | Scope::PARAMETER));

    // Qualifier BitValues : string[], 
    //     Scope(property, method, parameter),
    //     Flavor(Translatable);

    setQualifier(nameSpace, QualifierDecl("bitvalues", Array<String>(),
	Scope::PROPERTY | Scope::METHOD | Scope::PARAMETER,
	Flavor::DEFAULTS | Flavor::TRANSLATABLE));

    // Qualifier Counter : boolean = false, 
    //     Scope(property, method, parameter);

    setQualifier(nameSpace, QualifierDecl("counter", false,
	Scope::PROPERTY | Scope::METHOD | Scope::PARAMETER));

    // Qualifier Delete : boolean = false, 
    //     Scope(association, reference);

    setQualifier(nameSpace, QualifierDecl("delete", false,
	Scope::ASSOCIATION | Scope::REFERENCE));

    // Qualifier Description : string = null, 
    //     Scope(any), 
    //     Flavor(translatable);

    setQualifier(nameSpace, QualifierDecl("description", String(),
	Scope::ANY, 
	Flavor::TRANSLATABLE));

    // Qualifier DisplayName : string = null, 
    //     Scope(any), 
    //     Flavor(translatable);

    setQualifier(nameSpace, QualifierDecl("displayname", String(),
	Scope::ANY,
	Flavor::DEFAULTS | Flavor::TRANSLATABLE));

    // Qualifier Expensive : boolean = false,
    //     Scope(property, reference, method, class, association);

    setQualifier(nameSpace, QualifierDecl("expensive", false,
	Scope::PROPERTY | Scope::REFERENCE | Scope::METHOD | Scope::CLASS |
	Scope::ASSOCIATION));

    // Qualifier Gauge : boolean = false, 
    //     Scope(property, method, parameter);

    setQualifier(nameSpace, QualifierDecl("gauge", false,
	Scope::PROPERTY | Scope::METHOD | Scope::PARAMETER));

    // Qualifier Ifdeleted : boolean = false, 
    //     Scope(association, reference);

    setQualifier(nameSpace, QualifierDecl("ifdeleted", false,
	Scope::ASSOCIATION | Scope::REFERENCE));

    // Qualifier In : boolean = true, 
    //     Scope(parameter), 
    //     Flavor(disableoverride);

    setQualifier(nameSpace, QualifierDecl("in", true,
	Scope::PARAMETER,
	Flavor::TOSUBCLASS));

    // Qualifier Indication : boolean = false, 
    //     Scope(class, indication),
    //     Flavor(disableoverride);

    setQualifier(nameSpace, QualifierDecl("indication", false,
	Scope::CLASS | Scope::INDICATION,
	Flavor::TOSUBCLASS));

    // Qualifier Invisible : boolean = false,
    //     Scope(reference, association, class, property, method);

    setQualifier(nameSpace, QualifierDecl("invisible", false,
	Scope::REFERENCE | Scope::ASSOCIATION | Scope::CLASS | Scope::PROPERTY |
	Scope::METHOD));

    // Qualifier Key : boolean = false, 
    //     Scope(property, reference),
    //     Flavor(disableoverride);

    setQualifier(nameSpace, QualifierDecl("key", false,
	Scope::PROPERTY | Scope::REFERENCE,
	Flavor::TOSUBCLASS));

    // Qualifier Large : boolean = false, 
    //     Scope(property, class);

    setQualifier(nameSpace, QualifierDecl("large", false,
	Scope::PROPERTY | Scope::CLASS));

    // Qualifier MappingStrings : string[],
    //     Scope(class, property, association, indication, reference);

    setQualifier(nameSpace, QualifierDecl("mappingstrings", Array<String>(),
	Scope::CLASS | Scope::PROPERTY | Scope::ASSOCIATION | 
	Scope::INDICATION | Scope::REFERENCE));

    // Qualifier Max : uint32 = null, Scope(reference);

    setQualifier(nameSpace, QualifierDecl("max", Sint32(0),
	Scope::PROPERTY | Scope::REFERENCE));

    // Qualifier MaxLen : uint32 = null, 
    //     Scope(property, method, parameter);

#if 0
    setQualifier(nameSpace, QualifierDecl("maxlen", Uint32(0),
	Scope::PROPERTY | Scope::METHOD | Scope::PARAMETER));
#else
    setQualifier(nameSpace, QualifierDecl("maxlen", Sint32(0),
	Scope::PROPERTY | Scope::METHOD | Scope::PARAMETER));
#endif

    // Qualifier MaxValue : sint64 = null, 
    //     Scope(property, method, parameter);
    // ATTN: XML schema requires sint32!

    setQualifier(nameSpace, QualifierDecl("maxvalue", Sint32(0),
	Scope::PROPERTY | Scope::METHOD | Scope::PARAMETER));
    
    // Qualifier Min : sint32 = null, Scope(reference);

    setQualifier(nameSpace, QualifierDecl("min", Sint32(0),
	Scope::REFERENCE));

    // Qualifier MinValue : sint64 = null, 
    //     Scope(property, method, parameter);
    // ATTN: Type expected by XML spec is sint32!

    setQualifier(nameSpace, QualifierDecl("minvalue", Sint32(0),
	Scope::PROPERTY | Scope::METHOD | Scope::PARAMETER));

    // Qualifier ModelCorrespondence : string[], 
    //     Scope(property);

    setQualifier(nameSpace, QualifierDecl("modelcorrespondence", 
	Array<String>(),
	Scope::PROPERTY));

    // Qualifier NonLocal : string = null, 
    //     Scope(reference);

    setQualifier(nameSpace, QualifierDecl("nonlocal", String(),
	Scope::REFERENCE));

    // Qualifier NullValue : string = null, 
    //     Scope(property),
    //     Flavor(tosubclass, disableoverride);

    setQualifier(nameSpace, QualifierDecl("nullvalue", String(),
	Scope::PROPERTY,
	Flavor::TOSUBCLASS));

    // Qualifier Out : boolean = false, 
    //     Scope(parameter), 
    //     Flavor(disableoverride);

    setQualifier(nameSpace, QualifierDecl("out", false,
	Scope::PARAMETER,
	Flavor::TOSUBCLASS));

    // Qualifier Override : string = null, 
    //     Scope(property, method, reference),
    //     Flavor(disableoverride);

    setQualifier(nameSpace, QualifierDecl("override", String(),
	Scope::PROPERTY | Scope::METHOD | Scope::REFERENCE,
	Flavor::TOSUBCLASS));

    // Qualifier Propagated : string = null, 
    //     Scope(property, reference),
    //     Flavor(disableoverride);

    setQualifier(nameSpace, QualifierDecl("propagated", String(),
	Scope::PROPERTY | Scope::REFERENCE,
	Flavor::TOSUBCLASS));

    // Qualifier Provider : string = null, 
    //     Scope(any);

    setQualifier(nameSpace, QualifierDecl("provider", String(),
	Scope::ANY));

    // Qualifier Read : boolean = true, Scope(property);

    setQualifier(nameSpace, QualifierDecl("read", true,
	Scope::PROPERTY));

    // Qualifier Required : boolean = false, 
    //     Scope(property);

    setQualifier(nameSpace, QualifierDecl("required", false,
	Scope::PROPERTY));

    // Qualifier Revision : string = null,
    //     Scope(schema, class, association, indication),
    //     Flavor(translatable);
    // ATTN: No such scope as Scope::SCHEMA

    setQualifier(nameSpace, QualifierDecl("revision", String(),
	Scope::CLASS | Scope::ASSOCIATION | Scope::INDICATION,
	Flavor::DEFAULTS | Flavor::TRANSLATABLE));

    // Qualifier Schema : string = null, 
    //     Scope(property, method),
    //     Flavor(disableoverride, translatable);

    setQualifier(nameSpace, QualifierDecl("schema", String(),
	Scope::PROPERTY | Scope::METHOD,
	Flavor::TOSUBCLASS | Flavor::TRANSLATABLE));

    // Qualifier Source : string = null, 
    //     Scope(class, association, indication);

    setQualifier(nameSpace, QualifierDecl("source", String(),
	Scope::CLASS | Scope::ASSOCIATION | Scope::INDICATION));

    // Qualifier SourceType : string = null,
    //     Scope(class, association, indication,reference);

    setQualifier(nameSpace, QualifierDecl("sourcetype", String(),
	Scope::CLASS | Scope::ASSOCIATION | Scope:: INDICATION |
	Scope::REFERENCE));
    
    // Qualifier Static : boolean = false,
    //     Scope(property, method), Flavor(disableoverride);

    setQualifier(nameSpace, QualifierDecl("static", false,
	Scope::PROPERTY | Scope::METHOD,
	Flavor::TOSUBCLASS));

    // Qualifier Syntax : string = null,
    //     Scope(property, reference, method, parameter);

    setQualifier(nameSpace, QualifierDecl("syntax", String(),
	Scope::PROPERTY | Scope::REFERENCE | Scope::METHOD | Scope::PARAMETER));

    // Qualifier SyntaxType : string = null,
    //     Scope(property, reference, method, parameter);

    setQualifier(nameSpace, QualifierDecl("syntaxtype", String(),
	Scope::PROPERTY | Scope::REFERENCE | Scope::METHOD | Scope::PARAMETER));

    // Qualifier Terminal : boolean = false, 
    //     Scope(class);

    setQualifier(nameSpace, QualifierDecl("terminal", false,
	Scope::CLASS));

    // Qualifier TriggerType : string = null,
    //     Scope(class, property, reference, method, association, indication);

    setQualifier(nameSpace, QualifierDecl("triggertype", String(),
	Scope::CLASS | Scope::PROPERTY | Scope::REFERENCE | Scope::METHOD |
	Scope::ASSOCIATION | Scope::INDICATION));

    // Qualifier Units : string = null, 
    //     Scope(property, method, parameter),
    //     Flavor(translatable);

    setQualifier(nameSpace, QualifierDecl("units", String(),
	Scope::PROPERTY | Scope::METHOD | Scope::PARAMETER,
	Flavor::DEFAULTS | Flavor::TRANSLATABLE));

    // Qualifier UnknownValues : string[], 
    //     Scope(property), 
    //     Flavor(disableoverride, tosubclass);

    setQualifier(nameSpace, QualifierDecl("unknownvalues", Array<String>(),
	Scope::PROPERTY,
	Flavor::TOSUBCLASS));

    // Qualifier UnsupportedValues : string[], 
    //     Scope(property),
    //     Flavor(disableoverride, tosubclass);

    setQualifier(nameSpace, QualifierDecl("unsupportedvalues", Array<String>(),
	Scope::PROPERTY,
	Flavor::TOSUBCLASS));

    // Qualifier ValueMap : string[], 
    //     Scope(property, method, parameter);

    setQualifier(nameSpace, QualifierDecl("valuemap", Array<String>(),
	Scope::PROPERTY | Scope::METHOD | Scope::PARAMETER));

    // Qualifier Values : string[], 
    //     Scope(property, method, parameter),
    //     Flavor(translatable);

    setQualifier(nameSpace, QualifierDecl("values", Array<String>(),
	Scope::PROPERTY | Scope::METHOD | Scope::PARAMETER,
	Flavor::DEFAULTS | Flavor::TRANSLATABLE));

    // Qualifier Version : string = null,
    //     Scope(schema, class, association, indication), 
    //     Flavor(translatable);
    // ATTN: No such scope as Scope::SCHEMA

    setQualifier(nameSpace, QualifierDecl("version", String(),
	Scope::CLASS | Scope::ASSOCIATION | Scope::INDICATION,
	Flavor::DEFAULTS | Flavor::TRANSLATABLE));

    // Qualifier Weak : boolean = false, 
    //     Scope(reference),
    //     Flavor(disableoverride, tosubclass);

    setQualifier(nameSpace, QualifierDecl("weak", false,
	Scope::REFERENCE,
	Flavor::TOSUBCLASS));

    // Qualifier Write : boolean = false, 
    //     Scope(property);

    setQualifier(nameSpace, QualifierDecl("write", false,
	Scope::PROPERTY));
}

PEGASUS_NAMESPACE_END
