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
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_AssocFile_h
#define Pegasus_AssocFile_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Array.h>

PEGASUS_NAMESPACE_BEGIN

/** Maintains all associations for a given namesspace.
*/
class PEGASUS_REPOSITORY_LINKAGE AssocFile
{
public:

    /** Appends a row into the association table. There is no checking
	for duplicate entries (the caller ensures this).
    */
    static void append(
	const String& path,
	const String& assocInstanceName,
	const String& assocClassName,
	const String& fromObjectName,
	const String& fromClassName,
	const String& fromPropertyName,
	const String& toObjectName,
	const String& toClassName,
	const String& toPropertyName);

    /** Determines whether table contains an assoication entry for the given 
	object (either a class or an instance). This routine compares the 
	objectName parameter with the fromObjectName column.
	@returns true if table contains the given object.
    */
    static Boolean containsObject(
	const String& path,
	const String& objectName);

    /** Deletes the given association from the table by removing every entry
	with an assocInstanceName equal to the assocInstanceName parameter.
	@returns true if such an association was found.
    */
    static Boolean deleteAssociation(
	const String& path,
	const String& assocInstanceName);

    /** Finds all associators of the given object. See 
	CIMOperations::associators() for a full description.
    */
    static Boolean getAssociatorNames(
	const String& path,
	const String& objectName,
        const String& assocClass,
        const String& resultClass,
        const String& role,
        const String& resultRole,
	Array<String>& associatorNames);

    /** Obtain all references (association instance names) in which the
	given object is involved. See CIMOperations::associators() for a 
	full description.
    */
    static Boolean getReferenceNames(
	const String& path,
	const String& objectName,
 	const String& resultClass,
 	const String& role,
	Array<String>& referenceNames);

private:

    AssocFile() { /* private */ }
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_AssocFile_h */
