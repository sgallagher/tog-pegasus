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
// Author: Bapu Patil, Hewlett-Packard Company (bapu_patil@hp.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include "LocalAuthUtils.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

/**
 *  A unique sequence number starts with  this.
 */
LocalAuthUtils::sequenceCount = 212;

/**
 * File path used to create random file
 *
 */
LocalAuthUtils::filepath="/tmp/cimclient";


LocalAuthUtils::LocalAuthUtils()
{
   cout << " In LocalAuthUtils Constructor" << endl;
   _randomToken = 0;
}

LocalAuthUtils::~LocalAuthUtils() { }

/**
 * create a file and make an random token data entry to it.
 * send the file name back to caller
 */
char* LocalAuthUtils::createFile(char* forUser)
{
    return(NULL);
}

/**
 * Create a random file and make generate random token and write
 * to this file
 */
Uint32 LocalAuthUtils::_makeEntry(char* fileName)
{
    Uint retCode = 0;

    return(retCode);
}

/**
 *
 *
 */
bool LocalAuthUtils::compareContents(char *recvData)
{
   
    if  ( strcmp(_randomTokenString,recvData) != 0 )
    {
         return false;
    }
    else
    {
         return true;
    }
}


/**
 * deletes the file that was created
 *
 */ 
bool LocalAuthUtils::deleteFile()
{
    //
    return true;
}

/**
 * changes the file owner to one specified
 *
 */ 
Uint32 LocalAuthUtils::_changeFileOwner(char* fileName)
{
    Uint32 retCode = 0;

    return(retCode);
}

/**
 * Generate a random token data
 *
 */
void LocalAuthUtils::_generateRandomToken()
{

}

/**
 *
 * Generate a random token data
 */
void LocalAuthUtils::_generateRandomTokenString()
{
}
PEGASUS_NAMESPACE_END
