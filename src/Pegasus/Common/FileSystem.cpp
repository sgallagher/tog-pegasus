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
// $Log: FileSystem.cpp,v $
// Revision 1.16  2001/04/13 19:57:17  mike
// Fixed several memory leaks.
// Fixed build crash problem (caused by screwed up NT flags).
//
// Revision 1.15  2001/04/12 09:57:39  mike
// Post Channel Port to Linux
//
// Revision 1.13  2001/04/08 21:57:13  karl
// dir hier tested
//
// Revision 1.12  2001/04/08 20:31:55  mike
// Fixed
//
// Revision 1.11  2001/04/08 20:29:41  mike
// Added std:: before cout and endl
//
// Revision 1.10  2001/04/08 20:28:27  karl
// test
//
// Revision 1.9  2001/04/08 19:56:38  karl
// Test version
//
// Revision 1.8  2001/04/08 19:20:04  mike
// more TCP work
//
// Revision 1.7  2001/04/08 01:13:21  mike
// Changed "ConstCIM" to "CIMConst"
//
// Revision 1.5  2001/03/11 23:35:32  mike
// Ports to Linux
//
// Revision 1.3  2001/02/13 02:06:40  mike
// Added renameFile() method.
//
// Revision 1.2  2001/02/11 05:42:33  mike
// new
//
// Revision 1.1.1.1  2001/01/14 19:51:35  mike
// Pegasus import
//
//
//END_HISTORY

#include <iostream>
#include <cstdio>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/System.h>
#include "Destroyer.h"
#include "FileSystem.h"
#include "System.h"
#include "Dir.h"

using namespace std;

PEGASUS_NAMESPACE_BEGIN

// Clone the path as a C String but discard trailing slash if any:

static char* _clonePath(const String& path)
{
    char* p = path.allocateCString();

    if (!*p)
	return p;

    char* last = p + path.getLength() - 1;

    if (*last == '/')
	*last = '\0';

    return p;
}

Boolean FileSystem::exists(const String& path)
{
    ArrayDestroyer<char> p(_clonePath(path));
    return System::exists(p.getPointer());
}

Boolean FileSystem::getCurrentDirectory(String& path)
{
    path.clear();
    char tmp[4096];

    if (!System::getCurrentDirectory(tmp, sizeof(tmp) - 1))
	return false;

    path.append(tmp);
    return true;
}

Boolean FileSystem::existsIgnoreCase(const String& path, String& realPath)
{
    realPath.clear();
    ArrayDestroyer<char> destroyer(_clonePath(path));
    char* p = destroyer.getPointer();

    char* dirPath;
    char* fileName;
    char* slash = strrchr(p, '/');

    if (slash)
    {
	*slash = '\0';
	fileName = slash + 1;
	dirPath = p;

	if (*fileName == '\0')
	    return false;
    }
    else
    {
	fileName = p;
	dirPath = ".";
    }

    for (Dir dir(dirPath); dir.more(); dir.next())
    {
	if (CompareIgnoreCase(fileName, dir.getName()) == 0)
	{
	    if (strcmp(dirPath, ".") == 0)
		realPath = dir.getName();
	    else
	    {
		realPath = dirPath;
		realPath += '/';
		realPath += dir.getName();
	    }
	    return true;
	}
    }

    return false;
}

Boolean FileSystem::canRead(const String& path)
{
    ArrayDestroyer<char> p(_clonePath(path));
    return System::canRead(p.getPointer());
}

Boolean FileSystem::canWrite(const String& path)
{
    ArrayDestroyer<char> p(_clonePath(path));
    return System::canWrite(p.getPointer());
}

Boolean FileSystem::isDirectory(const String& path)
{
    ArrayDestroyer<char> p(_clonePath(path));
    return System::isDirectory(p.getPointer());
}

Boolean FileSystem::changeDirectory(const String& path)
{
    ArrayDestroyer<char> p(_clonePath(path));
    return System::changeDirectory(p.getPointer());
}

Boolean FileSystem::makeDirectory(const String& path)
{
    ArrayDestroyer<char> p(_clonePath(path));
    return System::makeDirectory(p.getPointer());
}

Boolean FileSystem::getFileSize(const String& path, Uint32& size)
{
    ArrayDestroyer<char> p(_clonePath(path));
    return System::getFileSize(p.getPointer(), size);
}

