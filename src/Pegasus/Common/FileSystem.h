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
#include <Pegasus/Common/Array.h>
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

    /** Get the current working Directory. */
    static Boolean getCurrentDirectory(String& path);

    /** Remove the given directory. The directory must be empty
	to be eligible for removal
	@param String path is the relative or ablsolute path to
	the directory to remove
	@return true if directory removed
    */
    static Boolean removeDirectory(const String& path);

    /** Remove a directory hiearchy. Removes a complete hiearchy of
	directories and files.
	
	WARNING: This differs significantly from the <TT>removeDirectory</TT>
	function in that it removes both directories and files and
	removes a complete hiearchy.  Use with caution.
	
	@parm path defines the high level directory to be removed
	@return Boolean - ATTN.
	@exception  - ATTN: Not sure if there is any exception
    */ 
    static Boolean removeDirectoryHier(const String& path);

    /** Remove the file defined by the input parameter
	@param path of file to remove
	@return Boolean true if directory removed
    */
    static Boolean removeFile(const String& path);

    /** Get the names of the files (and directories) in the given directory:

	@param path - the path of the directory from which we will get filenames
	@param paths - On return, this  Array contains the names of the files 
	in the directory
	ATTN: Is this local names or fully qualified names with paths.
	@return Boolean that is today only true.
	@exception Throws "NoSuchDirectory" if the directory defined in path does 
	not exist.
    */
    static Boolean getDirectoryContents(
	const String& path,
	Array<String>& paths);

    /** Load the contents of the file into the array. Throws CannotOpenFile if
	unable to open file.
    */

    static void loadFileToMemory(
	Array<Sint8>& array,
	const String& fileName);

    /** Compare two file for content.
	@param filename of first file
	@param filename of second file
	ATTN: are filenames local or global???
	@return Return true if the two files are identical. 
	@exception Throws CannotOpenFile if either file cannot be opened.
    */
    static Boolean compare(
	const String& fileName1,
	const String& fileName2);

    /** Rename the given file to the new name. */
    static Boolean renameFile(
	const String& oldFileName,
	const String& newFileName);

    /** Translate backward slashes to forward slashes: */
    static void translateSlashes(String& path);

    /** Returns true is the given directory is empty. */
    static Boolean isDirectoryEmpty(const String& path);
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_FileSystem_h */
