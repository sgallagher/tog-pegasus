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
//         Jenny Yu, Hewlett-Packard Company (jenny_yu@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_InstanceIndexFile_h
#define Pegasus_InstanceIndexFile_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/CIMObjectPath.h>

PEGASUS_NAMESPACE_BEGIN

/** This class manages access to an 'instance index file' which maps
    instance names to the index (or location) of the instances in an
    'instance file'.  Lines of this file have the following form:
  
    <pre>
	<hash-code> <index> <size> ClassName.key1=value1,...,keyN=valueN 
    </pre>

    Where the hash-code is an eight digit hex number, the index is the byte
    location of the instance record in the instance file, the size is the
    record size of the instance record, and instance name is a CIM style
    object name.

    Here's an example:

    <pre>
	A6B275A9 0 1425 Employee.ssn=444332222 
	A6BA08BE 1425 1430 Employee.ssn=555667777 
    </pre>

    Each instance in Pegasus is represented as a record in the instance 
    file for the CIM class.  Each instance record contains the CIM/XML 
    encoding of the instance.  Information about each instance record in
    an instance file is kept in an instance index file.  The name of 
    an instance index file has the following form:

    <pre>
	ClassName.idx
    </pre>

    Here's an example:

    <pre>
	Employee.idx
    </pre>

    An instance can be obtained from an instance name by searching all entries
    in the instance index file with the same hash code (to account for hash
    clashes), comparing the instance names, and obtaining the index and size 
    of the instance record in the corresponding instance file.

    Methods are provided for managing the instance index file: adding,
    removing, and modifying.

    It is very important to keep both the instance and index file in a
    consistent state.  In order to minimize the window in which the
    instance and index file can be left in an inconsistent state (after
    an unexpected system termination or failure), all updates to the
    instance and index files are done first to the temporary files.  After
    an update operation is successful on BOTH the instance and index files,
    the temporary files are renamed back to the original files.

*/

class PEGASUS_REPOSITORY_LINKAGE InstanceIndexFile
{
public:
    
    /** Searches the instance index file for the given instance name. Sets 
        the index parameter to the corresponding index, and sets the size
        parameter to the corresponding instance record size.  Returns true
        on success.

        @param   path          the file path of the instance index file
        @param   instanceName  the name of the instance
        @param   sizeOut       the size of the instance record found
        @param   indexOut      the index of the instance record found

        @return  true          if the instance is found
                 false         if the instance cannot be found
    */
    static Boolean lookup(
	const String& path, 
	const CIMObjectPath& instanceName,
	Uint32& sizeOut,  
	Uint32& indexOut);

    /** Inserts a new entry into the instance index file. Saves the index and
        size of the instance record passed in.  This method assumes that the
	keys in the instance name are in sorted order. This must be done
        prior to calling the method.  Returns true on success.

        @param   path          the file path of the instance index file
        @param   instanceName  the name of the instance
        @param   sizeIn        the size of the new instance record 
        @param   indexIn       the index of the new instance record

        @return  true          if successful
                 false         if an error occurs in inserting the entry
    */
    static Boolean insert(
	const String& path, 
	const CIMObjectPath& instanceName,
        Uint32 sizeIn,
	Uint32 indexIn);

    /** Removes the entry with the given instance name.  Returns true on 
        success.

        @param   path          the file path of the instance index file
        @param   instanceName  the name of the instance

        @return  true          if successful
                 false         if an error occurs in removing the entry
    */
    static Boolean remove(
	const String& path, 
	const CIMObjectPath& instanceName);

    /** Modifies an entry by first removing the old entry and then inserting
        a new entry at the end of the file.  Returns true on success.

        @param   path          the file path of the instance index file
        @param   instanceName  the name of the instance
        @param   sizeIn        the size of the modified instance record 
        @param   indexIn       the index of the modified instance record

        @return  true          if successful
                 false         if an error occurs in modifying the entry
    */
    static Boolean modify(
	const String& path, 
	const CIMObjectPath& instanceName,
        Uint32 sizeIn,
	Uint32 indexIn);

    /** Gets the information stored in the index file for all the instances
        of the given class.  Appends the instance names, indices and sizes 
        to the given arrays (does not clear the arrays first).  Returns
        true on success.

        @param   path           the file path of the instance index file
        @param   instanceNames  an array to hold the instance names
        @param   indices        an array to hold the indices of the instance
                                records
        @param   sizes          an array to hold the sizes of the instance
                                records

        @return  true          if successful
                 false         if an error occurs in loading information from
                               the instance index file
    */
    static Boolean appendInstanceNamesTo(
	const String& path,
	Array<CIMObjectPath>& instanceNames,
	Array<Uint32>& indices,
        Array<Uint32>& sizes);

private:
    
    /** Appends a new entry to the temporary index file.  Returns true
        on success.

        @param   os            the ofstream of the temporary index file
        @param   instanceName  the name of the instance 
        @param   sizeIn        the size of the new instance record 
        @param   indexIn       the index of the new instance record

        @return  true          if successful
                 false         if an error occurs in appending the entry
    */
    static void _appendEntry(
        PEGASUS_STD(ofstream)& os,
        const CIMObjectPath& instanceName,
        Uint32 sizeIn,
        Uint32 indexIn);
    
    /** Removes an entry from the temporary instance index file.  Returns
        true on success.

        @param   os            the ofstream of the temporary index file
        @param   path          the file path of the original instance index file
        @param   instanceName  the name of the instance 

        @return  true          if successful
                 false         if an error occurs in removing the entry
    */
    static Boolean _removeEntry(
        PEGASUS_STD(ofstream)& os,
        const String& path,
        const CIMObjectPath& instanceName);
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_InstanceIndexFile_h */
