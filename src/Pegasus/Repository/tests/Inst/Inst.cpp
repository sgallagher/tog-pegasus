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
// $Log: Inst.cpp,v $
// Revision 1.1  2001/02/11 17:18:43  mike
// new
//
//
//END_HISTORY

#include <fstream>
#include <Pegasus/Repository/Repository.h>
#include <Pegasus/Common/Destroyer.h>

using namespace Pegasus;
using namespace std;

class InstanceIndexFile
{
public:
    
    static Uint32 lookupIndex(
	const String& path, 
	const String& instanceName);

    static Uint32 insertEntry(
	const String& path, 
	const String& instanceName);
};

Uint32 InstanceIndexFile::lookupIndex(
    const String& path, 
    const String& instanceName)
{
    return 0;
}

Boolean GetLine(istream& is, Array<char>& x)
{
    x.clear();
    x.reserve(1024);

    char c;

    while (is.get(c) && c != '\n')
	x.append(c);

    x.append('\0');

    return is ? true : false;
}

struct CorruptInstanceIndexFile { };

Uint32 InstanceIndexFile::insertEntry(
    const String& path, 
    const String& instanceName)
{
    // First scan the file looking for the next available index.

    Uint32 index = Uint32(-1);

    Destroyer<char> p(path.allocateCString());

    ifstream is(p.getPointer());

    if (is)
    {
	Array<char> line;
	Array<Uint8> used;
	used.reserve(4096);

	while (GetLine(is, line))
	{
	    // cout << "line[" << line.getData() << "]" << endl;

	    // Find the space that separates the index from the instance
	    // name.

	    const char* instanceName = line.getData();
	    char* sep = strrchr(instanceName, ' ');

	    if (!sep)
		throw CorruptInstanceIndexFile();

	    *sep = '\0';
	    const char* indexString = sep + 1;

	    cout << "instanceName=[" << instanceName << "]" << endl;
	    // cout << "indexString=" << indexString << endl;

	    char* end = 0;
	    long tmpIndex = strtol(indexString, &end, 10);

	    if (!end || *end != '\0')
		throw CorruptInstanceIndexFile();

	    cout << "tmpIndex=" << tmpIndex << endl;

	    if (used.getSize() < tmpIndex)
		used.grow(tmpIndex, false);

	    used[tmpIndex-1] = true;
	}

	// Find the first unused index:

	index = Uint32(-1);

	for (Uint32 i = 0, n = used.getSize(); i < n; i++)
	{
	    if (!used[i])
	    {
		index = i + 1;
		break;
	    }
	}

	if (index == Uint32(-1))
	    index = used.getSize() + 1;

    }
    else
	index = 1;

    cout << "available index=" << index << endl;

    return 0;
}

int main(int argc, char** argv)
{
    try
    {
	InstanceIndexFile::insertEntry("X.idx", "X.key1=123");
    }
    catch (CorruptInstanceIndexFile&)
    {
	cerr << "Exception: CorruptInstanceIndexFile" << endl;
	exit(1);
    }
    return 0;
}
