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
// Author: Karl Schopmeyer (k.schopmeyer@opengroup.org)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////


#include <cstdlib>
#include <cassert>
#include <Pegasus/Common/Queue.h>
#include <Pegasus/Common/Stopwatch.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

int main()
{
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

	assert(q1.back() = 2);
	assert(q1.front() = 1);

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
    
	for (Uint32 i = 0; i < queueSize; i++)
	    q2.enqueue(i);
    
	assert(q2.size() == queueSize);
	for (Uint32 i = 0; i < queueSize; i++)
	    q2.dequeue();
    	cout << "Queue fill and empty of " << queueSize << " integers in " << 
		sw.getElapsed() << " Seconds" << endl;
	
	// Performance test - Passing item through a queue.
	{
	    Queue<Uint32> q3;
    	    queueSize = 30;
	    for (Uint32 i = 0; i < queueSize; i++)
		q2.enqueue(i);

	    Stopwatch sw;
	    Uint32 iterations = 100000;
	    for (Uint32 i = 0; i < iterations; i++)
		 q3.enqueue(i);
		 q3.dequeue();

	   cout << "Queue transit. Queue size =  " << queueSize 
	       << " integers. Transit of " << iterations << " in "
		<< sw.getElapsed() << " Seconds" << endl;

	    
	}
	
    }
    catch (Exception& e)
    {
	cout << "Exception: " << e.getMessage() << endl;
	exit(1);
    }

    cout << "+++++ passed all tests" << endl;

    return 0;
}

