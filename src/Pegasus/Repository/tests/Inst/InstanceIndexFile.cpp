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
// Revision 1.1  2001/02/13 01:28:35  mike
// new
//
//
//END_HISTORY

#include <fstream>
#include <Pegasus/Common/Destroyer.h>
#include "InstanceIndexFile.h"

using namespace std;

PEGASUS_NAMESPACE_BEGIN

Uint32 InstanceIndexFile::lookupIndex(
    const String& path, 
    const String& instanceName)
{
    return 0;
}

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
    Uint32& indexOut)
{
    Array<char> line;

    if (!_GetLine(is, line))
	return false;

    const char* instanceName = line.getData();
    char* sep = strrchr(instanceName, ' ');

    if (!sep)
	throw CorruptInstanceIndexFile();

    *sep = '\0';
    const char* indexString = sep + 1;

    char* end = 0;
    long tmpIndex = strtol(indexString, &end, 10);

    if (!end || *end != '\0')
	throw CorruptInstanceIndexFile();

    instanceNameOut = instanceName;
    indexOut = tmpIndex;
    return true;
}

//------------------------------------------------------------------------------
//
// InstanceIndexFile::insertEntry()
//
//------------------------------------------------------------------------------

Uint32 InstanceIndexFile::insertEntry(
    const String& path, 
    const String& instanceName)
{
    // ATTN: keys are assumed to be in the same order in the instance name.
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
	Array<Uint8> used;
	used.reserve(1024);

	while (_GetNextLine(is, tmpInstanceName, index))
	{
	    if (instanceName == tmpInstanceName)
		throw InstanceAlreadyExists();

	    if (used.getSize() < index)
		used.grow(index, false);

	    used[index-1] = true;
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

    return newIndex;
}

PEGASUS_NAMESPACE_END
