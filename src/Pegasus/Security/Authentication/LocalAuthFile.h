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
//                Nag Boranna, Hewlett-Packard Company (nagaraja_boranna@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_LocalAuthFile_h
#define Pegasus_LocalAuthFile_h

#include <fstream>
#include <stdlib.h>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/System.h>
#include "Linkage.h"

PEGASUS_NAMESPACE_BEGIN

/**
 * Maximum buffer size 
 */
#define MAX_BUFFER_SIZE 256

/**
 * Maximum password buffer size. Used by getpwnam_r call.
 */
#define MAX_PWD_BUFFER_SIZE 1024


/**
    This class provides the required functions to create, write and delete to a 
    file on UNIX file system.
*/

class PEGASUS_SECURITY_LINKAGE LocalAuthFile
{
public:

    /** Constructor. */
    LocalAuthFile(String userName);

      
    /** Destructor. */
    ~LocalAuthFile();

    /**
        Create a file and make a random token data entry to it. 
     
        @return   String    full path name of the file created if 
                            the file created successfull, null otherwise. 
    */
    String create();
    
    /**
        Removes the file created.
     
        @return   true      on successful removal of the file,
                            false otherwise.
    */
    Boolean remove();

    /**
        Get the string that was created as a challenge string
    */
    String getChallengeString();

private:

    /**
        Changes file owner to the current process owner and also set
        file permissions to read-only by that user.
     
        @param    filename   name of the file      
        @return   true on success, false on failure.
     */
     Boolean _changeFileOwner(char* fileName);

    /**
        Generate random token string.
     */
     String _generateRandomTokenString();

    /**
        Used to store the user name
     */
    char*  _user;

    /**
        Full path name of the file that is sent in the challenge.
    */
    String _filePathName;

    /**
        Challenge string that is written to the file.
    */
    String _challenge;

};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_LocalAuthFile_h */
