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
// Modified By:
//              Sean Keenan, Hewlett-Packard Company (sean.keenan@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////


#include <cstdlib>
#include <cassert>
#include <Pegasus/Common/Queue.h>
#include <Pegasus/Common/Stopwatch.h>
#include <Pegasus/Common/Array.h>
PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

static char * verbose;

int main(int argc, char** argv)
{
    verbose = getenv("PEGASUS_TEST_VERBOSE");
    try
    {
        // Simple test with Uint32 Stack of push, pop, top, and tests.
        Queue<Uint32> q1;
        assert (q1.isEmpty());

        q1.enqueue(1);
        assert(q1.size() == 1);
        assert(!q1.isEmpty());
        assert(q1.back() == 1);
        assert(q1.front() == 1);

        q1.enqueue(2);
        assert(q1.size() == 2);
        assert (!q1.isEmpty());
        assert(q1.back() == 2);
        assert(q1.front() == 1);

        q1.dequeue();
        assert(q1.size() == 1);
        assert(!q1.isEmpty());
        assert(q1.back() == 2);
        assert(q1.front() == 2);


        q1.dequeue();
        assert(q1.size() == 0);
        assert (q1.isEmpty());



        // Performance tests - Filling and emptying

        Stopwatch sw;

        Queue<Uint32> q2;
        Uint32 queueSize = 10000;

        sw.start();

        for (Uint32 i = 0; i < queueSize; i++)
            q2.enqueue(i);

        assert(q2.size() == queueSize);

        for (Uint32 i = 0; i < queueSize; i++)
            q2.dequeue();

        sw.stop();

        if(verbose)
        {
            cout << "Queue fill and empty of " << queueSize << " integers in " <<
                sw.getElapsed() << " Seconds" << endl;
        }

        // Performance test - Passing item through a queue.
        {
            Stopwatch sw;

            Queue<Uint32> q3;
            Uint32 queueSize = 3000;

            sw.start();

            for (Uint32 i = 0; i < queueSize; i++)
                q2.enqueue(i);

            Uint32 iterations = 10000;

            for (Uint32 i = 0; i < iterations; i++)
                q3.enqueue(i);

            q3.dequeue();

            sw.stop();

            if (verbose)
            {
                cout << "Queue transit. Queue size =  " << queueSize
                << " integers. Transit of " << iterations << " in "
                << sw.getElapsed() << " Seconds" << endl;
            }
        }

        // Testing items in a queue
        Queue<Uint32> q4;
        for (Uint32 i = 0; i < 30; i++)
        {
            q4.enqueue(i);
        }
        Boolean found = false;
        /*if (Contains(q4, 12))
        {
            found = true;
        }
        assert(found);
        found = false;

        for (Uint32 i = 0; i < 30; i++)
        {
            if (q4[i] = 12)
            {
                found = true;
            }
        }
        assert(found);*/

        assert(q4.size() == 30);
        q4.remove(12);

        assert(q4.size() == 29);
        found = false;

        for (Uint32 i = 0; i < q4.size(); i++)
        {
            if (q4[i] == 12)
            {
                assert(false);
            }
        }

        // Test removing from a queue with remove
        {
            Queue<Uint32> q5;
            for (Uint32 i = 0; i < 30; i++)
            {
                q5.enqueue(i);
            }
            while (q5.size() > 0)
            {
                Uint32 qsize = q5.size();
                q5.remove(0);
                if (q5.size() != qsize - 1)
                {
                    assert (false);
                }
            }
        }

    }
    catch (Exception& e)
    {
    cout << "Exception: " << e.getMessage() << endl;
    exit(1);
    }

    cout << argv[0] << " +++++ passed all tests" << endl;

    return 0;
}

