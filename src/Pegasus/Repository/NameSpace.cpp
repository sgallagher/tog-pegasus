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
#include "NameSpace.h"

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
    os << "=== NameSpace: " << _nameSpaceName << '\n';
    os << "_nameSpacePath: " << _nameSpacePath << '\n';
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

static Boolean _IsNameSpaceDir(const String& nameSpacePath)
{
    if (!FileSystem::isDirectory(nameSpacePath))
	return false;

    if (!FileSystem::isDirectory(nameSpacePath + "/classes"))
	return false;

    if (!FileSystem::isDirectory(nameSpacePath + "/instances"))
	return false;

    if (!FileSystem::isDirectory(nameSpacePath + "/qualifiers"))
	return false;

    return true;
}

NameSpaceManager::NameSpaceManager(const String& repositoryRoot)
    : _repositoryRoot(repositoryRoot)
{
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

	    NameSpace* nameSpace;
	    
	    try
	    {
		nameSpace = new NameSpace(nameSpacePath, nameSpaceName);
	    }
	    catch(Exception& e)
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
    delete _rep;
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

PEGASUS_NAMESPACE_END