Boolean FileSystem::removeDirectory(const String& path)
{
    ArrayDestroyer<char> p(_clonePath(path));
    return System::removeDirectory(p.getPointer());
}

Boolean FileSystem::removeDirectoryHier(const String& path)
{
    Array<String> fileList;

    // Get contents of current directory

    if (!FileSystem::getDirectoryContents(path,fileList))
	return false;

    // for files-in-directory, delete or recall removedir

    for (Uint32 i = 0, n = fileList.getSize(); i < n; i++)
    {   
	String newPath = path;	 // extend path	to subdir
	newPath.append("/");
	newPath.append(fileList[i]);
	
	if (FileSystem::isDirectory(newPath))
	{
	    // Recall ourselves with extended path
	    if (!FileSystem::removeDirectoryHier(newPath))
		return false; 
	}

	else
	{
          if (!FileSystem::removeFile(newPath))
		return false;
	}
    }

    return removeDirectory(path);	
}

Boolean FileSystem::removeFile(const String& path)
{
    ArrayDestroyer<char> p(_clonePath(path));
    return System::removeFile(p.getPointer());
}

void FileSystem::loadFileToMemory(
    Array<Sint8>& array,
    const String& fileName)
{
    Uint32 fileSize;

    if (!getFileSize(fileName, fileSize))
	throw CannotOpenFile(fileName);

    char* tmp = fileName.allocateCString();
    FILE* fp = fopen(tmp, "rb");
    delete [] tmp;

    if (fp == NULL)
	throw CannotOpenFile(fileName);

    array.reserve(fileSize);
    char buffer[4096];
    size_t n;

    while ((n = fread(buffer, 1, sizeof(buffer), fp)) > 0)
        array.append(buffer, n);

    fclose(fp);
}

Boolean FileSystem::compare(
    const String& fileName1,
    const String& fileName2)
{
    Uint32 fileSize1;

    if (!getFileSize(fileName1, fileSize1))
	throw CannotOpenFile(fileName1);

    Uint32 fileSize2;

    if (!getFileSize(fileName2, fileSize2))
	throw CannotOpenFile(fileName2);

    if (fileSize1 != fileSize2)
	return false;

    char* tmp1 = fileName1.allocateCString();
    FILE* fp1 = fopen(tmp1, "rb");
    delete [] tmp1;

    if (fp1 == NULL)
	throw CannotOpenFile(fileName1);

    char* tmp2 = fileName2.allocateCString();
    FILE* fp2 = fopen(tmp2, "rb");
    delete [] tmp2;

    if (fp2 == NULL)
    {
	fclose(fp1);
	throw CannotOpenFile(fileName2);
    }

    int c1;
    int c2;

    while ((c1 = fgetc(fp1)) != EOF && (c2 = fgetc(fp2)) != EOF)
    {
	if (c1 != c2)
	{
	    fclose(fp1);
	    fclose(fp2);
	    return false;
	}
    }

    fclose(fp1);
    fclose(fp2);
    return true;
}

//
//  Get the file list in the directory into the
//  array of strings provided
//  @return The function should return false under these circumstances:
//
//
//  1. The directory does not exist.
//  2. The file exists but is not a directory.
//  3. The directory is inaccessible.
//
//
Boolean FileSystem::getDirectoryContents(
    const String& path,
    Array<String>& paths)
{
#if 0
    // This may be just extra fluff but added anyway
    if (!FileSystem::isDirectory(path))
	return false;
#endif
    
    paths.clear();

    try
    { 
	for (Dir dir(path); dir.more(); dir.next())
	{
	    String name = dir.getName();

	    if (String::equal(name, ".") || String::equal(name, ".."))
		continue;

	    paths.append(name);
	}
	return true;
    }

    // Catch the Dir exception
    catch(CannotOpenDirectory&)
    {
    	return false;
    }
}

Boolean FileSystem::renameFile(
    const String& oldFileName,
    const String& newFileName)
{
    ArrayDestroyer<char> p(oldFileName.allocateCString());
    ArrayDestroyer<char> q(newFileName.allocateCString());
    return System::renameFile(p.getPointer(), q.getPointer());
}

PEGASUS_NAMESPACE_END
