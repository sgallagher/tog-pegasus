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
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/HashTable.h>
#include <Pegasus/Common/Dir.h>
#include <Pegasus/Common/Tracer.h>
#include "NameSpaceManager.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

static char _CLASSES_DIR[] = "classes";
static char _INSTANCES_DIR[] = "instances";
static char _QUALIFIERS_DIR[] = "qualifiers";

static char _CLASSES_SUFFIX[] = "/classes";
static char _INSTANCES_SUFFIX[] = "/instances";
static char _QUALIFIERS_SUFFIX[] = "/qualifiers";

////////////////////////////////////////////////////////////////////////////////
//
// _MakeClassFilePath()
//
////////////////////////////////////////////////////////////////////////////////

static inline String _MakeClassFilePath(
    const String& nameSpacePath,
    const String& className,
    const String& superClassName)
{
    if (superClassName.size())
    {
        String returnString(nameSpacePath);
        returnString.append(_CLASSES_SUFFIX);
        returnString.append('/');
        returnString.append(className);
        returnString.append('.');
        returnString.append(superClassName);
        return returnString;
    }
    else
    {
        String returnString(nameSpacePath);
        returnString.append(_CLASSES_SUFFIX);
        returnString.append('/');
        returnString.append(className);
        returnString.append(".#");
        return returnString;
    }
}

////////////////////////////////////////////////////////////////////////////////
//
// _MakeQualifierFilePath()
//
////////////////////////////////////////////////////////////////////////////////

static inline String _MakeQualifierFilePath(
    const String& nameSpacePath,
    const String& qualifierName)
{
    String returnString(nameSpacePath);
    returnString.append(_QUALIFIERS_SUFFIX);
    returnString.append('/');
    returnString.append(qualifierName);
    return returnString;
}

////////////////////////////////////////////////////////////////////////////////
//
// _MakeInstanceDataFileBase()
//
////////////////////////////////////////////////////////////////////////////////

static inline String _MakeInstanceDataFileBase(
    const String& nameSpacePath,
    const String& className)
{
    String returnString(nameSpacePath);
    returnString.append(_INSTANCES_SUFFIX);
    returnString.append('/');
    returnString.append(className);
    return returnString;
}

////////////////////////////////////////////////////////////////////////////////
//
// NameSpace
//
////////////////////////////////////////////////////////////////////////////////

class NameSpace
{
public:

    NameSpace(const String& nameSpacePath, const String& nameSpaceName);

    ~NameSpace();

    const String& getNameSpacePath() const { return _nameSpacePath; }

    const String& getNameSpaceName() const { return _nameSpaceName; }

    const String getClassFilePath(const String& className) const;

    const String getQualifierFilePath(const String& qualifierName) const;

    const String getInstanceDataFileBase(const String& className) const;

    InheritanceTree& getInheritanceTree() { return _inheritanceTree; }

    /** Print this namespace. */
    void print(PEGASUS_STD(ostream)& os) const;

private:

    InheritanceTree _inheritanceTree;
    String _nameSpacePath;
    String _nameSpaceName;
};

NameSpace::NameSpace(const String& nameSpacePath, const String& nameSpaceName)
    : _nameSpacePath(nameSpacePath), _nameSpaceName(nameSpaceName)
{
    _inheritanceTree.insertFromPath(nameSpacePath + "/classes");
}

NameSpace::~NameSpace()
{

}

const String NameSpace::getClassFilePath(const String& className) const
{
    String superClassName;

    if (!_inheritanceTree.getSuperClass(className, superClassName))
	throw PEGASUS_CIM_EXCEPTION(CIM_ERR_INVALID_CLASS, className);

    return _MakeClassFilePath(_nameSpacePath, className, superClassName);
}

const String NameSpace::getQualifierFilePath(const String& qualifierName) const
{
    return _MakeQualifierFilePath(_nameSpacePath, qualifierName);
}

const String NameSpace::getInstanceDataFileBase(const String& className) const
{
    return _MakeInstanceDataFileBase(_nameSpacePath, className);
}

