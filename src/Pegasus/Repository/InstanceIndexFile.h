//%2005////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
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
// Modified By: Jenny Yu, Hewlett-Packard Company (jenny_yu@hp.com)
// Modified By: Michael Brasher (mbrasher@bmc.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_InstanceIndexFile_h
#define Pegasus_InstanceIndexFile_h

#include <fstream>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/InternalException.h>
#include <Pegasus/Common/CIMObjectPath.h>
#include <Pegasus/Repository/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

/** This class manages access to an "instance index file" which maps
    instance names to offsets of the instances contained in the "instance 
    data file". 

    The first line of this file is a free count, expressed as eight hex 
    digits followed by a newline. The free count indicates how many instances 
    are free (but not reclaimed). When instances are deleted or modified, the 
    corresponding entry in this index file is marked as free (by changing the
    first column of the entry from '0' to '1'). When the free count reaches 
    some upper bound, reclamation is performed which involves removing free
    entries (from both the index file and the data file).

    All subsequent lines contain an entry with the following form:
  
    <pre>
	<free> <hash-code> <index> <size> ClassName.key1=val1,...,keyN=valN
    </pre>

    The free column is a flag (either '0' or '1') which indicates whether
    the entry is no longer used (in which case it can be removed during
    reclamation).

    The hash-code is an eight digit hex number, the index is the byte
    location of the instance record in the instance file, the size is the
    record size of the instance record, and instance name is a CIM style
    object name.

    Here's an example of an index file:

    <pre>
	00000001
	0 A6B275A9 0 1425 Employee.ssn=444332222 
	1 A6BA08B1 1425 1430 Employee.ssn=555667777 
    </pre>

    To lookup an entry in the index file, first take the hash code of the
    target key. Then find the first non-free entry whose hash code and key
    are the same as the target hash code and key. Note that an entry may
    usually be ruled out by comparing the hash codes (except in the case of
    clashes).
    
    Methods are provided for managing the instance index file: adding,
    removing, and modifying.
*/
class PEGASUS_REPOSITORY_LINKAGE InstanceIndexFile
{
public:
    
    /** Searches the instance index file for the given instance name. Sets 
        the index parameter to the corresponding index, and sets the size
        parameter to the corresponding instance record size.  Returns true
        on success.

        @param path the file path of the instance index file
        @param instanceName the name of the instance
        @param indexOut the index of the instance record found
        @param sizeOut the size of the instance record found
        @return true if the instance is found; false otherwise.
    */
    static Boolean lookupEntry(
	const String& path, 
	const CIMObjectPath& instanceName,
	Uint32& indexOut,
	Uint32& sizeOut);

    /** Creates a new entry in the instance index file. Saves the index and
        size of the instance record passed in.  This method assumes that the
	keys in the instance name are in sorted order. This must be done
        prior to calling the method.  Returns true on success.

        @param path the file path of the instance index file
        @param instanceName the name of the instance
        @param indexIn the index of the new instance record
        @param sizeIn the size of the new instance record 
        @return true if successful; false otherwise.
    */
    static Boolean createEntry(
	const String& path, 
	const CIMObjectPath& instanceName,
	Uint32 indexIn,
        Uint32 sizeIn);

    /** Deletes the entry with the given instance name.
        @param path path of the instance index file
        @param instanceName name of the instance
        @return true on success
    */
    static Boolean deleteEntry(
	const String& path, 
	const CIMObjectPath& instanceName,
	Uint32& freeCount);

    /** Modifies an entry by first removing the old entry and then inserting
        a new entry at the end of the file.

        @param path the file path of the instance index file.
        @param instanceName the name of the instance.
        @param indexIn the index of the modified instance record.
        @param sizeIn the size of the modified instance record.
        @return true on success.
    */
    static Boolean modifyEntry(
	const String& path, 
	const CIMObjectPath& instanceName,
	Uint32 indexIn,
        Uint32 sizeIn,
	Uint32& freeCount);

    /** Gets the information stored in the index file for all the instances
        of the given class.  Appends the instance names, indices and sizes 
        to the given arrays (does not clear the arrays first).  Returns
        true on success.

        @param path path of the instance index file.
        @param instanceNames array to hold the instance names.
        @param indices array to hold the indices of the instance records.
        @param sizes an array to hold the sizes of the instance records.
        @return true on success.
    */
    static Boolean enumerateEntries(
	const String& path,
	Array<Uint32>& freeFlags,
	Array<Uint32>& indices,
        Array<Uint32>& sizes,
	Array<CIMObjectPath>& instanceNames,
	Boolean includeFreeEntries);


    /** Returns true if this index file has any non-free entries:
    */
    static Boolean hasNonFreeEntries(const String& path);

    /** Begin a transaction to modify this file. The effect of subsequent
	modifications can be rolled back by calling rollbackTransaction().
	The current implementation simply copies the index file to a a file
	with the same name but with ".rollback" appended to it.
    */
    static Boolean beginTransaction(const String& path);

    /** Roll back any changes to the file since the last time 
        beginTransaction() was called. The current implementation deletes
	the current file and renames the rollback file to the same name.
    */
    static Boolean rollbackTransaction(const String& path);

    /** Commit changes made after beginTransaction() was called. The curent
	implementation simply removes the .rollback file.
    */
    static Boolean commitTransaction(const String& path);

    /** Compact the file by removing entries which are marked as free.
    */
    static Boolean compact(
        const String& path);

private:

    /** Open the index file and position the file pointer on the first
	entry (immediately after the free count). Create the file if it
	does not exist and if the create flag is true (writing a free 
        count of zero).
    */
    static Boolean _openFile(
	const String& path, 
        PEGASUS_STD(fstream)& fs,
        Boolean create = false);
    
    /** Appends a new entry to the index file; called by both createEntry()
        and modifyEntry().
    */
    static Boolean _appendEntry(
        PEGASUS_STD(fstream)& fs,
        const CIMObjectPath& instanceName,
        Uint32 indexIn,
        Uint32 sizeIn);
    
    /** Increment the index file's free count; called by _markEntryFree().
	The resulting value is left in the freeCount parameter.
    */
    static Boolean _incrementFreeCount(
	PEGASUS_STD(fstream)& fs,
	Uint32& freeCount);

    /** Marks the entry matching the given instanceName as free; called by both
	deleteEntry() and modifyEntry().
    */
    static Boolean _markEntryFree(
        PEGASUS_STD(fstream)& fs,
        const CIMObjectPath& instanceName);

    /** Helper method for lookupEntry() which takes a file stream rather than
	a path.
    */
    static Boolean _lookupEntry(
	PEGASUS_STD(fstream)& fs,
	const CIMObjectPath& instanceName,
	Uint32& indexOut,
	Uint32& sizeOut,
	Uint32& entryOffset);
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_InstanceIndexFile_h */
