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

#ifndef Pegasus_InstanceIndexFile_h
#define Pegasus_InstanceIndexFile_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/CIMReference.h>

PEGASUS_NAMESPACE_BEGIN

/** This class manages access to an "instance index file" which maps
    instance names to indexes. Lines of this file have the following form:

    <pre>
	<hash-code> ClassName.key1=value1,...,keyN=valueN Index
    </pre>

    Where the hash-code is an eight digit hex number, the instance name is
    a CIM style object name, and the index is a positive integer (non-zero). 
    Here's an example:

    <pre>
	A6B275A9 Employee.ssn=444332222 1
	A6BA08BE Employee.ssn=555667777 3
    </pre>

    Each instance in Pegasus is represented as a disk file containing the
    CIM/XML encoding of the instance. The name of the file has this form:

    <pre>
	ClassName.Index
    </pre>

    Here's an example:

    <pre>
	Employee.3
    </pre>

    An instance can be obtained from an instance name by searching all entries
    in the index file with the same hash code (to account for hash clashes),
    comparing the instance names, and then forming the name of the instance
    file itself from the index.

    Methods are provided for managing the instance index file: adding,
    removing, and modifying instance names.
*/

class PEGASUS_REPOSITORY_LINKAGE InstanceIndexFile
{
public:
    
    /** Search the index file for the given instance name. Sets the index
	parameter to the corresponding index. Returns true on success.
    */
    static Boolean lookup(
	const String& path, 
	const CIMReference& instanceName,
	Uint32& index);

    /** Inserts a new entry into the instance file. The index parameter is
	set to the index of the entry. This method assumes that the keys in 
	the instance name are in sorted order. This must be done prior to 
	calling the method. Return true on success.
    */
    static Boolean insert(
	const String& path, 
	const CIMReference& instanceName,
	Uint32& index);

    /** Remove the entry with the given instance name. Returns true on success.
    */
    static Boolean remove(
	const String& path, 
	const CIMReference& instanceName);
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_InstanceIndexFile_h */
