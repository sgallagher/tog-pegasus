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
// Modified by:
//         Steve Hills (steve.hills@ncr.com)
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
	if( !(i == 6) )
		throw Exception( "!(i == 6)" );
	++i;
	if( !(i == 7) )
		throw Exception( "!(i == 7)" );
	i--;
	if( !(i == 6) )
		throw Exception( "!(i == 6)" );
	--i;
	if( !(i == 5) )
		throw Exception( "!(i == 5)" );

	if (verbose) {
		cout << "Testing: i+Uint32, i+AtomicInt, i-Uint32, etc.. "<<endl;
	}	
	if( !((i + 5) == 10) )
		throw Exception( "!((i + 5)" );
	if( !(i == 5) )
		throw Exception( "!(i == 5)" );
	j = 1;	
	if( !(i + j == 6) )
		throw Exception( "!(i + j == 6)" );
	if( !(j + i == 6) )
		throw Exception( "!(j + i == 6)" );
	if( !(i == 5 && j == 1) )
		throw Exception( "!(i == 5 && j == 1)" );
	i = j - 5; // Ugly.
	if( !(i.value() > 0) )
		throw Exception( "!(i.value() > 0)" );
	ii = 4;
	i += ii;
	if( !(i >= 0) )
		throw Exception( "!(i >= 0)" );
	if( !(ii < 5) )
		throw Exception( "!(ii < 5)" );
	// if( !(5 > ii) )
	jj = 2;
	ii+= jj + jj;
	if( !(ii == 8) )
		throw Exception( "!(ii == 8)" );
	if( !(jj == 2) )
		throw Exception( "!(jj == 2)" );

	i = 20;
	j = 10;
	ii = i + j;
	if( !(i == 20) )
		throw Exception( "!(i == 20)" );
	if( !(j == 10) )
		throw Exception( "!(j == 10)" );

	ii = i + 1;
	if( !(i == 20) )
		throw Exception( "!(i == 20)" );

	ii = i - j;
	if( !(i == 20) )
		throw Exception( "!(i == 20)" );

	ii = i - 1;
	if( !(i == 20) )
		throw Exception( "!(i == 20)" );
    }
    catch (Exception & e)
    {
        cout << "Exception: " << e.getMessage () << endl;
        exit (1);
    }
    cout << argv[0] << " +++++ passed all tests" << endl;

    return 0;
}