void NameSpace::print(PEGASUS_STD(ostream)& os) const
{
    os << "=== NameSpace: " << _nameSpaceName << PEGASUS_STD(endl);
    os << "_nameSpacePath: " << _nameSpacePath << PEGASUS_STD(endl);
    _inheritanceTree.print(os);
}

////////////////////////////////////////////////////////////////////////////////
//
// NameSpaceManagerRep
//
////////////////////////////////////////////////////////////////////////////////

typedef HashTable<String,NameSpace*,EqualFunc<String>,HashFunc<String> > Table;

struct NameSpaceManagerRep
{
    Table table;
};

////////////////////////////////////////////////////////////////////////////////
//
// NameSpaceManager
//
////////////////////////////////////////////////////////////////////////////////

static Boolean _IsNameSpaceDir(const String& nameSpacePath)
{
    if (!FileSystem::isDirectory(nameSpacePath))
	return false;

    if (!FileSystem::isDirectory(nameSpacePath + _CLASSES_SUFFIX))
	return false;

    if (!FileSystem::isDirectory(nameSpacePath + _INSTANCES_SUFFIX))
	return false;

    if (!FileSystem::isDirectory(nameSpacePath + _QUALIFIERS_SUFFIX))
	return false;

    return true;
}

static void _CreateNameSpaceDirectories(const String& nameSpacePath)
{
    if (!FileSystem::makeDirectory(nameSpacePath))
	throw CannotCreateDirectory(nameSpacePath);

    String classesPath = nameSpacePath + _CLASSES_SUFFIX;
    String instancesPath = nameSpacePath + _INSTANCES_SUFFIX;
    String qualifiersPath = nameSpacePath + _QUALIFIERS_SUFFIX;

    if (!FileSystem::makeDirectory(classesPath))
	throw CannotCreateDirectory(classesPath);

    if (!FileSystem::makeDirectory(instancesPath))
	throw CannotCreateDirectory(instancesPath);

    if (!FileSystem::makeDirectory(qualifiersPath))
	throw CannotCreateDirectory(qualifiersPath);
}

static Boolean _NameSpaceDirHierIsEmpty(const String& nameSpacePath)
{
    for (Dir dir(nameSpacePath); dir.more(); dir.next())
    {
	const char* name = dir.getName();

	if (strcmp(name, ".") != 0 &&
	    strcmp(name, "..") != 0 &&
	    CompareNoCase(name, _CLASSES_DIR) != 0 &&
	    CompareNoCase(name, _INSTANCES_DIR) != 0 &&
	    CompareNoCase(name, _QUALIFIERS_DIR) != 0)
	{
	    return true;
	}
    }

    String classesPath = nameSpacePath + _CLASSES_SUFFIX;
    String instancesPath = nameSpacePath + _INSTANCES_SUFFIX;
    String qualifiersPath = nameSpacePath + _QUALIFIERS_SUFFIX;

    return
	FileSystem::isDirectoryEmpty(classesPath) &&
	FileSystem::isDirectoryEmpty(instancesPath) &&
	FileSystem::isDirectoryEmpty(qualifiersPath);
}

NameSpaceManager::NameSpaceManager(const String& repositoryRoot)
    : _repositoryRoot(repositoryRoot)
{
    // Create directory if does not already exist:

    if (!FileSystem::isDirectory(_repositoryRoot))
    {
	if (!FileSystem::makeDirectory(_repositoryRoot))
	    throw CannotCreateDirectory(_repositoryRoot);
    }

    _rep = new NameSpaceManagerRep;

    // Create a NameSpace object for each directory under repositoryRoot.
    // This will throw an exception if the directory does not exist:

    for (Dir dir(repositoryRoot); dir.more(); dir.next())
    {
	String dirName = dir.getName();

	if (dirName == ".." || dirName == ".")
	    continue;

	String nameSpacePath = repositoryRoot + "/" + dirName;

	if (_IsNameSpaceDir(nameSpacePath))
	{
	    String nameSpaceName = dirName;
	    nameSpaceName.translate('#', '/');

	    NameSpace* nameSpace = 0;
	
	    try
	    {
		nameSpace = new NameSpace(nameSpacePath, nameSpaceName);
	    }
	    catch (Exception& e)
	    {
		delete nameSpace;
		throw e;
	    }

	    _rep->table.insert(nameSpaceName, nameSpace);
	}
    }
}

