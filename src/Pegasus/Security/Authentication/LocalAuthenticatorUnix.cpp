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


#include "LocalAuthenticator.h"

PEGASUS_NAMESPACE_BEGIN

/**
 *  Maximum data we can expect in a user name or file name that was sent
 *  over a channel.
 */
const Uint32 MAX_NAME_SIZE = 256;

/**
 *  Maximum data we can expect that was sent ver a channel while 
 *  challenging a client.
 */
const Uint32 MAX_DATA_SIZE = 1024;

/*
 *  constructor.
 */
LocalAuthenticator::LocalAuthenticator() { }

/*
 *  destructor.
 */
LocalAuthenticator::~LocalAuthenticator() { }

/*
 *  This is only called on server side.
 *  To ensure security over Client Local Channel connection, CIM Server
 *  verifies whether the Client that is connecting to it has permission
 *  or not. This is done by challenging CIM Clients over local channel.
 */
Boolean LocalAuthenticator::verifyAuthenticate(Channel* channel)
{

    Boolean isValid = false;
    char *fileName;
    char thisFileName[MAX_NAME_SIZE];
    char userName[MAX_NAME_SIZE];
    char recvData[MAX_DATA_SIZE];
    int bytesRead;


    // read User Name sent by the client
    bytesRead = channel->read(userName, sizeof(userName)-1);
    userName[bytesRead]=0;

         
    // create a file with random number
    LocalAuthUtils localAuthFile;
    fileName = localAuthFile.createFile((char *) userName);
    if (  fileName == NULL )
    {
           // TODO - LOG
           // cout << "Unauthorized client " << endl;

           return false;
    }

    fileName[strlen(fileName)]=0;
    strcpy(thisFileName, fileName);
    thisFileName[strlen(fileName)]=0;


    // write file name back to client
    int wrotebytes = channel->writeN(thisFileName, strlen(thisFileName));


    // read some data sent by the client
    bytesRead = channel->read(recvData, sizeof(recvData)-1);

    isValid = localAuthFile.compareContents((char*) recvData );

    // Now we will delete file
    Boolean isDelete = localAuthFile.deleteFile();
    if ( isDelete != true )
    {
       // TODO: Log
       // Unable to delete this file
    }

    return(isValid);
}

/*
 *  challenge client
 */
int LocalAuthenticator::sendChallenge(Channel* channel, char* data, int size)
{
    return (channel->write(data, size));
}

/*
 * recv response
 */
int LocalAuthenticator::recvResponse(Channel* channel, char* data)
{
    return (channel->read(data,  sizeof(data)));
}

/*
 *  recv challenge
 */
int LocalAuthenticator::recvChallenge(Channel* channel, char* data)
{
    return (channel->read(data, sizeof(data)) );
}

/*
 *  challenge client
 */
int LocalAuthenticator::sendResponse(Channel* channel, char* data, int size)
{
    return (channel->write(data, size));
}

/*
 *  This is only called on client side.
 *  To ensure security over Client Local Channel connection, CIM Client
 *  responds back to CIM Server challenge.
 *
 */
Boolean LocalAuthenticator::proveAuthenticate(Channel* channel, char* user)
{

   char tmpFileName[MAX_NAME_SIZE];
   char tmpBuffer[MAX_NAME_SIZE];
   ifstream infs;


   // TODO: Log
   //cout << "handling authentication for: "<< user << endl;

   //
   // Should handle Authentication ...
   // send user name
   //
   int wsize = channel->writeN(user, strlen(user));
   if ( wsize == -1)
   {
      // error out
      return false;
   }


   //  - Read the Data send by the CIM Server
   //      This is the name of the file
   memset(tmpFileName, 0, sizeof(tmpFileName));
   int rsize = 0;

   rsize =   channel->read(tmpFileName, sizeof(tmpFileName) - 1);
   if ( rsize == -1)
   {
      // LOG perror("Error reading channel");

      // error out
      return false;
   }
   else
   { 
      // LOG
      // cout << "filename:"<< tmpFileName << "size:" << rsize << endl;
   }

   
   
   //
   // Open file and read the data
   //
   infs.open(tmpFileName, ios::in);
   if ( !infs )
   {
      // unable to open file - TODO LOG
      // cout << tmpFileName << " : Open failed" << endl;
      // error out
      return false;
   }

   infs.getline(tmpBuffer, sizeof(tmpBuffer));
   
   channel->writeN(tmpBuffer, sizeof(tmpBuffer));

   // Just close after reading
   infs.close();

   return true;
}

PEGASUS_NAMESPACE_END
