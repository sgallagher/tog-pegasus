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
// Author: Carol Ann Krug Graves, Hewlett-Packard Company 
//         (carolann_graves@hp.com)
//
// Modified By: Sean Keenan (sean.keenan@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

//
//  This test module tests the functions associated with the CIMFlavor class.
//

#include <cassert>
#include <Pegasus/Common/CIMFlavor.h>
#include <Pegasus/Common/Exception.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

static char * verbose;
static const CIMFlavor CIMFLAVOR_ALL = CIMFlavor::OVERRIDABLE +
    CIMFlavor::TOSUBCLASS + CIMFlavor::TOINSTANCE + CIMFlavor::TRANSLATABLE +
    CIMFlavor::DISABLEOVERRIDE + CIMFlavor::RESTRICTED;

void test01 ()
{
    #if defined(PEGASUS_PLATFORM_DARWIN_PPC_GNU) || defined (PEGASUS_OS_VMS)
	static const CIMFlavor CIMFLAVOR_ALL = CIMFlavor::OVERRIDABLE +
    CIMFlavor::TOSUBCLASS + CIMFlavor::TOINSTANCE + CIMFlavor::TRANSLATABLE +
    CIMFlavor::DISABLEOVERRIDE + CIMFlavor::RESTRICTED;
    #endif
    //
    //  Test default constructor
    //
    CIMFlavor f0;

    if (verbose)
    {
	PEGASUS_STD (cout) << "\n----------------------\n";
        PEGASUS_STD (cout) << "f0: " << f0.toString () << PEGASUS_STD (endl);
    }
    assert (f0.toString () == String::EMPTY);

    CIMFlavor f1 = CIMFlavor ();

    if (verbose)
    {
	PEGASUS_STD (cout) << "\n----------------------\n";
        PEGASUS_STD (cout) << "f1: " << f1.toString () << PEGASUS_STD (endl);
    }
    assert (f1.toString () == String::EMPTY);

    //
    //  Test Uint32 constructor
    //
    CIMFlavor f2 = CIMFlavor (CIMFlavor::NONE);

    if (verbose)
    {
	PEGASUS_STD (cout) << "\n----------------------\n";
        PEGASUS_STD (cout) << "f2: " << f2.toString () << PEGASUS_STD (endl);
    }
    assert (f2.toString () == String::EMPTY);

    CIMFlavor f3 = CIMFlavor (CIMFlavor::DISABLEOVERRIDE);

    if (verbose)
    {
	PEGASUS_STD (cout) << "\n----------------------\n";
        PEGASUS_STD (cout) << "f3: " << f3.toString () << PEGASUS_STD (endl);
    }
    assert (f3.toString () == "DISABLEOVERRIDE");

    CIMFlavor f4 = CIMFlavor (CIMFLAVOR_ALL);

    if (verbose)
    {
	PEGASUS_STD (cout) << "\n----------------------\n";
        PEGASUS_STD (cout) << "f4: " << f4.toString () << PEGASUS_STD (endl);
    }
    assert (f4.toString () == 
   "OVERRIDABLE TOSUBCLASS TOINSTANCE TRANSLATABLE DISABLEOVERRIDE RESTRICTED");

    CIMFlavor f5 = CIMFlavor 
        (CIMFlavor::DISABLEOVERRIDE + CIMFlavor::RESTRICTED);

    if (verbose)
    {
	PEGASUS_STD (cout) << "\n----------------------\n";
        PEGASUS_STD (cout) << "f5: " << f5.toString () << PEGASUS_STD (endl);
    }
    assert (f5.toString () == "DISABLEOVERRIDE RESTRICTED");

    //
    //  Test copy constructor
    //
    CIMFlavor f6 = CIMFlavor (f1);

    if (verbose)
    {
	PEGASUS_STD (cout) << "\n----------------------\n";
        PEGASUS_STD (cout) << "f6: " << f6.toString () << PEGASUS_STD (endl);
    }
    assert (f6.toString () == String::EMPTY);

    CIMFlavor f7 = CIMFlavor (f2);

    if (verbose)
    {
	PEGASUS_STD (cout) << "\n----------------------\n";
        PEGASUS_STD (cout) << "f7: " << f7.toString () << PEGASUS_STD (endl);
    }
    assert (f7.toString () == String::EMPTY);

    CIMFlavor f8 = CIMFlavor (f3);

    if (verbose)
    {
	PEGASUS_STD (cout) << "\n----------------------\n";
        PEGASUS_STD (cout) << "f8: " << f8.toString () << PEGASUS_STD (endl);
    }
    assert (f8.toString () == "DISABLEOVERRIDE");

    CIMFlavor f9 = CIMFlavor (f4);

    if (verbose)
    {
	PEGASUS_STD (cout) << "\n----------------------\n";
        PEGASUS_STD (cout) << "f9: " << f9.toString () << PEGASUS_STD (endl);
    }
    assert (f9.toString () == 
   "OVERRIDABLE TOSUBCLASS TOINSTANCE TRANSLATABLE DISABLEOVERRIDE RESTRICTED");

    CIMFlavor f10 = CIMFlavor (f5);

    if (verbose)
    {
	PEGASUS_STD (cout) << "\n----------------------\n";
        PEGASUS_STD (cout) << "f10: " << f10.toString () << PEGASUS_STD (endl);
    }
    assert (f10.toString () == "DISABLEOVERRIDE RESTRICTED");

    //
    //  Test assignment operator
    //
    f0 = f4;

    if (verbose)
    {
	PEGASUS_STD (cout) << "\n----------------------\n";
        PEGASUS_STD (cout) << "f0: " << f0.toString () << PEGASUS_STD (endl);
    }
    assert (f0.toString () == 
   "OVERRIDABLE TOSUBCLASS TOINSTANCE TRANSLATABLE DISABLEOVERRIDE RESTRICTED");

    f0 = f3;

    if (verbose)
    {
	PEGASUS_STD (cout) << "\n----------------------\n";
        PEGASUS_STD (cout) << "f0: " << f0.toString () << PEGASUS_STD (endl);
    }
    assert (f0.toString () == "DISABLEOVERRIDE");

    //
    //  Test equal (CIMFlavor) 
    //
    assert (f1.equal (f2));
    assert (f3.equal (f0));
    assert (!(f1.equal (f5)));
    assert (!(f9.equal (f10)));

    //
    //  Test addFlavor (Uint32) 
    //
    f0.addFlavor (CIMFlavor::TRANSLATABLE);

    if (verbose)
    {
	PEGASUS_STD (cout) << "\n----------------------\n";
        PEGASUS_STD (cout) << "f0: " << f0.toString () << PEGASUS_STD (endl);
    }
    assert (f0.toString () == "TRANSLATABLE DISABLEOVERRIDE");

    f7.addFlavor 
        (CIMFlavor::TOSUBCLASS + CIMFlavor::TOINSTANCE + 
         CIMFlavor::TRANSLATABLE);

    if (verbose)
    {
	PEGASUS_STD (cout) << "\n----------------------\n";
        PEGASUS_STD (cout) << "f7: " << f7.toString () << PEGASUS_STD (endl);
    }
    assert (f7.toString () == "TOSUBCLASS TOINSTANCE TRANSLATABLE");

    f4.addFlavor (CIMFlavor::TRANSLATABLE);

    if (verbose)
    {
	PEGASUS_STD (cout) << "\n----------------------\n";
        PEGASUS_STD (cout) << "f4: " << f4.toString () << PEGASUS_STD (endl);
    }
    assert (f4.toString () == 
   "OVERRIDABLE TOSUBCLASS TOINSTANCE TRANSLATABLE DISABLEOVERRIDE RESTRICTED");

    f10.addFlavor (CIMFlavor::NONE);

    if (verbose)
    {
	PEGASUS_STD (cout) << "\n----------------------\n";
        PEGASUS_STD (cout) << "f10: " << f10.toString () << PEGASUS_STD (endl);
    }
    assert (f10.toString () == "DISABLEOVERRIDE RESTRICTED");

    f2.addFlavor (CIMFLAVOR_ALL);

    if (verbose)
    {
	PEGASUS_STD (cout) << "\n----------------------\n";
        PEGASUS_STD (cout) << "f2: " << f2.toString () << PEGASUS_STD (endl);
    }
    assert (f2.toString () == 
   "OVERRIDABLE TOSUBCLASS TOINSTANCE TRANSLATABLE DISABLEOVERRIDE RESTRICTED");

    //
    //  Test hasFlavor (Uint32) 
    //
    assert (f0.hasFlavor (CIMFlavor::TRANSLATABLE));
    assert (f0.hasFlavor 
        (CIMFlavor::TRANSLATABLE + CIMFlavor::DISABLEOVERRIDE));
    assert (!(f0.hasFlavor (CIMFlavor::OVERRIDABLE)));
    assert (!(f0.hasFlavor (CIMFlavor::OVERRIDABLE + CIMFlavor::TOINSTANCE)));
    assert (!(f0.hasFlavor (CIMFLAVOR_ALL)));

    assert (f7.hasFlavor (CIMFlavor::TOSUBCLASS));
    assert (f7.hasFlavor (CIMFlavor::TOSUBCLASS + CIMFlavor::TOINSTANCE));
    assert (!(f7.hasFlavor (CIMFlavor::RESTRICTED)));
    assert (!(f7.hasFlavor 
        (CIMFlavor::RESTRICTED + CIMFlavor::DISABLEOVERRIDE)));
    assert (!(f7.hasFlavor (CIMFLAVOR_ALL)));

    assert (f4.hasFlavor (CIMFLAVOR_ALL));
    assert (f4.hasFlavor (CIMFlavor::OVERRIDABLE));
    assert (f4.hasFlavor (CIMFlavor::OVERRIDABLE + CIMFlavor::TOSUBCLASS));

    //
    //  Test hasFlavor (CIMFlavor) 
    //
    assert (f0.hasFlavor (f8));
    assert (f4.hasFlavor (f9));
    assert (!(f0.hasFlavor (f7)));
    assert (!(f0.hasFlavor (f4)));
    assert (!(f7.hasFlavor (f5)));

    //
    //  Test removeFlavor (Uint32) 
    //
    f6.removeFlavor (CIMFLAVOR_ALL);
    if (verbose)
    {
	PEGASUS_STD (cout) << "\n----------------------\n";
        PEGASUS_STD (cout) << "f6: " << f6.toString () << PEGASUS_STD (endl);
    }
    assert (f6.toString () == String::EMPTY);

    f6.removeFlavor (CIMFlavor::NONE);
    if (verbose)
    {
	PEGASUS_STD (cout) << "\n----------------------\n";
        PEGASUS_STD (cout) << "f6: " << f6.toString () << PEGASUS_STD (endl);
    }
    assert (f6.toString () == String::EMPTY);

    f2.removeFlavor (CIMFlavor::NONE);
    if (verbose)
    {
	PEGASUS_STD (cout) << "\n----------------------\n";
        PEGASUS_STD (cout) << "f2: " << f2.toString () << PEGASUS_STD (endl);
    }
    assert (f2.toString () == 
   "OVERRIDABLE TOSUBCLASS TOINSTANCE TRANSLATABLE DISABLEOVERRIDE RESTRICTED");

    f2.removeFlavor (CIMFlavor::TOINSTANCE);
    if (verbose)
    {
	PEGASUS_STD (cout) << "\n----------------------\n";
        PEGASUS_STD (cout) << "f2: " << f2.toString () << PEGASUS_STD (endl);
    }
    assert (f2.toString () == 
        "OVERRIDABLE TOSUBCLASS TRANSLATABLE DISABLEOVERRIDE RESTRICTED");

    f2.removeFlavor (CIMFlavor::DISABLEOVERRIDE + CIMFlavor::RESTRICTED);
    if (verbose)
    {
	PEGASUS_STD (cout) << "\n----------------------\n";
        PEGASUS_STD (cout) << "f2: " << f2.toString () << PEGASUS_STD (endl);
    }
    assert (f2.toString () == "OVERRIDABLE TOSUBCLASS TRANSLATABLE");

    f2.removeFlavor (CIMFLAVOR_ALL);
    if (verbose)
    {
	PEGASUS_STD (cout) << "\n----------------------\n";
        PEGASUS_STD (cout) << "f2: " << f2.toString () << PEGASUS_STD (endl);
    }
    assert (f2.toString () == String::EMPTY);

    f8.removeFlavor (CIMFlavor::DISABLEOVERRIDE);
    if (verbose)
    {
	PEGASUS_STD (cout) << "\n----------------------\n";
        PEGASUS_STD (cout) << "f8: " << f8.toString () << PEGASUS_STD (endl);
    }
    assert (f8.toString () == String::EMPTY);


    //
    //  Test toString ()
    //
    String sf1 = f1.toString ();
    assert (sf1 == String::EMPTY);

    String sf3 = f3.toString ();
    assert (sf3 == "DISABLEOVERRIDE");

    String sf7 = f7.toString ();
    assert (sf7 == "TOSUBCLASS TOINSTANCE TRANSLATABLE");

    String sf4 = f4.toString ();
    assert (sf4 == "OVERRIDABLE TOSUBCLASS TOINSTANCE TRANSLATABLE "
                       "DISABLEOVERRIDE RESTRICTED");
}

int main (int argc, char** argv)
{
    verbose = getenv ("PEGASUS_TEST_VERBOSE");

    try
    {
        test01 ();

        PEGASUS_STD (cout) << argv [0] << " +++++ passed all tests" 
                           << PEGASUS_STD (endl);
    }
    catch (Exception & e)
    {
        PEGASUS_STD (cerr) << argv [0] << " Exception " << e.getMessage () 
                           << PEGASUS_STD (endl);
        exit (1);
    }
    return 0;
}
