//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software, Hewlett-Packard Company, IBM,
// The Open Group, Tivoli Systems
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
//      Gets the next line of the file:
//
//------------------------------------------------------------------------------

static Boolean _GetLine(istream& is, Array<char>& x)
{
    x.clear();
    x.reserveCapacity(1024);

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
//      Gets the next record in the index file.
//
//------------------------------------------------------------------------------

static Boolean _GetNextRecord(
    istream& is, 
    Array<char>& line,
    Uint32& hashCode,
    const char*& objectName,
    Uint32& index,
    Uint32& size,
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
    {
        error = true;
        return false;
    }

    // -- Get index:

    const char* indexString = end;

    end = 0;
    index = strtoul(indexString, &end, 10);

    if (!end)
    {
        error = true;
        return false;
    }

    // -- Skip whitespace:

    while (*end && isspace(*end))
        end++;

    if (!*end)
    {
        error = true;
        return false;
    }

    // -- Get size:
    
    const char* sizeString = end;

    end = 0;
    size = strtoul(sizeString, &end, 10);

    if (!end)
    {
        error = true;
        return false;
    }

    // -- Skip whitespace:

    while (*end && isspace(*end))
        end++;

    if (!*end)
    {
        error = true;
        return false;
    }

    // -- Get instance name:

    objectName = end;

    return true;
}

//------------------------------------------------------------------------------
//
// InstanceIndexFile::lookup()
//
//------------------------------------------------------------------------------

Boolean InstanceIndexFile::lookup(
    const String& path, 
    const CIMObjectPath& instanceName,
    Uint32& sizeOut,
    Uint32& indexOut)
{
    indexOut = Uint32(-1);

    String realPath;

    if (!FileSystem::existsNoCase(path, realPath))
        return false;

    ArrayDestroyer<char> p(realPath.allocateCString());
    ifstream is(p.getPointer(), ios::binary);

    if (is)
    {
        Uint32 targetHashCode = instanceName.makeHashCode();

        Array<char> line;
        Uint32 hashCode;
        const char* objectName;
        Uint32 size;
        Uint32 index;
        Boolean error;

        while (_GetNextRecord(is, line, hashCode, objectName, index, size,
                              error))
        {
            if (hashCode == targetHashCode &&
                CIMObjectPath(objectName) == instanceName)
            {
                indexOut = index;
                sizeOut = size;
                return true;
            }
        }

        if (error)
        {
            // ATTN:  This should only happen if the index file is corrupted,
            //        may want to log an error in the log file.
        }
    }

    return false;
}

//------------------------------------------------------------------------------
//
// InstanceIndexFile::insert()
//
// This method creates a temporary file, copies the contents of the original
// instance index file to the temporary file, and appends the new entry to
// the temporary file.  The caller must rename the temporary file back to 
// the original file after the insert operation is successful on BOTH the
// the instance index file and the instance file.
//
//------------------------------------------------------------------------------

Boolean InstanceIndexFile::insert(
    const String& path, 
    const CIMObjectPath& instanceName,
    Uint32 sizeIn,
    Uint32 indexIn)
{
    //
    // Create a temporary instance index file
    //
    // First make sure that there is not already a temporary file exists.
    // If a temporary file already exists, remove it first.
    //
    String tempFilePath;
    if (FileSystem::existsNoCase((path + ".tmp"), tempFilePath))
    {
        if (!FileSystem::removeFileNoCase(tempFilePath))
            return false;
    }

    ArrayDestroyer<char> p(path.allocateCString(4));
    strcat(p.getPointer(), ".tmp");
    ofstream os(p.getPointer(), ios::app | ios::binary);

    if (!os)
        return false;

    //
    // Check for the existence of the original instance index file.  If 
    // file exists, copy the contents of the file to the temporary file.
    //
    String realPath;
    if (FileSystem::existsNoCase(path, realPath))
    {
        ArrayDestroyer<char> p(realPath.allocateCString());
        ifstream is(p.getPointer(), ios::binary);

        if (!is)
            return false;

        //
        // get the size of the instance file
        //
        Uint32 fileSize;
        if (!FileSystem::getFileSizeNoCase(realPath, fileSize))
            return false;

        char* data = new char[fileSize];
        is.clear();
        is.seekg(0);
        is.read(data, fileSize);

        if (is.fail())
            return false;

        os.write(data, fileSize);

        delete [] data;

        is.close();
    }

    //
    // Append the new instance to the temporary instance index file:
    //
    _appendEntry(os, instanceName, sizeIn, indexIn);

    os.flush();
    os.close();

    return true;
}

//------------------------------------------------------------------------------
//
// InstanceIndexFile::remove()
//
// This method creates a temporary file, copies the contents of the original
// instance index file to the temporary file, and removes the entry from the
// the temporary file.  The caller must rename the temporary file back to the
// original file after the remove operation is successful on BOTH the 
// instance index file and the instance file.
//
//------------------------------------------------------------------------------

Boolean InstanceIndexFile::remove(
    const String& path_, 
    const CIMObjectPath& instanceName)
{
    //
    // Check for the existence of the instance index file and get the
    // real name of the file. 
    //
    String path;

    if (!FileSystem::existsNoCase(path_, path))
        return false;

    //
    // Create a temporary instance index file
    //
    // First make sure that there is not already a temporary file exists.
    // If a temporary file already exists, remove it first.
    //
    String tempFilePath;
    if (FileSystem::existsNoCase((path + ".tmp"), tempFilePath))
    {
        if (!FileSystem::removeFileNoCase(tempFilePath))
            return false;
    }

    ArrayDestroyer<char> p(path.allocateCString(4));
    strcat(p.getPointer(), ".tmp");
    ofstream os(p.getPointer(), ios::binary);

    if (!os)
    {
        return false;
    }

    //
    // remove the entry from file
    //
    if (!_removeEntry(os, path, instanceName))
    {
        return false;
    }

    os.flush();
    os.close();

    return true;
}

//------------------------------------------------------------------------------
//
// InstanceIndexFile::modify()
//
// This method creates a temporary file and copies the contents of the
// original instance index file to the temporary file.  Modifications 
// are done only to the temporary file.  The caller must rename the
// temporary file back to the original file after the modify operation 
// is successful on BOTH the instance index file and the instance file.
//
//------------------------------------------------------------------------------

Boolean InstanceIndexFile::modify(
    const String& path, 
    const CIMObjectPath& instanceName,
    Uint32 sizeIn,
    Uint32 indexIn)
{
    //
    // Check for the existence of the instance index file and get the
    // real name of the file. 
    //
    String realPath;

    if (!FileSystem::existsNoCase(path, realPath))
        return false;

    //
    // Create a temporary instance index file
    //
    // First make sure that there is not already a temporary file exists.
    // If a temporary file already exists, remove it first.
    //
    String tempFilePath;
    if (FileSystem::existsNoCase((path + ".tmp"), tempFilePath))
    {
        if (!FileSystem::removeFileNoCase(tempFilePath))
        {
            return false;
        }
    }

    ArrayDestroyer<char> p(realPath.allocateCString(4));
    strcat(p.getPointer(), ".tmp");
    ofstream os(p.getPointer(), ios::binary);

    if (!os)
    {
        return false;
    }

    //
    // remove the entry from file
    //
    if (!_removeEntry(os, realPath, instanceName))
    {
        return false;
    }

    //
    // Append the new instance to the temporary instance index file:
    //
    _appendEntry(os, instanceName, sizeIn, indexIn);

    os.flush();
    os.close();

    return true;
}

//------------------------------------------------------------------------------
//
// InstanceIndexFile::appendInstanceNamesTo()
//
// This method returns a list of all the instance names, as well as a list
// of the indices (or byte locations) of the instance records, and a list
// of the sizes of the instance records.
//
//------------------------------------------------------------------------------

Boolean InstanceIndexFile::appendInstanceNamesTo(
    const String& path,
    Array<CIMObjectPath>& instanceNames,
    Array<Uint32>& indices,
    Array<Uint32>& sizes)
{
    // -- Open index file and load the instance names

    String realPath;

    if (FileSystem::existsNoCase(path, realPath))
    {
        ArrayDestroyer<char> p(realPath.allocateCString());

        ifstream is(p.getPointer(), ios::binary);

        if (!is)
            return false;

        // -- Build instance-names array:

        Array<char> line;
        Uint32 hashCode;
        const char* objectName;
        Uint32 index;
        Uint32 size;
        Boolean error;

        while (_GetNextRecord(is, line, hashCode, objectName, index, size, 
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

//------------------------------------------------------------------------------
//
// InstanceIndexFile::_appendEntry()
//
// Appends a new entry to the temporary instance index file. 
//
//------------------------------------------------------------------------------

void InstanceIndexFile::_appendEntry(
    ofstream& os,
    const CIMObjectPath& instanceName,
    Uint32 sizeIn,
    Uint32 indexIn)
{
    //
    // Append the new instance to the temporary instance index file:
    //
    Uint32 targetHashCode = instanceName.makeHashCode();

    char buffer[32];
    sprintf(buffer, "%08X", targetHashCode);
    os << buffer << ' ' << indexIn << ' ' << sizeIn << ' ';
    os << instanceName << endl;
}

//------------------------------------------------------------------------------
//
// InstanceIndexFile::_removeEntry()
//
// Removes an entry from the temporary instance index file. 
//
//------------------------------------------------------------------------------

Boolean InstanceIndexFile::_removeEntry(
    ofstream& os,
    const String& path, 
    const CIMObjectPath& instanceName)
{
    //
    // Open the index file
    //
    ArrayDestroyer<char> q(path.allocateCString());
    ifstream is(q.getPointer(), ios::binary);

    if (!is)
    {
        return false;
    }

    //
    // Copy all entries except the one specified:
    //
    Uint32 targetHashCode = instanceName.makeHashCode();

    Boolean found = false;
    Array<char> line;
    Uint32 hashCode;
    const char* objectName;
    Uint32 size;
    Uint32 index;
    Boolean error;
    Uint32 deletedSize = 0;

    while (_GetNextRecord(is, line, hashCode, objectName, index, size, error))
    {
        if (targetHashCode == hashCode &&
            CIMObjectPath(objectName) == instanceName)
        {
            //
            // found the entry of the instance to be deleted, keep track 
            // of the size of the deleted record
            //
            found = true;
            deletedSize = size;
        }
        else
        {
            if (found)
            {
                //
                // Adjust the index of the subsequent records by subtracting
                // the size of the deleted record from the index.  This 
                // requires that the index file entries are in the same order 
                // as the instance records in the instance file.
                //
                index = index - deletedSize;
            }

            _appendEntry(os, objectName, size, index);
        }
    }

    is.close();

    if (error)
    {
        return false;
    }

    return found;
}

PEGASUS_NAMESPACE_END