NameSpaceManager::~NameSpaceManager()
{
    for (Table::Iterator i = _rep->table.start(); i; i++)
	delete i.value();

    delete _rep;
}

Boolean NameSpaceManager::nameSpaceExists(const String& nameSpaceName) const
{
    return _rep->table.contains(nameSpaceName);
}

void NameSpaceManager::createNameSpace(const String& nameSpaceName)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "NameSpaceManager::createNameSpace()");

    // Throw exception if namespace already exists:

    if (nameSpaceExists(nameSpaceName))
    {
        PEG_METHOD_EXIT();
	throw PEGASUS_CIM_EXCEPTION(CIM_ERR_ALREADY_EXISTS, nameSpaceName);
    }

    // Attempt to create all the namespace diretories:

    String nameSpaceDirName = nameSpaceName;
    nameSpaceDirName.translate('/', '#');
    String nameSpacePath = _repositoryRoot + "/" + nameSpaceDirName;

    _CreateNameSpaceDirectories(nameSpacePath);

    // Create NameSpace object and register it:

    NameSpace* nameSpace = 0;

    try
    {
	nameSpace = new NameSpace(nameSpacePath, nameSpaceName);
    }
    catch (Exception& e)
    {
	delete nameSpace;
        PEG_METHOD_EXIT();
	throw e;
    }

    _rep->table.insert(nameSpaceName, nameSpace);

    PEG_METHOD_EXIT();
}

void NameSpaceManager::deleteNameSpace(const String& nameSpaceName)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "NameSpaceManager::deleteNameSpace()");

    // If no such namespace:

    NameSpace* nameSpace = 0;

    if (!_rep->table.lookup(nameSpaceName, nameSpace))
    {
        PEG_METHOD_EXIT();
	throw PEGASUS_CIM_EXCEPTION(CIM_ERR_INVALID_NAMESPACE, nameSpaceName);
    }

    // Form namespace path:

    String nameSpaceDirName = nameSpaceName;
    nameSpaceDirName.translate('/', '#');
    String nameSpacePath = _repositoryRoot + "/" + nameSpaceDirName;

    // Delete the entire namespace directory hierarchy:

    if (!_NameSpaceDirHierIsEmpty(nameSpacePath))
    {
        PEG_METHOD_EXIT();
	throw NonEmptyNameSpace(nameSpaceName);
    }

    if (!FileSystem::removeDirectoryHier(nameSpacePath))
    {
        PEG_METHOD_EXIT();
	throw CannotRemoveDirectory(nameSpacePath);
    }

    // Remove and delete the namespace object:

    Boolean success = _rep->table.remove(nameSpaceName);
    PEGASUS_ASSERT(success);
    delete nameSpace;

    PEG_METHOD_EXIT();
}

void NameSpaceManager::getNameSpaceNames(Array<String>& nameSpaceNames) const
{
    nameSpaceNames.clear();

    for (Table::Iterator i = _rep->table.start(); i; i++)
	nameSpaceNames.append(i.key());
}

String NameSpaceManager::getClassFilePath(
    const String& nameSpaceName,
    const String& className) const
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "NameSpaceManager::getClassFilePath()");

    NameSpace* nameSpace = 0;

    if (!_rep->table.lookup(nameSpaceName, nameSpace))
    {
        PEG_METHOD_EXIT();
	throw PEGASUS_CIM_EXCEPTION(CIM_ERR_INVALID_NAMESPACE, nameSpaceName);
    }

    PEG_METHOD_EXIT();
    return nameSpace->getClassFilePath(className);
}

