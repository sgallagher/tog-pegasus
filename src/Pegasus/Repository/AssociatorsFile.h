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

#ifndef Pegasus_AssociatorsFile_h
#define Pegasus_AssociatorsFile_h

#include <Pegasus/Common/Config.h>

PEGASUS_NAMESPACE_BEGIN

/** The AssociatorsFile class keeps track of associators of a particular 
    instance. This class supports the implementation of the associators
    and associatorNames CIM operations. For details, see the documentation
    for CIMOperations::associators() and CIMOperations::associatorNames().

    If an instance (or class) is involed in any associations, then an 
    associator file must exist which contains those associators. For example, 
    if the instance contained in the file "MyClass.451" has associators,
    they will be kept in the file named "MyClass.451.associators". Similarly,
    if the class contained in "MyClass.YourClass" has assocaitors, they
    are kept in "MyClass.YourClass.associators".

    Associator files have at least one line for each association in which the
    particular instance (or class) is involved (if an association ties
    an instance to N other instances, then there will be N lines in the
    association file for that association). Lines of the files are broken 
    into fields as shown below (for a thorough explanation of the first four 
    fields, see the documentation for CIMOperations::instanceNames()).

	<ul>
	<li>association-class</li>
	<li>result-class</li>
	<li>role</li>
	<li>result-role</li>
	<li>object-name - name of instance (or class) with which the instance 
	    (or class) has an association.</li>
	</ul>

    This class provides methods for inserting, modifying, finding, and deleting
    entries from the associators files.
*/
class AssociatorsFile
{
public:

    /** Insert an entry into the given association file.
    */
    static Boolean insert(
	const String& path, 
        const String& assocClass,
        const String& resultClass,
        const String& role,
        const String& resultRole,
	const CIMReference& objectName);

private:

    AssociatorsFile() { }
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_AssociatorsFile_h */
