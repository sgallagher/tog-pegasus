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

#ifndef Pegasus_InheritanceTree_h
#define Pegasus_InheritanceTree_h

#include <iostream>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>

PEGASUS_NAMESPACE_BEGIN

struct InheritanceTreeRep;

/** The InheritanceTree class tracks inheritance relationships of CIM classes.

    This class is a memory resident version of the repository's persistent 
    inheritance information (represented using file names). The InheritanceTree 
    provides O(1) access (via hashing) to any class in the inheritance tree.

*/
class PEGASUS_REPOSITORY_LINKAGE InheritanceTree
{
public:

    InheritanceTree();

    ~InheritanceTree();

    Boolean insert(const String& className, const String& superClassName);

    void print(std::ostream& os) const;

private:

    InheritanceTreeRep* _rep;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_InheritanceTree_h */
