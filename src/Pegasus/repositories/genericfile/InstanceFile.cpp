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
// Author: Jenny Yu, Hewlett-Packard Company (jenny_yu@hp.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <fstream>
#include <Pegasus/Common/Destroyer.h>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/XmlWriter.h>
#include "InstanceFile.h"

// Note:  If the INDENT_XML_FILES option is set, the XmlWriter is called
//        to parse the xml data to add indentations before saving the 
//        instance data to the instance file.  It somewhat impacts the
//        performance of a create or modify instance operation.  In the
//        future, this option should be made a configurable property to
//        allow enabling and disabling without re-compilation.

#define INDENT_XML_FILES

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

/**
    Loads an instance record in the instance file to memory.
 */
Boolean InstanceFile::loadInstance(
    const String& path,
    Uint32 index,
    Uint32 size,
    Array<Sint8>& data)
{
    //
    // check for the existence of the instance file, and get the real path
    // of the file
    //
    String realPath;

    if (!FileSystem::existsNoCase(path, realPath))
    {
	return false;
    }

    //
    // load data from file
    //
    if (!_loadData(realPath, index, size, data))
    {
        return false;
    }
   
    return true;
}

/**
    Loads all the instance records in the instance file to memory.
 */
Boolean InstanceFile::loadAllInstances(
    const String& path,
    Array<Sint8>& data)
{
    //
    // check for the existence of the instance file, and get the real path
    // of the file
    //
    String realPath;

    if (!FileSystem::existsNoCase(path, realPath))
    {
	return false;
    }

    //
    // get the size of the instance file
    //
    Uint32 fileSize; 
    if (!FileSystem::getFileSizeNoCase(realPath, fileSize))
    {
        return false;
    }

    //
    // load all the instance data stored in the instance file
    //
    if (!_loadData(realPath, 0, fileSize, data))
    {
        return false;
    }

    return true;
}

/**
    Inserts a new record into the instance file.  Sets the byte position
    and the size of the newly added instance record.  Returns true on 
    success.

    This method creates a temporary file, copies the contents of the original
    instance file to the temporary file, and appends the new entry to the
    temporary file.  The caller must rename the temporary file back to the
    original file after the insert operation is successful on BOTH the
    instance index file and the instance file.
 */
Boolean InstanceFile::insertInstance(
    Array<Sint8> out,
    const String& path, 
    Uint32& index,
    Uint32& size)
{
    //
    // Create a temporary instance file
    //
    // First make sure that there is not already a temporary file.
    // If a temporary file already exists, remove it first. 
    //
    String tempFilePath;
    if (FileSystem::existsNoCase(path + ".tmp", tempFilePath))
    {
        if (!FileSystem::removeFileNoCase(tempFilePath))
        {
            return false;
        }
    }

    ArrayDestroyer<char> p(path.allocateCString(4));
    strcat(p.getPointer(), ".tmp");
    ofstream os(p.getPointer(), ios::app | ios::binary);

    if (!os)
    {
        return false;
    }

    //
    // Check for the existence of the instance file.  If file exists, copy
    // the contents of the file to the temporary file.
    //
    String realPath;
    if (FileSystem::existsNoCase(path, realPath))
    {
        ArrayDestroyer<char> p(realPath.allocateCString());
        ifstream is(p.getPointer(), ios::binary);

        if (!is)
        {
            return false;
        }

        //
        // get the size of the instance file
        //
        Uint32 fileSize; 
        if (!FileSystem::getFileSizeNoCase(realPath, fileSize))
        {
            return false;
        }

        char* buffer = new char[fileSize];
        is.clear();
        is.seekg(0);
        is.read(buffer, fileSize);

        if (is.fail())
            return false;

        os.write(buffer, fileSize);

        delete [] buffer;

        is.close();
    }

    //
    // append the new instance to the end of the file
    //
    if (!_insertData(out, os, index, size))
    {
        return false;
    }

    os.close();

    return true;
}

/**
    Removes an instance record from the instance file.

    This method creates a temporary file, then calls removeData() to remove
    the entry from the temporary file.  The caller must rename the temporary
    file back to the original file after the remove operation is successful
    on BOTH the instance index file and the instance file.
 */
Boolean InstanceFile::removeInstance(
    const String& path, 
    Uint32 size,
    Uint32 index)
{
    //
    // check for the existence of the instance file, and get the real path
    // of the file
    //
    String realPath;

    if (!FileSystem::existsNoCase(path, realPath))
    {
	return false;
    }

    //
    // Create a temporary instance index file
    //
    // First make sure that there is not already a temporary file.
    // If a temporary file already exists, remove it first.
    //
    String tempFilePath;
    if (FileSystem::existsNoCase(realPath + ".tmp", tempFilePath))
    {
        if (!FileSystem::removeFileNoCase(tempFilePath))
        {
            return false;
        }
    }

    ArrayDestroyer<char> p(realPath.allocateCString(4));
    strcat(p.getPointer(), ".tmp");
    ofstream os(p.getPointer(), ios::app | ios::binary);

    if (!os)
    {
        return false;
    }

    //
    // remove the entry from file
    //
    if (!_removeData(realPath, os, size, index))
    {
        return false;
    }

    os.close();

    return true;
}

