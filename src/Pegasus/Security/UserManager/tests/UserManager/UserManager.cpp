//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 BMC Software, Hewlett-Packard Company, IBM,
// The Open Group, Tivoli Systems
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies of substantial portions of this software.
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
// Author: Sushma Fernandes, Hewlett-Packard Company 
//         (sushma_fernandes@hp.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <cassert>
#include <iostream>
#include <Pegasus/Common/String.h>
#include <Pegasus/Security/UserManager/UserManager.h>
#include <Pegasus/Security/UserManager/UserExceptions.h>

PEGASUS_USING_PEGASUS;

PEGASUS_USING_STD;

static const String GOOD_USER       = "guest";

static const String BAD_USER        = "nosuchuser";

//
// main
//

int main()
{
#ifdef PEGASUS_OS_HPUX
    UserManager* 	userManager = UserManager::getInstance(); 
    Boolean      	exceptionFlag = false;
    Array<String> 	userNames;

    try 
    {
        //
	// Try to add an invalid system user
        //
        userManager->addUser(BAD_USER, BAD_USER);
    }
    catch (InvalidSystemUser& isu)
    {
        exceptionFlag = true;
    }
    assert( exceptionFlag == true );

    exceptionFlag = false;
    try 
    {
        //
	// Try to modify an invalid user
        //
        userManager->modifyUser(BAD_USER, BAD_USER, BAD_USER);
    }
    catch (Exception& e)
    {
        exceptionFlag = true;
    }
    assert( exceptionFlag == true );

    exceptionFlag = false;
    try
    {
        //
        // Try to remove an invalid user
        //
        userManager->removeUser(BAD_USER);
    }
    catch (Exception& e)
    {
        exceptionFlag = true;
    }
    assert( exceptionFlag == true );

    exceptionFlag = false;
    try
    {
        //
        // Try to list users
        //
        userManager->getAllUserNames(userNames);
    }
    catch (Exception& e)
    {
        exceptionFlag = true;
    }
    assert( exceptionFlag == false );

    //
    // Positive tests
    //
     
    exceptionFlag = false;
    try
    {
        //
        // Try to add a valid system user
        //
        userManager->addUser(GOOD_USER, GOOD_USER);
    }
    catch (DuplicateUser& du)
    {
    }
    catch (Exception& e)
    {
        exceptionFlag = true;
    }
    assert( exceptionFlag == false );

    exceptionFlag = false;
    try
    {
        //
        // Try to modify a valid user's password
        //
        userManager->modifyUser(GOOD_USER, GOOD_USER, BAD_USER);
    }
    catch (PasswordMismatch& pm)
    {
    }
    catch (Exception& e)
    {
        exceptionFlag = true;
    }
    assert( exceptionFlag == false );

    exceptionFlag = false;
    try
    {
        //
        // Try to remove a valid user
        //
        userManager->removeUser(GOOD_USER);
    }
    catch (Exception& e)
    {
        exceptionFlag = true;
    }
    assert( exceptionFlag == false );

    exceptionFlag = false;
    try
    {
        //
        // Try to list users
        //
        userManager->getAllUserNames(userNames);
    }
    catch (Exception& e)
    {
        exceptionFlag = true;
    }
    assert( exceptionFlag == false );
    cout << "+++++ passed all tests" << endl;
    return 0;
#else
    cout << "+++++ passed all tests" << endl;
    return 0;
#endif 
}