String NameSpaceManager::getInstanceDataFileBase(
    const String& nameSpaceName,
    const String& className) const
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "NameSpaceManager::getInstanceDataFileBase()");

    NameSpace* nameSpace = 0;

    if (!_rep->table.lookup(nameSpaceName, nameSpace))
    {
        PEG_METHOD_EXIT();
	throw PEGASUS_CIM_EXCEPTION(CIM_ERR_INVALID_NAMESPACE, nameSpaceName);
    }

    PEG_METHOD_EXIT();
    return nameSpace->getInstanceDataFileBase(className);
}

String NameSpaceManager::getQualifierFilePath(
    const String& nameSpaceName,
    const String& qualifierName) const
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "NameSpaceManager::getQualifierFilePath()");

    NameSpace* nameSpace = 0;

    if (!_rep->table.lookup(nameSpaceName, nameSpace))
    {
        PEG_METHOD_EXIT();
	throw PEGASUS_CIM_EXCEPTION(CIM_ERR_INVALID_NAMESPACE, nameSpaceName);
    }

    PEG_METHOD_EXIT();
    return nameSpace->getQualifierFilePath(qualifierName);
}

void NameSpaceManager::deleteClass(
    const String& nameSpaceName,
    const String& className) const
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "NameSpaceManager::deleteClass()");

    // -- Lookup NameSpace object:

    NameSpace* nameSpace = 0;

    if (!_rep->table.lookup(nameSpaceName, nameSpace))
    {
        PEG_METHOD_EXIT();
	throw PEGASUS_CIM_EXCEPTION(CIM_ERR_INVALID_NAMESPACE, nameSpaceName);
    }

    // -- Get path to class file:

    String classFilePath = nameSpace->getClassFilePath(className);

    // -- Remove the file from the inheritance tree:

    nameSpace->getInheritanceTree().remove(className);

    // -- Remove the file from disk:

    if (!FileSystem::removeFileNoCase(classFilePath))
    {
        PEG_METHOD_EXIT();
	throw CannotRemoveFile(classFilePath);
    }

    PEG_METHOD_EXIT();
}

void NameSpaceManager::print(PEGASUS_STD(ostream)& os) const
{
    for (Table::Iterator i = _rep->table.start(); i; i++)
    {
	NameSpace* nameSpace = i.value();
	nameSpace->print(os);
    }

    os << PEGASUS_STD(endl);
}

void NameSpaceManager::createClass(
    const String& nameSpaceName,
    const String& className,
    const String& superClassName,
    String& classFilePath)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "NameSpaceManager::createClass()");

    // -- Lookup namespace:

    NameSpace* nameSpace = 0;

    if (!_rep->table.lookup(nameSpaceName, nameSpace))
    {
        PEG_TRACE_STRING(TRC_REPOSITORY, Tracer::LEVEL4, "Invalid NameSpace.");
        PEG_METHOD_EXIT();
	throw PEGASUS_CIM_EXCEPTION(CIM_ERR_INVALID_NAMESPACE, nameSpaceName);
    }

    InheritanceTree& it = nameSpace->getInheritanceTree();

    // -- Be certain class doesn't already exist:

    if (it.containsClass(className))
    {
        PEG_TRACE_STRING(TRC_REPOSITORY, Tracer::LEVEL4, "Class already exists.");
        PEG_METHOD_EXIT();
	throw PEGASUS_CIM_EXCEPTION(CIM_ERR_ALREADY_EXISTS, className);
    }

    // -- Be certain superclass exists:

    if (superClassName.size() && !it.containsClass(superClassName))
    {
        PEG_TRACE_STRING(TRC_REPOSITORY, Tracer::LEVEL4, 
                         "SuperClass does not exists.");
        PEG_METHOD_EXIT();
	throw PEGASUS_CIM_EXCEPTION(CIM_ERR_INVALID_SUPERCLASS, superClassName);
    }

    // -- Insert the entry:

    it.insert(className, superClassName);

    // -- Build the path to the class:

    classFilePath = _MakeClassFilePath(
	nameSpace->getNameSpacePath(), className, superClassName);

    PEG_METHOD_EXIT();
}

