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
// $Log: InstanceIndexFile.cpp,v $
// Revision 1.3  2001/02/26 04:33:30  mike
// Fixed many places where cim names were be compared with operator==(String,String).
// Changed all of these to use CIMName::equal()
//
// Revision 1.2  2001/02/17 00:34:53  bob
// Added <cstdlib> for strtol on linux
//
// Revision 1.1  2001/02/13 07:08:06  mike
// new
//
// Revision 1.2  2001/02/13 02:12:47  mike
// new
//
// Revision 1.1  2001/02/13 01:28:35  mike
// new
//
//
//END_HISTORY

#include <fstream>
#include <Pegasus/Common/Destroyer.h>
#include <Pegasus/Common/FileSystem.h>
#include <cstdlib>    // for strtol on linux
#include "InstanceIndexFile.h"

using namespace std;

PEGASUS_NAMESPACE_BEGIN

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

static Boolean _GetNextLine(
    istream& is, 
    String& instanceNameOut,
    Uint32& indexOut,
    Boolean& error)
{
    error = false;
    Array<char> line;

    if (!_GetLine(is, line))
	return false;

    const char* instanceName = line.getData();
    char* sep = strrchr(instanceName, ' ');

    if (!sep)
    {
	error = true;
	return false;
    }

    *sep = '\0';
    const char* indexString = sep + 1;

    char* end = 0;
    long tmpIndex = strtol(indexString, &end, 10);

    if (!end || *end != '\0')
    {
	error = false;
	return false;
    }

    instanceNameOut = instanceName;
    indexOut = tmpIndex;
    return true;
}

//------------------------------------------------------------------------------
//
// InstanceIndexFile::insert()
//
//------------------------------------------------------------------------------

Boolean InstanceIndexFile::lookup(
    const String& path, 
    const String& instanceName,
    Uint32& indexOut)
{
    indexOut = Uint32(-1);
    Destroyer<char> p(path.allocateCString());
    ifstream is(p.getPointer());

    if (is)
    {
	String tmpInstanceName;
	Uint32 index;
	Boolean error;

	while (_GetNextLine(is, tmpInstanceName, index, error))
	{
	    if (String::equal(instanceName, tmpInstanceName))
	    {
		indexOut = index;
		return true;
	    }
	}
    }

    // Not found:

    return false;
}

//------------------------------------------------------------------------------
//
// InstanceIndexFile::insert()
//
//------------------------------------------------------------------------------

Boolean InstanceIndexFile::insert(
    const String& path, 
    const String& instanceName,
    Uint32& indexOut)
{
    // ATTN: keys are assumed to be in the same order in the instance names.
    // Otherwise, the search will fail. The caller must sort the key order
    // prior to calling this routine.

    //--------------------------------------------------------------------------
    // Search index file for next available index and check to see if the
    // instance name already occurs in the file.
    //--------------------------------------------------------------------------

    Uint32 newIndex = Uint32(-1);
    Destroyer<char> p(path.allocateCString());
    ifstream is(p.getPointer());

    if (is)
    {
	String tmpInstanceName;
	Uint32 index;
	Boolean error;
	Array<Uint8> used;
	used.reserve(1024);

	while (_GetNextLine(is, tmpInstanceName, index, error))
	{
	    if (String::equal(instanceName, tmpInstanceName))
	    {
		indexOut = Uint32(-1);
		return false;
	    }

	    if (used.getSize() < index)
		used.grow(index, false);

	    used[index-1] = true;
	}

	if (error)
	{
	    indexOut = Uint32(-1);
	    return false;
	}

	// Find the first unused newIndex:

	newIndex = Uint32(-1);

	for (Uint32 i = 0, n = used.getSize(); i < n; i++)
	{
	    if (!used[i])
	    {
		newIndex = i + 1;
		break;
	    }
	}

	if (newIndex == Uint32(-1))
	    newIndex = used.getSize() + 1;
    }
    else
	newIndex = 1;

    is.close();

    //--------------------------------------------------------------------------
    // Append the new instance to the file:
    //--------------------------------------------------------------------------

    ofstream os(p.getPointer(), std::ios::app);
    os << instanceName << ' ' << newIndex << endl;

    indexOut = newIndex;
    return true;
}

//------------------------------------------------------------------------------
//
// InstanceIndexFile::remove()
//
//------------------------------------------------------------------------------

Boolean InstanceIndexFile::remove(
    const String& path, 
    const String& instanceName)
{
    // Open output file:

    Destroyer<char> p(path.allocateCString(4));
    strcat(p.getPointer(), ".tmp");
    ofstream os(p.getPointer());

    if (!os)
	return false;

    // Open intput file:

    Destroyer<char> q(path.allocateCString());
    ifstream is(q.getPointer());

    if (!is)
	return false;

    // Copy all entries except the one specified:
	
    Boolean found = false;
    String tmpInstanceName;
    Uint32 index;
    Boolean error;

    while (_GetNextLine(is, tmpInstanceName, index, error))
    {
	if (String::equal(instanceName, tmpInstanceName))
	    found = true;
	else
	    os << tmpInstanceName << ' ' << index << '\n';
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

PEGASUS_NAMESPACE_END
