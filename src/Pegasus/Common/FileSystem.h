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
// $Log: FileSystem.h,v $
// Revision 1.2  2001/02/11 05:42:33  mike
// new
//
// Revision 1.1.1.1  2001/01/14 19:51:36  mike
// Pegasus import
//
//
//END_HISTORY

////////////////////////////////////////////////////////////////////////////////
//
// FileSystem.h
//
//	This class provides utilities for manipulating and interrogating the
//	file system.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_FileSystem_h
#define Pegasus_FileSystem_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Exception.h>

PEGASUS_NAMESPACE_BEGIN

class PEGASUS_COMMON_LINKAGE FileSystem
{
public:

    /// Return true if the file exists (and false otherwise).
    static Boolean exists(const String& path);

    /** Return true if the file exists (and false otherwise). Ignore the
	case of the file and return the real name of the file.
    */
    static Boolean existsIgnoreCase(const String& path, String& realPath);

    // Returns true if the file exists and can be read:

    static Boolean canRead(const String& path);

    // Returns true if the file exists and can be written:

    static Boolean canWrite(const String& path);

    // Returns true if the file exists and can be executed:

    // static Boolean canExecute(const String& path);

    // Returns true if file exists and is a directory:

    static Boolean isDirectory(const String& path);

    // Change to the given directory:

    static Boolean changeDirectory(const String& path);

    // Create a directory:

    static Boolean makeDirectory(const String& path);

    // Get the size of the file in bytes:

    static Boolean getFileSize(const String& path, Uint32& size);

    // Remove the given directory:

    static Boolean removeDirectory(const String& path);

    // Remove the given file:

    static Boolean removeFile(const String& path);

    // Get the names of the files (and directories) in the given directory:

    static Boolean getDirectoryContents(
	const String& path,
	Array<String>& paths);

    // Load the contents of the file into the array. Throws CannotOpenFile if
    // unable to open file.

    static void loadFileToMemory(
	Array<Sint8>& array,
	const String& fileName);

    // Return true if the two files are identical. Throws CannotOpenFile if
    // either file cannot be opened.

    static Boolean compare(
	const String& fileName1,
	const String& fileName2);
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_FileSystem_h */
