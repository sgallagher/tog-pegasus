//%2006////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
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
// Author: Vageesh Umesh (vagumesh@in.ibm.com)
//
//%/////////////////////////////////////////////////////////////////////////////

/**
    This file has testcases that are added to cover the functions in
    Pegasus/Common/Cimom.cpp.
*/

#include <Pegasus/Common/PegasusAssert.h>
#include <Pegasus/Common/Cimom.h>
#include <Pegasus/Common/String.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

void test01()
{
    /**
        Added to cover the Function
        const String & message_module::get_name()
    */
    Uint32 capabilities = 0;
    Uint32 mask = 0;
    Uint32 queue = 0;
    message_module msg_mod("name",capabilities,mask,queue);
    PEGASUS_TEST_ASSERT( msg_mod.get_name() == "name");

    /**
        Added to cover the Function
        void message_module::put_name(String & name)
    */
    String newname = "newname";
    msg_mod.put_name(newname);
    PEGASUS_TEST_ASSERT( msg_mod.get_name() == "newname");

    /**
        Added to cover the Functions
        void message_module::put_capabilities(Uint32 capabilities)  and
        Uint32 message_module::get_capabilities()
    */
    msg_mod.put_capabilities(10);
    PEGASUS_TEST_ASSERT( msg_mod.get_capabilities() == 10 );

    /**
        Added to cover the Functions
        void message_module::put_mask(Uint32 mask)   and
        Uint32 message_module::get_mask()
    */
    msg_mod.put_mask(20);
    PEGASUS_TEST_ASSERT( msg_mod.get_mask() == 20 );

    /**
        Added to cover the Functions
        void message_module::put_queue(Uint32 queue)   and
        Uint32 message_module::get_queue() const { return _q_id ; }
    */
    msg_mod.put_queue(30);
    PEGASUS_TEST_ASSERT( msg_mod.get_queue() == 30 );

    /**
        Added to cover the Functions
        Boolean message_module::operator ==(Uint32 q) const
    */
    message_module dummy_mod(newname,10,20,30);
    PEGASUS_TEST_ASSERT( dummy_mod == 30 );

    /**
        Added to cover the Functions
        Boolean message_module::operator == (const message_module *mm) const
    */
    message_module &new_mod = dummy_mod;
    PEGASUS_TEST_ASSERT( dummy_mod == &new_mod );

    /**
        Added to cover the Functions
        Boolean message_module::operator == (const String & name ) const
    */
    String myString = "newname";
    PEGASUS_TEST_ASSERT( dummy_mod == myString );

    /**
        Added to cover the Functions
        Uint32 cimom::getModuleCount()
    */
    cimom obj;
    PEGASUS_TEST_ASSERT( obj.getModuleCount() == 0 );

    /**
        Added to cover the Function
        Uint32 cimom::getModuleIDs(Uint32 *ids, Uint32 count)
    */
    Uint32 ids = 0;
    Uint32 count = 0;
    Uint32 mod_ids = obj.getModuleIDs(0,0);
    PEGASUS_TEST_ASSERT( mod_ids == 0 );
    //Added to cover False condition
    PEGASUS_TEST_ASSERT( obj.getModuleIDs(&ids,count) == 0 );

    /**
        Added to cover thw Function
        void cimom::set_default_op_timeout(const struct timeval *buffer)  and
        void cimom::get_default_op_timeout(struct timeval *timeout) const
    */
    struct timeval buffer;
    buffer.tv_sec = 10;
    buffer.tv_usec = 20;
    obj.set_default_op_timeout(&buffer);

    struct timeval timeout;
    obj.get_default_op_timeout(&timeout);
    PEGASUS_TEST_ASSERT( timeout.tv_sec == 10 && timeout.tv_usec == 20 );

    /**
        Added to cover the Function
        Boolean cimom::moduleChange(struct timeval last)
    */
    Boolean mod_change = obj.moduleChange(buffer);
    PEGASUS_TEST_ASSERT( mod_change == true );
}

int main( int argc, char *argv[] )
{
    try
    {
       test01();
    }
    catch (Exception& e)
    {
        PEGASUS_STD (cout) << "Exception: " << e.getMessage() 
        << PEGASUS_STD(endl);
        exit(1);
    }

    PEGASUS_STD (cout) << argv[0] << " +++++ passed all tests" 
        << PEGASUS_STD(endl);
    return 0;
}

