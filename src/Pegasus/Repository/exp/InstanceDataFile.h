//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 BMC Software, Hewlett-Packard Company, IBM,
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

#ifndef Pegasus_InstanceDataFile_h
#define Pegasus_InstanceDataFile_h

PEGASUS_NAMESPACE_BEGIN

/** This class manages access to an 'instance file' which stores all the
    instances of a CIM class.
  
    For each instance, The CIM/XML representation is stored in the file.
    The byte location and size of a particular instance record in the file
    is stored in the 'instance index file'.  The name of an instance file
    has the following form:
  
    <pre>
        className.instances
    </pre>
  
    Here's an example:
  
    <pre>
        Employee.instances
    </pre>

    When an instance is added, it is appended to the end of the 
    instance file.  After an instance is deleted, the instance file is
    reorganized to reuse the disk space.  When an instance is modified, 
    the old instance is deleted, and the modified instance is
    appended to the end of the file.  
  
    It is very important to keep both the instance and index file in
    a consistent state.  In order to minimize the window in which the
    instance and index file can be left in an inconsistent state (after
    an unexpected system termination or failure), all updates to the 
    instance and index files are done first to temporary files.  After
    an update operation is successful on BOTH the instance and index files,
    the temporary files are renamed back to the original files.

*/
class PEGASUS_REPOSITORY_LINKAGE InstanceDataFile
{
public:
    
    /** loads an instance record in the instance file to memory.  The caller 
        passes the byte position and the size of the instance record to be 
        read.  Returns true on success.

        @param   path      the file path of the instance file
        @param   index     the byte positon of the instance record
        @param   size      the size of the instance record 
        @param   data      the buffer to hold the instance data 

        @return  true      if successful
                 false     if an error occurs in accessing the instance file
     */
    static Boolean loadInstance(
	const String& path, 
	Uint32 index,
	Uint32 size,  
        Array<Sint8>& data);
    
    /** loads all the instance records in the instance file to memory. 
        Returns true on success.

        @param   path      the file path of the instance file
        @param   data      the buffer to hold the data 

        @return  true      if successful
                 false     if an error occurs in accessing the instance file
     */
    static Boolean loadAllInstances(
	const String& path, 
        Array<Sint8>& data);
    
    /** Inserts a new instance record into the instance file.  The byte 
        position and the size of the newly inserted instance record are 
        returned.  Returns true on success.
     
        @param   out       the buffer containing the CIM/XML encoding of the 
                           instance
        @param   path      the file path of the instance file
        @param   index     the byte positon of the instance record
        @param   size      the size of the instance record 

        @return  true      if successful
                 false     if an error occurs in accessing the instance file
                           or adding a new record to the instance file
     */
    static Boolean insertInstance(
        Array<Sint8> out,
	const String& path, 
	Uint32& index,
	Uint32& size); 

    /** Removes an instance record in the instance file at the given byte 
        position for the given size.  Returns true on success.

        @param   path      the file path of the instance file
        @param   size      the size of the instance record 
        @param   index     the byte positon of the instance record

        @return  true      if successful
                 false     if an error occurs in accessing the instance file
                           or removing a record from the instance file
     */
    static Boolean removeInstance(
	const String& path, 
	Uint32 size,
	Uint32 index);
    
    /** Modifies an instance record in the instance file by first removing
        the old instance record in the instance file at the given byte
        position, then inserts a new instance record into the instance file.
        The byte position and the size of the newly added instance record
        are returned.  Returns true on success.
     
        @param   out       the buffer containing the CIM/XML encoding of the 
                           modified instance
        @param   path      the file path of the instance file
        @param   oldIndex  the byte positon of the old instance record
        @param   oldSize   the size of the old instance record 
        @param   newIndex  the byte positon of the new instance record
        @param   newSize   the size of the new instance record 

        @return  true      if successful
                 false     if an error occurs in accessing or modifying the 
                           instance file
     */
    static Boolean modifyInstance(
        Array<Sint8> out,
	const String& path, 
	Uint32 oldIndex,
	Uint32 oldSize, 
	Uint32& newIndex,
	Uint32& newSize); 

private:
    
    /** Loads data from the instance file.

        This method is called to load data from the instance file.  It loads
        the given number of bytes starting at the given byte position of the
        file.  Returns true on success.          

        @param   path      the file path of the instance file
        @param   index     the starting byte positon 
        @param   size      the number of bytes to load
        @param   data      the buffer to hold the data 

        @return  true      if successful
                 false     if an error occurs in accessing the instance file
     */
    static Boolean _loadData(
	const String& path, 
	Uint32 index,
	Uint32 size,  
        Array<Sint8>& data);
    
    /** Removes data in the temporary instance file at the given byte position
        for the given size.  Returns true on success.

        @param   path      the file path of the instance file
        @parm    os        the ofstream of the temporary instance file
        @param   size      the size of the instance record 
        @param   index     the byte positon of the instance record

        @return  true      if successful
                 false     if an error occurs in accessing the instance file
                           or removing a record from the instance file
     */
    static Boolean _removeData(
	const String& path, 
	PEGASUS_STD(ofstream)& os, 
	Uint32 size,
	Uint32 index);

    /** Appends data to the end of the temporary instance file.  Returns true
        on success.

        @param   out       the buffer containing the CIM/XML encoding of the 
                           instance
        @parm    os        the ofstream of the temporary instance file
        @param   index     the byte positon of the instance record
        @param   size      the size of the instance record 

        @return  true      if successful
                 false     if an error occurs in accessing the instance file
                           or removing a record from the instance file
     */
    static Boolean _insertData(
        Array<Sint8> out,
	PEGASUS_STD(ofstream)& os, 
	Uint32& index,
	Uint32& size);
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_InstanceDataFile_h */
