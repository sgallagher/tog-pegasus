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
//         Jenny Yu, Hewlett-Packard Company (jenny_yu@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <fstream>
#include <cctype>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <Pegasus/Common/Destroyer.h>
#include <Pegasus/Common/FileSystem.h>
#include "InstanceIndexFile.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

static const Uint32 _MAX_FREE_COUNT = 16;

////////////////////////////////////////////////////////////////////////////////
//
// Local routines:
//
////////////////////////////////////////////////////////////////////////////////

//
// Gets one line from the given file.
//

static Boolean _GetLine(fstream& fs, Array<char>& x)
{
    x.clear();
    x.reserve(1024);

    char c;

    while (fs.get(c) && c != '\n') 
        x.append(c);

    x.append('\0');

    return !!fs;
}

inline void _SkipWhitespace(char*& p)
{
    while (*p && isspace(*p))
        p++;
}

//
// Get an integer field from the character pointer and advance the
// pointer past the field.
//

Boolean _GetIntField(
    char*& ptr,
    Boolean& error,
    Uint32& value,
    int base)
{
    char* end = 0;
    value = strtoul(ptr, &end, base);

    if (!end)
    {
        error = true;
        return false;
    }

    _SkipWhitespace(end);

    if (!*end)
    {
        error = true;
        return false;
    }

    ptr = end;
    return true;
}

//
// Gets the next record in the index file.
//

static Boolean _GetNextRecord(
    fstream& fs, 
    Array<char>& line,
    Uint32& freeFlag,
    Uint32& hashCode,
    Uint32& index,
    Uint32& size,
    const char*& objectName,
    Boolean& error)
{
    error = false;

    //
    // Get next line:
    //

    if (!_GetLine(fs, line))
        return false;

    //
    // Get the free flag field:
    //

    char* end = (char*)line.getData();

    if (!_GetIntField(end, error, freeFlag, 10))
	return false;

    if (freeFlag != 0 && freeFlag != 1)
    {
	error = true;
	return false;
    }

    //
    // Get the hash-code field:
    //

    if (!_GetIntField(end, error, hashCode, 16))
	return false;

    //
    // Get index field:
    //

    if (!_GetIntField(end, error, index, 10))
	return false;

    //
    // Get size field:
    //

    if (!_GetIntField(end, error, size, 10))
	return false;

    //
    // Get instance name:
    //

    objectName = end;

    return true;
}

////////////////////////////////////////////////////////////////////////////////
//
// InstanceIndexFile:
//
////////////////////////////////////////////////////////////////////////////////

Boolean InstanceIndexFile::lookupEntry(
    const String& path, 
    const CIMReference& instanceName,
    Uint32& sizeOut,
    Uint32& indexOut)
{
    fstream fs;

    if (!_openFile(path, fs))
	return false;

    Uint32 entryOffset = 0;

    Boolean result = _lookupEntry(
	fs, instanceName, sizeOut, indexOut, entryOffset);

    fs.close();
    return result;
}

Boolean InstanceIndexFile::createEntry(
    const String& path, 
    const CIMReference& instanceName,
    Uint32 sizeIn,
    Uint32 indexIn)
{
    //
    // Open the file:
    //

    fstream fs;

    if (!_openFile(path, fs))
	return false;

    //
    // Return false if entry already exists:
    //

    Uint32 tmpSize;
    Uint32 tmpIndex;
    Uint32 tmpEntryOffset;

    if (InstanceIndexFile::_lookupEntry(
	fs, instanceName, tmpSize, tmpIndex, tmpEntryOffset))
	return false;

    //
    // Append the new entry to the end of the file:
    //

    if (!_appendEntry(fs, instanceName, sizeIn, indexIn))
	return false;

    //
    // Close the file:
    //

    fs.close();
    return true;
}

Boolean InstanceIndexFile::deleteEntry(
    const String& path, 
    const CIMReference& instanceName)
{
    //
    // Open the file:
    //

    fstream fs;

    if (!_openFile(path, fs))
	return false;

    //
    // Mark the entry as free:
    //

    if (!_markEntryFree(fs, instanceName))
	return false;

    //
    // Increment the free count:
    //

    Uint32 freeCount = 0;

    if (!_incrementFreeCount(fs, freeCount))
	return false;

    //
    // Close the file:
    //

    fs.close();
    return true;

    //
    // Compact if max free count reached:
    //

    if (freeCount == _MAX_FREE_COUNT)
	_compact(path);
}

Boolean InstanceIndexFile::modifyEntry(
    const String& path, 
    const CIMReference& instanceName,
    Uint32 sizeIn,
    Uint32 indexIn)
{
    //
    // Open the file:
    //

    fstream fs;

    if (!_openFile(path, fs))
	return false;

    //
    // Mark the entry as free:
    //

    if (!_markEntryFree(fs, instanceName))
	return false;

    //
    // Append new entry:
    //

    if (!_appendEntry(fs, instanceName, sizeIn, indexIn))
	return false;

    //
    // Increment the free count:
    //

    Uint32 freeCount = 0;

    if (!_incrementFreeCount(fs, freeCount))
	return false;

    //
    // Close the file:
    //

    fs.close();
    return true;

    //
    // Compact if max free count reached:
    //

    if (freeCount == _MAX_FREE_COUNT)
	_compact(path);

    return true;
}

