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

#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/HashTable.h>
#include <Pegasus/Common/Dir.h>
#include "NameSpaceManager.h"

using namespace std;

PEGASUS_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
//
// NameSpace
//
////////////////////////////////////////////////////////////////////////////////

NameSpace::NameSpace(const String& nameSpacePath, const String& nameSpaceName)
    : _nameSpacePath(nameSpacePath), _nameSpaceName(nameSpaceName)
{
    _inheritanceTree.insertFromPath(nameSpacePath + "/classes");
}

NameSpace::~NameSpace()
{

}

void NameSpace::print(std::ostream& os) const
{
    os << "HERE WE ARE" << std::endl;

    os << "=== NameSpace: " << _nameSpaceName << std::endl;
    os << "_nameSpacePath: " << _nameSpacePath << std::endl;
    _inheritanceTree.print(os);
}

////////////////////////////////////////////////////////////////////////////////
//
// NameSpaceManagerRep
//
////////////////////////////////////////////////////////////////////////////////

typedef HashTable<String, NameSpace*> Table;

struct NameSpaceManagerRep
{
    Table table;
};

////////////////////////////////////////////////////////////////////////////////
//
// NameSpaceManager
//
////////////////////////////////////////////////////////////////////////////////

static char _CLASSES_DIR[] = "classes";
static char _INSTANCES_DIR[] = "instances";
static char _QUALIFIERS_DIR[] = "qualifiers";

static char _CLASSES_SUFFIX[] = "/classes";
static char _INSTANCES_SUFFIX[] = "/instances";
static char _QUALIFIERS_SUFFIX[] = "/qualifiers";

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
	    CompareIgnoreCase(name, _CLASSES_DIR) != 0 &&
	    CompareIgnoreCase(name, _INSTANCES_DIR) != 0 &&
	    CompareIgnoreCase(name, _QUALIFIERS_DIR) != 0)
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
    // Throw exception if namespace already exists:

    if (nameSpaceExists(nameSpaceName))
	throw CIMException(CIMException::ALREADY_EXISTS);

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
	throw e;
    }

    _rep->table.insert(nameSpaceName, nameSpace);
}

void NameSpaceManager::deleteNameSpace(const String& nameSpaceName)
{
    // If no such namespace:

    NameSpace* nameSpace = 0;

    if (!_rep->table.lookup(nameSpaceName, nameSpace))
	throw CIMException(CIMException::INVALID_NAMESPACE);

    // Form namespace path:

    String nameSpaceDirName = nameSpaceName;
    nameSpaceDirName.translate('/', '#');
    String nameSpacePath = _repositoryRoot + "/" + nameSpaceDirName;

    // Delete the entire namespace directory hierarchy:

    if (!_NameSpaceDirHierIsEmpty(nameSpacePath))
	throw NonEmptyNameSpace(nameSpaceName);

    if (!FileSystem::removeDirectoryHier(nameSpacePath))
	throw FailedToRemoveDirectory(nameSpacePath);

    // Remove and delete the namespace object:

    Boolean success = _rep->table.remove(nameSpaceName);
    PEGASUS_ASSERT(success);
    delete nameSpace;
}

void NameSpaceManager::print(std::ostream& os) const
{
    for (Table::Iterator i = _rep->table.start(); i; i++)
    {
	NameSpace* nameSpace = i.value();
	nameSpace->print(os);
    }

    os << std::endl;
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
    // -- Lookup NameSpace object:

    NameSpace* nameSpace = 0;

    if (!_rep->table.lookup(nameSpaceName, nameSpace))
	throw CIMException(CIMException::INVALID_NAMESPACE);

    // -- Find class:

    if (!nameSpace->getInheritanceTree().containsClass(className))
	throw CIMException(CIMException::INVALID_CLASS);

    // -- Form the full name of the class file:

    return Cat(nameSpace->getNameSpacePath(), _CLASSES_SUFFIX, '/', className);
}

PEGASUS_NAMESPACE_END