void NameSpaceManager::checkModify(
    const String& nameSpaceName,
    const String& className,
    const String& superClassName,
    String& classFilePath)
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "NameSpaceManager::checkModify()");

    // -- Lookup namespace:

    NameSpace* nameSpace = 0;

    if (!_rep->table.lookup(nameSpaceName, nameSpace))
    {
        PEG_METHOD_EXIT();
	throw PEGASUS_CIM_EXCEPTION(CIM_ERR_INVALID_NAMESPACE, nameSpaceName);
    }

    InheritanceTree& it = nameSpace->getInheritanceTree();

    // -- Disallow changing of superclass:

    String oldSuperClassName;

    if (!it.getSuperClass(className, oldSuperClassName))
    {
        PEG_METHOD_EXIT();
	throw PEGASUS_CIM_EXCEPTION(CIM_ERR_INVALID_CLASS, className);
    }

    if (superClassName != oldSuperClassName)
    {
        PEG_METHOD_EXIT();
	throw PEGASUS_CIM_EXCEPTION(
	    CIM_ERR_FAILED, "attempt to change superclass");
    }

    // -- Disallow modification of class with subclasses:

    Boolean hasSubClasses;
    it.hasSubClasses(className, hasSubClasses);

    if (hasSubClasses)
    {
        PEG_METHOD_EXIT();
	throw PEGASUS_CIM_EXCEPTION(CIM_ERR_CLASS_HAS_CHILDREN, className);
    }

    // -- Build the path to the class:

    classFilePath = _MakeClassFilePath(
	nameSpace->getNameSpacePath(), className, superClassName);

    PEG_METHOD_EXIT();
}

void NameSpaceManager::getSubClassNames(
    const String& nameSpaceName,
    const String& className,
    Boolean deepInheritance,
    Array<String>& subClassNames) const
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "NameSpaceManager::getSubClassNames()");

    // -- Lookup namespace:

    NameSpace* nameSpace = 0;

    if (!_rep->table.lookup(nameSpaceName, nameSpace))
    {
        PEG_METHOD_EXIT();
	throw PEGASUS_CIM_EXCEPTION(CIM_ERR_INVALID_NAMESPACE, nameSpaceName);
    }

    InheritanceTree& it = nameSpace->getInheritanceTree();

    if (!it.getSubClassNames(className, deepInheritance, subClassNames))
    {
        PEG_METHOD_EXIT();
	throw PEGASUS_CIM_EXCEPTION(CIM_ERR_INVALID_CLASS, className);
    }

    PEG_METHOD_EXIT();
}

void NameSpaceManager::getSuperClassNames(
    const String& nameSpaceName,
    const String& className,
    Array<String>& subClassNames) const
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "NameSpaceManager::getSuperClassNames()");

    // -- Lookup namespace:

    NameSpace* nameSpace = 0;

    if (!_rep->table.lookup(nameSpaceName, nameSpace))
    {
        PEG_METHOD_EXIT();
	throw PEGASUS_CIM_EXCEPTION(CIM_ERR_INVALID_NAMESPACE, nameSpaceName);
    }

    InheritanceTree& it = nameSpace->getInheritanceTree();

    // -- Get names of all superclasses:

    if (!it.getSuperClassNames(className, subClassNames))
    {
        PEG_METHOD_EXIT();
	throw PEGASUS_CIM_EXCEPTION(CIM_ERR_INVALID_CLASS, className);
    }

    PEG_METHOD_EXIT();
}

String NameSpaceManager::getQualifiersRoot(const String& nameSpaceName) const
{
    PEG_METHOD_ENTER(TRC_REPOSITORY, "NameSpaceManager::getQualifiersRoot()");

    // -- Lookup namespace:

    NameSpace* nameSpace = 0;

    if (!_rep->table.lookup(nameSpaceName, nameSpace))
    {
        PEG_METHOD_EXIT();
	throw PEGASUS_CIM_EXCEPTION(CIM_ERR_INVALID_NAMESPACE, nameSpaceName);
    }

    PEG_METHOD_EXIT();
    return nameSpace->getNameSpacePath() + _QUALIFIERS_SUFFIX;
}

PEGASUS_NAMESPACE_END
