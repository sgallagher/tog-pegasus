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
// Modified By: Ben Heilbronn (ben_heilbronn@hp.com)
//
// Modified By: Nag Boranna (nagaraja_boranna@hp.com)
//              Sushma Fernandes (sushma_fernandes@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifdef PEGASUS_OS_HPUX
# include <dl.h>
#elif defined(PEGASUS_PLATFORM_ZOS_ZSERIES_IBM)
# include <dll.h>
#else
# include <dlfcn.h>
#endif

# include <unistd.h>
#include <dirent.h>
#include "System.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <cstdio>
#include <time.h>
#include <Pegasus/Common/Tracer.h>

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

String System::getCurrentASCIITime()
{
    char    str[50];
    time_t  rawTime;

    time(&rawTime);
    strftime(str, 40,"%T-%D", localtime(&rawTime));
    String time = str;
    return time;
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
    const char METHOD_NAME[] = "System::loadDynamicLibrary()";

    PEG_FUNC_ENTER(TRC_OS_ABSTRACTION, METHOD_NAME);

    Tracer::trace(TRC_OS_ABSTRACTION, Tracer::LEVEL2, 
                  "Attempting to load library %s", fileName);

#if defined(PEGASUS_OS_HPUX)
    void* handle = shl_load(fileName, BIND_IMMEDIATE | DYNAMIC_PATH, 0L);

    Tracer::trace(TRC_OS_ABSTRACTION, Tracer::LEVEL2, 
                  "After loading lib %s, error code is %d", fileName, errno);

    PEG_FUNC_EXIT(TRC_OS_ABSTRACTION, METHOD_NAME);
    return DynamicLibraryHandle(handle);
#elif defined(PEGASUS_OS_TRU64)
    PEG_FUNC_EXIT(TRC_OS_ABSTRACTION, METHOD_NAME);
    return DynamicLibraryHandle(dlopen(fileName, RTLD_NOW));
#elif defined(PEGASUS_OS_ZOS)
    PEG_FUNC_EXIT(TRC_OS_ABSTRACTION, METHOD_NAME);
    return DynamicLibraryHandle(dllload(fileName));
#else
    PEG_FUNC_EXIT(TRC_OS_ABSTRACTION, METHOD_NAME);
    return DynamicLibraryHandle(dlopen(fileName, RTLD_NOW | RTLD_GLOBAL));
#endif

}

void System::unloadDynamicLibrary(DynamicLibraryHandle libraryHandle)
{
    // ATTN: Should this method indicate success/failure?
#ifdef PEGASUS_OS_LINUX
    dlclose(libraryHandle);
#endif

#ifdef PEGASUS_OS_HPUX
    // Note: shl_unload will unload the library even if it has been loaded
    // multiple times.  No reference count is kept.
    int ignored = shl_unload(shl_t(libraryHandle));
#endif
}

String System::dynamicLoadError() {
    // ATTN: Is this safe in a multi-threaded process?  Should this string
    // be returned from loadDynamicLibrary?
#ifdef PEGASUS_OS_HPUX
    // ATTN: If shl_load() returns NULL, this value should be strerror(errno)
    return String();
#elif defined(PEGASUS_OS_ZOS)
    return String();
#else
    String dlerr = dlerror();
    return dlerr;
#endif
}


DynamicSymbolHandle System::loadDynamicSymbol(
    DynamicLibraryHandle libraryHandle,
    const char* symbolName)
{
#ifdef PEGASUS_OS_HPUX
    char* p = (char*)symbolName;
    void* proc = 0;

    if (shl_findsym((shl_t*)&libraryHandle, p, TYPE_UNDEFINED, &proc) == 0)
        return DynamicSymbolHandle(proc);

    p = strcpy(new char[strlen(symbolName) + 2], symbolName);
    strcpy(p, "_");
    strcat(p, symbolName);

    if (shl_findsym((shl_t*)libraryHandle, p, TYPE_UNDEFINED, &proc) == 0)
    {
        delete [] p;
        return DynamicSymbolHandle(proc);
    }

    return 0;

#elif defined(PEGASUS_OS_ZOS)
    return DynamicSymbolHandle(dllqueryfn((dllhandle *)libraryHandle,
                               (char*)symbolName));
#else

    return DynamicSymbolHandle(dlsym(libraryHandle, (char*)symbolName));

#endif
}

String System::getHostName()
{
    static char hostname[64];

    if (!*hostname)
        gethostname(hostname, sizeof(hostname));

    return hostname;
}

String System::getPassword(const char* prompt)
{

    String password;

    password = String(getpass( prompt ));

    return password; 
}

PEGASUS_NAMESPACE_END
