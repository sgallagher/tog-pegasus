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

#ifndef Pegasus_LocalAuthUtils_h
#define Pegasus_LocalAuthUtils_h

#include <fstream>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>
#include <time.h>
#include <sys/time.h>

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/System.h>

#include "Linkage.h"

/**
 * Maximum buffer size 
 */
#define MAX_BUFFER_SIZE 256

/**
 * Maximum password buffer size. Used by getpwnam_r call.
 */
#define MAX_PWD_BUFFER_SIZE 1024


PEGASUS_NAMESPACE_BEGIN
/**
    This class defines the necessary utilities used by VerifyAuthentication 
    module of LocalAuthenticator class.
*/

class PEGASUS_SECURITY_LINKAGE LocalAuthUtils
{
public:

    /**
        This class Constructor    
     */
    LocalAuthUtils();

      
    /**
        This class destructor    
     */
    ~LocalAuthUtils();

    /**
        Create a file and make a random token data entry to it. Send the file 
        name back to caller. This is called by Server side local 
        authenticator module to challenge client connecting to it.
     
        @param    forUser    User name passed by the caller
     
        @return   filename   Name of the file created if successfull 
                             else null.
                    
     */
    virtual char* createFile(char* forUser);
    
    /**
        Compares received data with the data it had generated previously. 
        This is called by Server side local authenticator module once
        the CIM Server receives the data sent by Client over a channel.
     
        @param    recvData   data for comparision
     
        @return   true       if successful 
                 false      else false 
                    
     */
    virtual Boolean compareContents(char* recvData);

    /**
        Deletes the file that was created. This is called by Server 
        side local authenticator module once the CIM Server done with 
        authenticating client.
     
        @return   true       if successful 
                  false      else false 
                    
     */
    virtual Boolean deleteFile();

    /**
       File path used to create random file
     
     */
    static const char filepath[];


private:

    /**
        Create a file, generate random token and write to this file
     
        @param    filename   Name of the file to be created
     
        @return   0          if successful 
                  1          if not successful
                    
     */
     virtual Uint32 _makeEntry(char*  fileName);

    /**
        Changes file owner to the current process owner and also set
        file permissions to read-only by the user
     
        @param    filename   Name of the file 
     
        @return   0          if successful 
                  1          if not successful
                    
     */
     virtual Uint32 _changeFileOwner(char* fileName);

    /**
        Generate  random token data which is long interger and set it
        it to _randomToken. This value is later written to
        file for challenging clients to read it.
     */
     virtual void _generateRandomToken();

    /**
        Generate  random token data which is string and set it
        it to _randomTokenString. This value is later written to
        file for challenging clients to read it.
     */
     virtual void _generateRandomTokenString();

    /**
        Used to store the CIM Client owner 
     */
    char  _user[MAX_BUFFER_SIZE]; 

    /**
        Used to store the random file name that will contain 
        random token data.
     */
    char  _fileName[MAX_BUFFER_SIZE]; 

    /**
        A unique random token data generated for each client connection.
     */
    long  _randomToken; 

    /**
        A unique random token data generated for each client connection.
     */
    char  _randomTokenString[MAX_BUFFER_SIZE];

};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_LocalAuthUtils_h */
