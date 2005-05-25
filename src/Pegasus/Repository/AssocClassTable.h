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
// Modified By: Carol Ann Krug Graves, Hewlett-Packard Company
//                  (carolann_graves@hp.com)
//              Karl Schopmeyer (k.schopmeyer@opengroup.org)
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//              David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_AssocClassTable_h
#define Pegasus_AssocClassTable_h

#include <fstream>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMObjectPath.h>
#include <Pegasus/Common/ArrayInternal.h>
#include <Pegasus/Repository/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

/** Maintains all associations for a given namesspace.
*/
class PEGASUS_REPOSITORY_LINKAGE AssocClassTable
{
public:

    /** Appends a row into the association class table. There is no checking
        for duplicate entries (the caller ensures this). The case of
        the arguments doesn't matter. They are ignored during comparison.
    */
    static void append(
        PEGASUS_STD(ofstream)& os,
        const CIMName& assocClassName,
        const CIMName& fromClassName,
        const CIMName& fromPropertyName,
        const CIMName& toClassName,
        const CIMName& toPropertyName);

    /** Appends a row into the association class table. There is no checking
        for duplicate entries (the caller ensures this). The case of the
        arguments doesn't matter. Case is ignored during comparison.
    */
    static void append(
        const String& path,
        const CIMName& assocClassName,
        const CIMName& fromClassName,
        const CIMName& fromPropertyName,
        const CIMName& toClassName,
        const CIMName& toPropertyName);

    /** Deletes the given association from the table by removing every entry
        with the given assocClassName.
        @returns true if such an association was found.
    */
    static Boolean deleteAssociation(
        const String& path,
        const CIMName& assocClassName);

    /** Finds all associators of the given class. See 
        CIMOperations::associators() for a full description.
    */
    static Boolean getAssociatorNames(
        const String& path,
        const Array<CIMName>& classList,
        const Array<CIMName>& assocClassList,
        const Array<CIMName>& resultClassList,
        const String& role,
        const String& resultRole,
        Array<String>& associatorNames);

    /** Get all references (association class names) in which the
        given class involved. See CIMOperations::referenceNames() for a 
        full description.
    */
    static Boolean getReferenceNames(
        const String& path,
        const Array<CIMName>& classList,
        const Array<CIMName>& resultClassList,
        const String& role,
        Array<String>& referenceNames);

private:

    AssocClassTable() { /* private */ }
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_AssocClassTable_h */
