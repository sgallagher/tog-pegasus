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
// Modified By: David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
//
//%/////////////////////////////////////////////////////////////////////////////


/* Test program for the base64 class
*/


#include <cassert>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Base64.h>
#include <Pegasus/Common/ArrayInternal.h>
#include <Pegasus/Common/InternalException.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

int main(int argc, char** argv)
{
    Boolean verbose = (getenv("PEGASUS_TEST_VERBOSE")) ? true : false;
    Uint32 maxLength = 1000;

    // Test a simple decode to determine if decode correct
    {
        Array<char> in;
        Array<char> out;
        // Create an array of the character A 4 times.
        in.append('A');
        in.append('A');
        in.append('A');
        in.append('A');
        out = Base64::decode(in);
        // Should produce 3 bytes of zeros
        assert(in.size() == 4);
        out = Base64::decode(in);
        assert(out.size() == 3);
        // The result should be all zeros.
        for (Uint32 i = 0; i < out.size(); i++)
            if (out[i] != 0)
            {
                if(verbose)
                    cout <<"Decode error" << endl;
            }
    }
    // Test a simple decode to determine if decode correct
    {
        Array<char> in;
        Array<char> out;
        // Create an array of the character A 4 times.
        in.append('/');
        in.append('/');
        in.append('/');
        in.append('/');
        out = Base64::decode(in);
        // Should produce 3 bytes of -1s
        assert(in.size() == 4);
        out = Base64::decode(in);
        assert(out.size() == 3);
        // The result should be all zeros.
        for (Uint32 i = 0; i < out.size(); i++)
            if (out[i] != char(0xFF)) //char 255
            {
                if(verbose)
                 cout << "Decode error " << static_cast<int>(out[i]) << endl;
            }
    }



    // Execute a set of fixed coding tests. Start from Uint buffer
    // and determine if correct character set created
    {
        // Test for creation of char A
        Array<char> in;
        Array<char> out;

        // Create Array of 3 zeros.
        in.append(0);
        in.append(0);
        in.append(0);

        // confirm that the character A is created, 6 characters

        out = Base64::encode(in);
        assert(out.size() == 4);
        for (Uint32 i = 0; i <out.size(); i++ )
        {
            assert(out[i] == 'A');
        }
    }

    if(verbose)
        cout << "Code and decode tests. Test 1 - all characters in buffer"
                << endl;
    {
        // Repeat test for buffer size from 0 to 1000 characters long
        for (Uint32 i = 0; i < maxLength; i++)
        {
            //build an array i characters long with integers 0 - 63

            Array<char> in;
            Array<char> out;

            Uint8 k = 0;

            // Build the array from assending set of characters
            in.append(k++);
            Array<char> temp = Base64::encode(in);
            out = Base64::decode(temp);

            assert(in == out);

            /* Detailed error testing if we need to turn it on

            if (in.size() != out.size())
            {
                cout << "size error" << endl;
            }
            for (Uint32 l = 0; l < out.size(); l++)
                if (in[l] != out[l])
                    cout << "Error in something" << endl;
            if (!(in == out))
               cout << "Problem with Base64 Equal test" << endl;
            */
        }
    }

    // Test with buffer of all zeros
    if(verbose)
        cout << "Test 2 -Test all zero buffer" << endl;
    {
        Array<char> in;
        Array<char> out;

        for (Uint32 i = 0; i < maxLength; i++)
        {
            in.append(0);
            Array<char> temp = Base64::encode(in);
            out = Base64::decode(temp);
            assert(in == out);
        }
    }


    // Test with buffer of char 0xFF (255)
    if(verbose)
        cout << "Test 3 -- Test all 1s in buffer" << endl;
    {
        Array<char> in;
        Array<char> out;

        for (Uint32 i = 0; i < maxLength; i++)
        {
            in.append('\xFF');
            Array<char> temp = Base64::encode(in);
            out = Base64::decode(temp);

            assert(in == out);
        }
    }

    // One more
    {
        Array<char> in;
        Array<char> out;

        for (Uint32 i = 0; i < maxLength; i++)
        {
            in.append('\xFF');
            out = Base64::decode(Base64::encode(in));

            assert(in == out);
        }

    }
    cout << argv[0] << " +++++ passed all tests" << endl;
    return 0;
}

