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

#include <dlfcn.h>
#include <unistd.h>
#include <dirent.h>
#include "System.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <cstdio>

PEGASUS_NAMESPACE_BEGIN

#include <sys/time.h>
#include <unistd.h>

inline void sleep_wrapper(Uint32 seconds)
{
    sleep(seconds);
}

void System::getCurrentTime(Uint32& seconds, Uint32& milliseconds)
{
    timeval tv;
    gettimeofday(&tv, 0);
    seconds = int(tv.tv_sec);
    milliseconds = int(tv.tv_usec) / 1000;
}

void System::sleep(Uint32 seconds)
{
    sleep_wrapper(seconds);
}

Boolean System::exists(const char* path)
{
    return access(path, F_OK) == 0;
}

Boolean System::canRead(const char* path)
{
    return access(path, R_OK) == 0;
}

Boolean System::canWrite(const char* path)
{
    return access(path, W_OK) == 0;
}

Boolean System::getCurrentDirectory(char* path, Uint32 size)
{
    return getcwd(path, size) != NULL;
}

Boolean System::isDirectory(const char* path)
{
    struct stat st;

    if (stat(path, &st) != 0)
	return false;

    return S_ISDIR(st.st_mode);
}

Boolean System::changeDirectory(const char* path)
{
    return chdir(path) == 0;
}

Boolean System::makeDirectory(const char* path)
{
    return mkdir(path, 0777) == 0;
}

Boolean System::getFileSize(const char* path, Uint32& size)
{
    struct stat st;

    if (stat(path, &st) != 0)
	return false;

    size = st.st_size;
    return true;
}

Boolean System::removeDirectory(const char* path)
{
    return rmdir(path) == 0;	
}

Boolean System::removeFile(const char* path)
{
    return unlink(path) == 0;	
}

Boolean System::renameFile(const char* oldPath, const char* newPath)
{
    if (link(oldPath, newPath) != 0)
	return false;

    return unlink(oldPath) == 0;
}

DynamicLibraryHandle System::loadDynamicLibrary(const char* fileName)
{
    return DynamicLibraryHandle(dlopen(fileName, 0));
}

DynamicSymbolHandle System::loadDynamicSymbol(
    DynamicLibraryHandle libraryHandle,
    const char* symbolName)
{
    return DynamicSymbolHandle(dlsym(libraryHandle, (char*)symbolName));
}

PEGASUS_NAMESPACE_END
