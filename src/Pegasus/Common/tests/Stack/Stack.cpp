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
// Author: Karl Schopmeyer (k.schopmeyer@opengroup.org)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Stack.h>

#include <cstdlib>
#include <cassert>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

int main(int argc, char** argv)
{
    try
    {
    // Simple test with Uint32 Stack of push, pop, top, and tests.
    Stack<Uint32> s1;
    assert (s1.isEmpty());

    s1.push(1);
    assert(s1.size() == 1);
    assert (!s1.isEmpty());

    s1.push(2);
    assert(s1.size() == 2);
    assert (!s1.isEmpty());

    assert(s1.top() == 2);

    s1.pop();
    assert(s1.size() == 1);
    assert (!s1.isEmpty());

    s1.pop();
    assert(s1.size() == 0);
    assert (s1.isEmpty());

    // Performance tests

    Stack<Uint32> s2;
    Uint32 stackSize = 1000000;

    for (Uint32 i = 0; i < stackSize; i++)
        s2.push(i);

    assert(s2.size() == stackSize);
    for (Uint32 i = 0; i < stackSize; i++)
        s2.pop();

    // SF-HP
    Stack<Uint32> s3;
        s3 = s1;

    const Stack<Uint32> s4;

        // throws Stack underflow
        try
        {
            s4.top();
        }
        catch(StackUnderflow& e)
        {
        }

        // throws OutOfBounds
        try
        {
            Uint32 tmp = s4[0];
        }
        catch(IndexOutOfBoundsException& e)
        {
        }

    s1.push(2);
        Uint32 tmp = s1[0];

    }
    catch (Exception& e)
    {
    cout << "Exception: " << e.getMessage() << endl;
    exit(1);
    }

    cout << argv[0] << " +++++ passed all tests" << endl;

    return 0;
}