#if 0

Boolean InstanceIndexFile::appendInstanceNamesTo(
    const String& path,
    Array<CIMReference>& instanceNames,
    Array<Uint32>& indices,
    Array<Uint32>& sizes)
{
    // -- Open index file and load the instance names

    String realPath;

    if (FileSystem::existsNoCase(path, realPath))
    {
        ArrayDestroyer<char> p(realPath.allocateCString());

        ifstream fs(p.getPointer(), ios::binary);

        if (!fs)
            return false;

        // -- Build instance-names array:

        Array<char> line;
        Uint32 hashCode;
        const char* objectName;
        Uint32 index;
        Uint32 size;
        Boolean error;

        while (_GetNextRecord(fs, line, hashCode, objectName, index, size, 
            error))
        {
            instanceNames.append(objectName);
            indices.append(index);
            sizes.append(size);
        }

        if (error)
            return false;
    }

    return true;
}

#endif

Boolean InstanceIndexFile::_incrementFreeCount(
    PEGASUS_STD(fstream)& fs,
    Uint32& freeCount)
{
    //
    // Position file pointer to beginning of file (where free count is
    // located) and read the current free count.
    //

    fs.seekg(0);
    char hexString[9];
    fs.read(hexString, 8);

    if (!fs)
	return false;

    hexString[8] = '\0';

    //
    // Convert hex string to integer:
    //

    char* end = 0;
    long tmp = strtol(hexString, &end, 16);

    if (!end || *end != '\0' || tmp < 0)
	return false;

    freeCount = Uint32(tmp);

    //
    // Increment and rewrite the free count:
    //

    sprintf(hexString, "%08X", ++freeCount);
    fs.seekg(0);
    fs.write(hexString, 8);

    return !!fs;
}

Boolean InstanceIndexFile::_openFile(
    const String& path, 
    PEGASUS_STD(fstream)& fs)
{
    const char ZERO_FREE_COUNT[] = "00000000\n";

    //
    // Open the file:
    //

    if (!FileSystem::openNoCase(fs, path, ios::binary | ios::in | ios::out))
    {
	//
	// File does not exist so create it:
	//

	ArrayDestroyer<char> p(path.allocateCString());
	fs.open(p.getPointer(), ios::binary | ios::out);

	if (!fs)
	    return false;

	fs.write(ZERO_FREE_COUNT, sizeof(ZERO_FREE_COUNT) - 1);
	fs.close();

	//
	// Reopen the file:
	//

	if (!FileSystem::openNoCase(fs, path, ios::binary | ios::in | ios::out))
	    return false;
    }

    //
    // Position the file pointer beyond the free count:
    //

    fs.seekg(sizeof(ZERO_FREE_COUNT) - 1);

    return true;
}

Boolean InstanceIndexFile::_appendEntry(
    PEGASUS_STD(fstream)& fs,
    const CIMReference& instanceName,
    Uint32 sizeIn,
    Uint32 indexIn)
{
    //
    // Position the file at the end:
    //

    fs.seekg(0, ios::end);

    if (!fs)
	return false;

    //
    // Write the entry:
    //

    Uint32 targetHashCode = instanceName.makeHashCode();

    char buffer[32];
    sprintf(buffer, "%08X", targetHashCode);

    fs << "0 " << buffer << ' ' << indexIn << ' ' << sizeIn << ' ';
    fs << instanceName << endl;

    return !!fs;
}

Boolean InstanceIndexFile::_markEntryFree(
    PEGASUS_STD(fstream)& fs,
    const CIMReference& instanceName)
{
    //
    // First look up the entry:
    //

    Uint32 size = 0;
    Uint32 index = 0;
    Uint32 entryOffset = 0;

    if (InstanceIndexFile::_lookupEntry(
	fs, instanceName, size, index, entryOffset))
	return false;

    //
    // Now mark the entry as free (change the first character of the entry
    // from a '0' to a '1').
    //

    fs.seekg(entryOffset);

    if (!fs)
	return false;

    fs.write("0", 1);

    return !!fs;
}

Boolean InstanceIndexFile::_lookupEntry(
    PEGASUS_STD(fstream)& fs,
    const CIMReference& instanceName,
    Uint32& sizeOut,
    Uint32& indexOut,
    Uint32& entryOffset)
{
    sizeOut = 0;
    indexOut = 0;
    entryOffset = 0;

    Uint32 targetHashCode = instanceName.makeHashCode();
    Array<char> line;
    Uint32 freeFlag;
    Uint32 hashCode;
    const char* objectName;
    Uint32 size;
    Uint32 index;
    Boolean error;

    entryOffset = fs.tellp();

    while (_GetNextRecord(
	fs, line, freeFlag, hashCode, index, size, objectName, error))
    {
	if (freeFlag == 0 &&
	    hashCode == targetHashCode &&
	    CIMReference(objectName) == instanceName)
	{
	    indexOut = index;
	    sizeOut = size;
	    return true;
	}

	entryOffset = fs.tellp();
    }

    return false;
}

void InstanceIndexFile::_compact(
    const String& path)
{

}

PEGASUS_NAMESPACE_END
