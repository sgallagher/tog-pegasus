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
// Author: Konrad Rzeszutek, IBM Corp.
// Modified by:
//         Steve Hills (steve.hills@ncr.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <cassert>
#include <Pegasus/Common/IPC.h>
#include <Pegasus/Common/Thread.h>
#include <Pegasus/Common/InternalException.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

void test01()
{
    Boolean bad = false;
    try
    {
        Boolean verbose = (getenv("PEGASUS_TEST_VERBOSE")) ? true : false;
	        
	AtomicInt i,j,ii,jj;

	if (verbose) {
		cout << "Testing: i++, ++ii, i--, --i	"<< endl;
	}
	i = 5;
	i++;
	PEGASUS_ASSERT( i.get() == 6 );
	i++;
	PEGASUS_ASSERT( i.get() == 7 );
	i--;
	PEGASUS_ASSERT( i.get() == 6 );
	i--;
	PEGASUS_ASSERT( i.get() == 5 );

	if (verbose) {
		cout << "Testing: i+Uint32, i+AtomicInt, i-Uint32, etc.. "<<endl;
	}	
	PEGASUS_ASSERT( (i.get() + 5) == 10 );
	PEGASUS_ASSERT( i.get() == 5 );
	j = 1;	
	PEGASUS_ASSERT( i.get() + j.get() == 6 );
	PEGASUS_ASSERT( j.get() + i.get() == 6 );
	PEGASUS_ASSERT( i.get() == 5 && j.get() == 1 );
	i = j.get() - 5; // Ugly.
	PEGASUS_ASSERT( i.get() > 0 );
	ii = 4;
	i.set(i.get() + ii.get());
	// Always true anyway: PEGASUS_ASSERT( i.get() >= 0 );
	PEGASUS_ASSERT( ii.get() < 5 );
	//PEGASUS_ASSERT( 5 > ii.get() );
	jj = 2;
	ii.set(ii.get() + jj.get() + jj.get());
	PEGASUS_ASSERT( ii.get() == 8 );
	PEGASUS_ASSERT( jj.get() == 2 );

	i = 20;
	j = 10;
	ii = i.get() + j.get();
	PEGASUS_ASSERT( i.get() == 20 );
	PEGASUS_ASSERT( j.get() == 10 );

	ii = i.get() + 1;
	PEGASUS_ASSERT( i.get() == 20 );

	ii = i.get() - j.get();
	PEGASUS_ASSERT( i.get() == 20 );

	ii = i.get() - 1;
	PEGASUS_ASSERT( i.get() == 20 );
    }
    catch (Exception & e)
    {
        cout << "Exception: " << e.getMessage () << endl;
        exit (1);
    }
}

static AtomicInt _atomic_int(0);

PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL test_thread(void* parm)
{
    Thread* thread = (Thread*)parm;

    for (;;)
    {
	const size_t N = 100000000;

	for (size_t i = 0; i < N; i++)
	{
	    for (size_t i = 0; i < 3; i++)
	    {
		_atomic_int++;
	    }

	    for (size_t i = 0; i < 3; i++)
	    {
		_atomic_int.decAndTestIfZero();
	    }
	}

	printf("here: %d\n", _atomic_int.get());
	pegasus_sleep(1000);
    }

    return 0;
}

void test02()
{
    Thread t1(test_thread, 0, false);
    t1.run();

    Thread t2(test_thread, 0, false);
    t2.run();
}

int main(int argc, char** argv)
{
    test01();
//     test02();

    cout << argv[0] << " +++++ passed all tests" << endl;

    return 0;
}
