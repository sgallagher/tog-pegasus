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

#include <fstream>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <Pegasus/Common/Destroyer.h>
#include <Pegasus/Common/FileSystem.h>
#include "InstanceIndexFile.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

//------------------------------------------------------------------------------
//
// _GetLine()
//
// 	Gets the next line of the file:
//
//------------------------------------------------------------------------------

static Boolean _GetLine(istream& is, Array<char>& x)
{
    x.clear();
    x.reserve(1024);

    char c;

    while (is.get(c) && c != '\n')
	x.append(c);

    x.append('\0');

    return is ? true : false;
}

//------------------------------------------------------------------------------
//
// _GetNextRecord()
//
// 	Gets the next record in the index file.
//
//------------------------------------------------------------------------------

static Boolean _GetNextRecord(
    istream& is, 
    Array<char>& line,
    Uint32& hashCode,
    const char*& objectName,
    Uint32& index,
    Boolean& error)
{
    error = false;

    // -- Get the next line:

    if (!_GetLine(is, line))
	return false;

    // -- Get the hash-code:

    char* end = 0;
    hashCode = strtoul(line.getData(), &end, 16);

    if (!end)
    {
	error = true;
	return false;
    }

    // -- Skip whitespace:

    while (*end && isspace(*end))
	end++;

    if (!*end)
	return false;

    // -- Get instance name:

    objectName = end;
    char* sep = strrchr(objectName, ' ');

    if (!sep)
    {
	error = true;
	return false;
    }

    *sep = '\0';

    // -- Get index:

    const char* indexString = sep + 1;

    end = 0;
    index = strtoul(indexString, &end, 10);

    if (!end || *end != '\0')
    {
	error = false;
	return false;
    }

    return true;
}

//------------------------------------------------------------------------------
//
// InstanceIndexFile::lookup()
//
//------------------------------------------------------------------------------

Boolean InstanceIndexFile::lookup(
    const String& path, 
    const CIMReference& instanceName,
    Uint32& indexOut)
{
    indexOut = Uint32(-1);

    String realPath;

    if (!FileSystem::existsNoCase(path, realPath))
	return false;

    ArrayDestroyer<char> p(realPath.allocateCString());

    ifstream is(p.getPointer());

    if (is)
    {
	Uint32 targetHashCode = instanceName.makeHashCode();
	Array<char> line;
	Uint32 hashCode;
	const char* objectName;
	Uint32 index;
	Boolean error;


	while (_GetNextRecord(is, line, hashCode, objectName, index, error))
	{
	    if (hashCode == targetHashCode &&
		CIMReference(objectName) == instanceName)
	    {
		indexOut = index;
		return true;
	    }
	}
    }

    return false;
}

//------------------------------------------------------------------------------
//
// InstanceIndexFile::insert()
//
//------------------------------------------------------------------------------

Boolean InstanceIndexFile::insert(
    const String& path, 
    const CIMReference& instanceName,
    Uint32& indexOut)
{
    //--------------------------------------------------------------------------
    // Search index file for next available index and check to see if the
    // instance name already occurs in the file.
    //--------------------------------------------------------------------------

    Uint32 newIndex = Uint32(-1);
    ArrayDestroyer<char> p(path.allocateCString());
    ifstream is(p.getPointer());
    Uint32 targetHashCode = instanceName.makeHashCode();

    if (is)
    {
	Array<Uint8> used;

	used.reserve(1024);
	Array<char> line;
	Uint32 hashCode;
	const char* objectName;
	Uint32 index;
	Boolean error;

	while (_GetNextRecord(is, line, hashCode, objectName, index, error))
	{
	    if (targetHashCode == hashCode &&
		CIMReference(objectName) == instanceName)
	    {
		indexOut = Uint32(-1);
		return false;
	    }

	    if (used.size() < index)
		used.grow(index, '\0');

	    used[index-1] = true;
	}

	if (error)
	{
	    indexOut = Uint32(-1);
	    return false;
	}

	// Find the first unused newIndex:

	newIndex = Uint32(-1);

	for (Uint32 i = 0, n = used.size(); i < n; i++)
	{
	    if (!used[i])
	    {
		newIndex = i + 1;
		break;
	    }
	}

	if (newIndex == Uint32(-1))
	    newIndex = used.size() + 1;

    }
    else
	newIndex = 1;

    is.close();

    //--------------------------------------------------------------------------
    // Append the new instance to the file:
    //--------------------------------------------------------------------------

    ofstream os(p.getPointer(), PEGASUS_STD(ios::app));

    char buffer[32];
    sprintf(buffer, "%08X", targetHashCode);
    os << buffer << ' ' << instanceName << ' ' << newIndex << endl;

    indexOut = newIndex;
    return true;
}

//------------------------------------------------------------------------------
//
// InstanceIndexFile::remove()
//
//------------------------------------------------------------------------------

Boolean InstanceIndexFile::remove(
    const String& path_, 
    const CIMReference& instanceName)
{
    // Get real name of file:

    String path;

    if (!FileSystem::existsNoCase(path_, path))
	return false;

    // Open output file:

    ArrayDestroyer<char> p(path.allocateCString(4));
    strcat(p.getPointer(), ".tmp");
    ofstream os(p.getPointer());

    if (!os)
	return false;

    // Open intput file:

    ArrayDestroyer<char> q(path.allocateCString());
    ifstream is(q.getPointer());

    if (!is)
	return false;

    // Copy all entries except the one specified:

    Uint32 targetHashCode = instanceName.makeHashCode();

    Boolean found = false;
    Array<char> line;
    Uint32 hashCode;
    const char* objectName;
    Uint32 index;
    Boolean error;

    while (_GetNextRecord(is, line, hashCode, objectName, index, error))
    {
	if (targetHashCode == hashCode &&
	    CIMReference(objectName) == instanceName)
	{
	    found = true;
	}
	else
	{
	    char buffer[32];
	    sprintf(buffer, "%08X", hashCode);
	    os << buffer << ' ' << objectName << ' ' << index << '\n';
	}
    }

    if (error)
	return false;

    os.flush();
    os.close();
    is.close();

    // Rename the file back:

    if (found)
    {
	if (!FileSystem::removeFile(q.getPointer()))
	    return false;

	if (!FileSystem::renameFile(p.getPointer(), q.getPointer()))
	    return false;
    }

    return found;
}

Boolean InstanceIndexFile::appendInstanceNamesTo(
    const String& path,
    Array<CIMReference>& instanceNames,
    Array<Uint32>& indices)
{
    // -- Open index file; return false if file doesn't exist:

    String realPath;

    if (!FileSystem::existsNoCase(path, realPath))
	return false;

    ArrayDestroyer<char> p(realPath.allocateCString());

    ifstream is(p.getPointer());

    if (!is)
	return false;

    // -- Build instance-names array:

    Array<char> line;
    Uint32 hashCode;
    const char* objectName;
    Uint32 index;
    Boolean error;

    while (_GetNextRecord(is, line, hashCode, objectName, index, error))
    {
	instanceNames.append(objectName);
	indices.append(index);
    }

    return true;
}

PEGASUS_NAMESPACE_END