/**
    Modifies an instance record in the instance file by first removing the
    the old instance record in the instance file, then appends the new 
    instance record to the instance file.  Returns the byte position and the
    size of the newly added instance record.

    This method creates a temporary file.  All updates are done to the 
    temporary file.  The caller must rename the temporary file back to the
    the original file after the modify operation is successful on BOTH the
    instance index file and the instance file.
 */
Boolean InstanceFile::modifyInstance(
    Array<Sint8> out,
    const String& path, 
    Uint32 oldIndex,
    Uint32 oldSize,
    Uint32& newIndex,
    Uint32& newSize)
{
    //
    // check for the existence of the instance file, and get the real path
    // of the file
    //
    String realPath;

    if (!FileSystem::existsNoCase(path, realPath))
    {
	return false;
    }

    //
    // Open a temporary instance file
    //
    // First make sure that there is not already a temporary file.
    // If a temporary file already exists, remove it first.
    //
    String tempFilePath;
    if (FileSystem::existsNoCase(realPath + ".tmp", tempFilePath))
    {
        if (!FileSystem::removeFileNoCase(tempFilePath))
        {
            return false;
        }
    }

    ArrayDestroyer<char> p(realPath.allocateCString(4));
    strcat(p.getPointer(), ".tmp");
    ofstream os(p.getPointer(), ios::app | ios::binary);

    if (!os)
    {
        return false;
    }

    //
    // remove the old entry from the temporary file
    //
    if (!_removeData(realPath, os, oldSize, oldIndex))
    {
        return false;
    }

    //
    // Append the new instance to the instance file
    //
    if (!_insertData(out, os, newIndex, newSize))
    {
        return false;
    }

    os.close();

    return true;
}

/**
    Loads data from the instance file at the given byte positon for the
    given size.
 */
Boolean InstanceFile::_loadData(
    const String& path,
    Uint32 index,
    Uint32 size,
    Array<Sint8>& data)
{
    //
    // open the instance file
    //
    ArrayDestroyer<char> p(path.allocateCString());
    ifstream is(p.getPointer(), ios::in | ios::binary);
   
    if (!is)
    {
	return false;
    }

    //
    // position the file get pointer at the specified location 
    //
    is.clear();               
    is.seekg(index); 

    //
    // Load data from file into memory
    //
    char* buffer = new char[size];
    is.read( buffer, size);

    if (is.fail())
        return false;

    data.clear();
    data.reserveCapacity(size+1);
    data.append(buffer, size);
    data.append('\0');

    is.close();

    delete [] buffer;

    return true;
}

/**
    Removes a record in the instance file at the given byte position for
    the given size.
 */
Boolean InstanceFile::_removeData(
    const String& realPath, 
    ofstream& os,
    Uint32 size,
    Uint32 index)
{
    //
    // Open the instance file 
    //
    ArrayDestroyer<char> q(realPath.allocateCString());
    ifstream is(q.getPointer(), ios::in | ios::binary);

    if (!is)
    {
        return false;
    }

    //
    // get the size of the instance file
    //
    Uint32 fileSize; 
    if (!FileSystem::getFileSizeNoCase(realPath, fileSize))
    {
        return false;
    }

    //
    // Copy all the entries in the instance file up to the one to be deleted
    //
    Array<Sint8> data;
    Uint32 copySize;
    Uint32 sizeNeeded;

    //
    //  determine the number of bytes to copy 
    //
    sizeNeeded = fileSize - size;

    //
    //  if the entry to be deleted is not the only entry in the instance file,
    //  copy the remaining entries 
    //
    if (sizeNeeded > 0)
    {
        char* buffer = new char[sizeNeeded];

        data.clear();
        data.reserveCapacity(sizeNeeded);

        is.clear();                 

        //
        // copy from the beginning of the file if the entry to be deleted 
        // is not the first entry in the file
        //
        if (index != 0)
        {
            is.seekg(0);                   
            is.read( buffer, index );

            if (is.fail())
                return false;

            data.append(buffer, index);
        }

        //
        // skip the entry to be deleted and copy the remaining entries 
        //
        Uint32 placeToCopy = index + size;

        is.seekg(placeToCopy);
        copySize = (fileSize - placeToCopy) - 1;

        is.read( buffer, copySize );

        if (is.fail())
            return false;

        data.append(buffer, copySize);
        data.append('\0');
    
        delete [] buffer;

        //
        // write data to the temporary file
        //
        os << (char*)data.getData() << endl;

        os.flush();
    }

    is.close();

    return true;
}

/**
    Inserts a new record into the instance file. 

 */
Boolean InstanceFile::_insertData(
    Array<Sint8> out,
    ofstream& os,
    Uint32& index,
    Uint32& size)
{
    Uint32 begPos;                 // file position before insertion 
    Uint32 endPos;                 // file position after insertion 

    //
    // determine current position of file
    //
    begPos = os.tellp();
    index = begPos;

    //
    // write the CIM/XML encoding of the instance to file
    //
#ifdef INDENT_XML_FILES
    out.append('\0');
    XmlWriter::indentedPrint(os, out.getData(), 2);
#else
    os.write((char*)out.getData(), out.size());
#endif

    //
    // determine the size of the instance record written to file
    //
    endPos = os.tellp();
    size = endPos - begPos;

    os.flush();

    return true;
}

PEGASUS_NAMESPACE_END
