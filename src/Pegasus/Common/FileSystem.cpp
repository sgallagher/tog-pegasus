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
//         Ramnath Ravindran(Ramnath.Ravindran@compaq.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <cstdio>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/System.h>
#include "Destroyer.h"
#include "FileSystem.h"
#include "System.h"
#include "Dir.h"

PEGASUS_NAMESPACE_BEGIN

// Clone the path as a C String but discard trailing slash if any:

static CString _clonePath(const String& path)
{
    String clone = path;

    if (clone.size() && clone[clone.size()-1] == '/')
        clone.remove(clone.size()-1);

    return clone.getCString();
}

Boolean FileSystem::exists(const String& path)
{
    return System::exists(_clonePath(path));
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

Boolean FileSystem::existsNoCase(const String& path, String& realPath)
{
    realPath.clear();
    CString cpath = _clonePath(path);
    const char* p = cpath;

    const char* dirPath;
    const char* fileName;
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
	if (CompareNoCase(fileName, dir.getName()) == 0)
	{
	    if (strcmp(dirPath, ".") == 0)
		realPath = dir.getName();
	    else
	    {
		realPath = dirPath;
		realPath.append('/');
		realPath.append(dir.getName());
	    }
	    return true;
	}
    }

    return false;
}

Boolean FileSystem::canRead(const String& path)
{
    return System::canRead(_clonePath(path));
}

Boolean FileSystem::canWrite(const String& path)
{
    return System::canWrite(_clonePath(path));
}

Boolean FileSystem::getFileSize(const String& path, Uint32& size)
{
    return System::getFileSize(_clonePath(path), size);
}

Boolean FileSystem::removeFile(const String& path)
{
    return System::removeFile(_clonePath(path));
}

void FileSystem::loadFileToMemory(
    Array<Sint8>& array,
    const String& fileName)
{
    Uint32 fileSize;

    if (!getFileSize(fileName, fileSize))
	throw CannotOpenFile(fileName);

    FILE* fp = fopen(fileName.getCString(), "rb");

    if (fp == NULL)
	throw CannotOpenFile(fileName);

    array.reserveCapacity(fileSize);
    char buffer[4096];
    size_t n;

    while ((n = fread(buffer, 1, sizeof(buffer), fp)) > 0)
        array.append(buffer, n);

    fclose(fp);
}

Boolean FileSystem::compareFiles(
    const String& path1,
    const String& path2)
{
    Uint32 fileSize1;

    if (!getFileSize(path1, fileSize1))
	throw CannotOpenFile(path1);

    Uint32 fileSize2;

    if (!getFileSize(path2, fileSize2))
	throw CannotOpenFile(path2);

    if (fileSize1 != fileSize2)
	return false;

    FILE* fp1 = fopen(path1.getCString(), "rb");

    if (fp1 == NULL)
	throw CannotOpenFile(path1);

    FILE* fp2 = fopen(path2.getCString(), "rb");

    if (fp2 == NULL)
    {
	fclose(fp1);
	throw CannotOpenFile(path2);
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

Boolean FileSystem::renameFile(
    const String& oldPath,
    const String& newPath)
{
    return System::renameFile(oldPath.getCString(), newPath.getCString());
}

Boolean FileSystem::copyFile(
    const String& fromPath,
    const String& toPath)
{
    return System::copyFile(fromPath.getCString(), toPath.getCString());
}

Boolean FileSystem::openNoCase(PEGASUS_STD(ifstream)& is, const String& path)
{
    String realPath;

    if (!existsNoCase(path, realPath))
	return false;

    is.open(_clonePath(realPath) PEGASUS_IOS_BINARY);
    return !!is;
}

Boolean FileSystem::openNoCase(
    PEGASUS_STD(fstream)& fs, 
    const String& path,
    int mode)
{
    String realPath;

    if (!existsNoCase(path, realPath))
	return false;

    fs.open(_clonePath(realPath), mode);
    return !!fs;
}

Boolean FileSystem::isDirectory(const String& path)
{
    return System::isDirectory(_clonePath(path));
}

Boolean FileSystem::changeDirectory(const String& path)
{
    return System::changeDirectory(_clonePath(path));
}

Boolean FileSystem::makeDirectory(const String& path)
{
    return System::makeDirectory(_clonePath(path));
}

Boolean FileSystem::removeDirectory(const String& path)
{
    return System::removeDirectory(_clonePath(path));
}

Boolean FileSystem::removeDirectoryHier(const String& path)
{
    Array<String> fileList;

    // Get contents of current directory

    if (!FileSystem::getDirectoryContents(path,fileList))
	return false;

    // for files-in-directory, delete or recall removedir

    for (Uint32 i = 0, n = fileList.size(); i < n; i++)
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

Boolean FileSystem::isDirectoryEmpty(const String& path)
{
    for (Dir dir(path); dir.more(); dir.next())
    {
        const char* name = dir.getName();

        if (strcmp(name, ".") != 0 && strcmp(name, "..") != 0)
            return false;
    }

    return true;
}

void FileSystem::translateSlashes(String& path)
{
    for (Uint32 i = 0; i < path.size(); i++)
    {
	if (path[i] == '\\')
	    path[i] = '/';
    }
}

Boolean GetLine(PEGASUS_STD(istream)& is, String& line)
{
    line.clear();

    Boolean gotChar = false;
    char c;

    while (is.get(c))
    {
        gotChar = true;

        if (c == '\n')
            break;

        line.append(c);
    }

    return gotChar;
}

PEGASUS_NAMESPACE_END
