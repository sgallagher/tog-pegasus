//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software, Hewlett-Packard Company, IBM,
// The Open Group, Tivoli Systems
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
// Author: Konrad Rzeszutek
//
//
//%/////////////////////////////////////////////////////////////////////////////

#include <cassert>
#include <Pegasus/Common/IPC.h>
#include <Pegasus/Common/Exception.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

int main(int argc, char** argv)
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
	assert ( i == 6 );
	++i;
	assert ( i == 7 );
	i--;
	assert ( i == 6 );
	--i;
	assert ( i == 5 );

	if (verbose) {
		cout << "Testing: i+Uint32, i+AtomicInt, i-Uint32, etc.. "<<endl;
	}	
	assert ( (i + 5)  == 10);
	assert ( i == 5);
	j = 1;	
	assert ( i + j == 6 );
	assert ( j + i == 6 );
	assert ( i == 5 && j == 1);
	i = j - 5; // Ugly.
	assert ( i.value() > 0 );
	ii = 4;
	i += ii;
	assert ( i >= 0 );
	assert ( ii < 5 );
	// assert ( 5 > ii );
	jj = 2;
	ii+= jj + jj;
	assert ( ii == 8 );
	assert ( jj == 2 );
	 
    }
    catch (Exception & e)
    {
        cout << "Exception: " << e.getMessage () << endl;
        exit (1);
    }
    cout << argv[0] << " +++++ passed all tests" << endl;

    return 0;
}
