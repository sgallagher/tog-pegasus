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
// Revision 1.6  2001/04/07 12:01:18  karl
// remove namespace support
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

#include <Pegasus/Common/Config.h>

#ifdef PEGASUS_OS_TYPE_WINDOWS
# include <io.h>
# include <direct.h>
#else
# include <unistd.h>
# include <dirent.h>
#endif

#include <sys/stat.h>
#include <sys/types.h>
#include <cstdio>
#include "Destroyer.h"
#include "FileSystem.h"
#include "Dir.h"
//DEBUG ONLY
#include <iostream.h>

// ATTN-B: porting!

PEGASUS_NAMESPACE_BEGIN

#ifdef PEGASUS_OS_TYPE_WINDOWS
static const int ACCESS_EXISTS = 0;
static const int ACCESS_WRITE = 2;
static const int ACCESS_READ = 4;
static const int ACCESS_READ_AND_WRITE = 6;
#endif

/** Clone the path as a C String but discard 
    trailing slash if any:
*/
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

#ifdef PEGASUS_OS_TYPE_WINDOWS
    return _access(p.getPointer(), ACCESS_EXISTS) == 0;
#else
    return access(p.getPointer(), F_OK) == 0;
#endif
}

Boolean FileSystem::getCurrentDirectory(String& path)
{
#ifdef PEGASUS_OS_TYPE_WINDOWS
    char* tmp = _getcwd(NULL, 0);

    if (!tmp)
	return false;

    path.append(tmp);
    delete [] tmp;
#else
    char tmp[4096];

    getcwd(tmp, sizeof(tmp));
    path.append(tmp);
#endif
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
#ifdef PEGASUS_OS_TYPE_WINDOWS
	if (stricmp(fileName, dir.getName()) == 0)
#else
	if (strcasecmp(fileName, dir.getName()) == 0)
#endif
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

#ifdef PEGASUS_OS_TYPE_WINDOWS
    return _access(p.getPointer(), ACCESS_READ) == 0;
#else
    return access(p.getPointer(), R_OK) == 0;
#endif
}

Boolean FileSystem::canWrite(const String& path)
{
    ArrayDestroyer<char> p(_clonePath(path));

#ifdef PEGASUS_OS_TYPE_WINDOWS
    return _access(p.getPointer(), ACCESS_WRITE) == 0;
#else
    return access(p.getPointer(), W_OK) == 0;
#endif
}

#if 0
// ATTN: not implemented for NT. But not used by Pegasus.
Boolean FileSystem::canExecute(const String& path)
{
    ArrayDestroyer<char> p(_clonePath(path));
    return access(p.getPointer(), X_OK) == 0;
}
#endif

Boolean FileSystem::isDirectory(const String& path)
{
    ArrayDestroyer<char> p(_clonePath(path));

    struct stat st;

#ifdef PEGASUS_OS_TYPE_WINDOWS

    if (stat(p.getPointer(), &st) != 0)
	return false;

    Boolean result = (st.st_mode & _S_IFDIR) != 0;
    return result;

#else

    if (stat(p.getPointer(), &st) != 0)
	return false;

    Boolean result = S_ISDIR(st.st_mode);
    return result;

#endif
}

Boolean FileSystem::changeDirectory(const String& path)
{
    ArrayDestroyer<char> p(_clonePath(path));
    return chdir(p.getPointer()) == 0;
}

Boolean FileSystem::makeDirectory(const String& path)
{
    ArrayDestroyer<char> p(_clonePath(path));
#ifdef PEGASUS_OS_TYPE_WINDOWS
    return _mkdir(p.getPointer()) == 0;
#else
    return mkdir(p.getPointer(), 0777) == 0;
#endif
}

Boolean FileSystem::getFileSize(const String& path, Uint32& size)
{
    struct stat st;

    ArrayDestroyer<char> p(_clonePath(path));

    if (stat(p.getPointer(), &st) != 0)
	return false;

    size = st.st_size;
    return true;
}

Boolean FileSystem::removeDirectory(const String& path)
{
    ArrayDestroyer<char> p(_clonePath(path));
    return rmdir(p.getPointer()) == 0;	
}

Boolean FileSystem::removeDirectoryHier(const String& path)
{
    // ATTN: Mike - remove following cmts and compile
    cout << "DEBUG RMDIR " << path << endl;

// Even the following does not compile
    // the ostream operator does not work
    // ATTN: Mike: remove following cmts and compile
    // String teststring;
    // teststring.append("abc");
    // cout << teststring;

    // Get list of files in the Directory
    Array<String> fileList;
    if (!FileSystem::getDirectoryContents(path,fileList))
	return false;

    // ATTN: Since not tested.  Following is bypass
    return true;

    // for files-in-directory, delete or recall removedir
    // Do not yet test for boolean returns on the removes
    // ATTN Diagnostics still installed ks.
    for (Uint32 i = 0, n = fileList.getSize(); i < n; i++)
    {
	if (FileSystem::isDirectory(fileList[i])){
	    //cout << "DEBUG RMDIR Next " << fileList[i] << endl;
	    FileSystem::removeDirectoryHier(fileList[i]);
	}

	else{
	    //cout << "DEBUG RMFIL " << fileList[i] <<endl;
	    removeFile(fileList[i]);
	}

    }
    return removeDirectory(path);	
}

Boolean FileSystem::removeFile(const String& path)
{
    ArrayDestroyer<char> p(_clonePath(path));
    return unlink(p.getPointer()) == 0;	
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
/*  Get the file list in the directory into the
    array of strings provided
    @return The function should return false under these circumstances:


    1. The directory does not exist.
    2. The file exists but is not a directory.
    3. The directory is inaccessible.

*/
Boolean FileSystem::getDirectoryContents(
    const String& path,
    Array<String>& paths)
{
    // This may be just extra fluff but added anyway
    if (!FileSystem::isDirectory(path))
	return false;
    
    paths.clear();
    try
    { 
	for (Dir dir(path); dir.more(); dir.next())
	{
	    String name = dir.getName();
    
	    if (String::equal(name, ".") || String::equal(name, ".."))
		continue;
    	    // cout << "DEBUG DIR = " << dir.getName() << endl;
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

#ifdef PEGASUS_OS_TYPE_WINDOWS
    return rename(p.getPointer(), q.getPointer()) == 0;
#else
    if (link(p.getPointer(), q.getPointer()) != 0)
	return false;

    return unlink(p.getPointer()) == 0;
#endif
}

PEGASUS_NAMESPACE_END
