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
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_NamedPipe_h
#define Pegasus_NamedPipe_h

#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/Linkage.h>

#include <windows.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

const DWORD NAMEDPIPE_MAX_BUFFER_SIZE = 8192;

typedef struct
{
    HANDLE hpipe;      //Handle for Windows NamedPipe
    OVERLAPPED overlap;//holds information used for asynchronous input and output 
}NamedPipeRep;


class PEGASUS_COMMON_LINKAGE NamedPipe
{
public:
    static bool read(HANDLE pipe, Buffer & buffer);
    static bool write(HANDLE pipe, Buffer & buffer, LPOVERLAPPED overlap = NULL);

	//Returns the name of the Pipe created
    String getName(void)
    {
        return _name;
    }
    
	//Returns the handle of the Pipe
    HANDLE getPipe(void)
    {
        return _pipe.hpipe;
    }

    //Returns the pointer to the OVERLAPPED object
	LPOVERLAPPED getOverlap(void)
    {
        return &_pipe.overlap;
    }

    //Flag that indicates the type of NamedPipe
	//TRUE indicates that it is a connection pipe
	//FALSE indicates that the pipe is CIMOperation Pipe.
	Boolean isConnectionPipe;

	//The data read from pipe is saved in this data member.
    char raw[NAMEDPIPE_MAX_BUFFER_SIZE];

	//Size in bytes - information read from pipe.
    DWORD bytesRead;

	void setPipeHandle()
	{
		_pipe.hpipe=INVALID_HANDLE_VALUE;
	}

	//To check if the pipe is already serving a client.
	bool isUnderUse()
	{
		return _isBusy;
	}

	//To set the state of the pipe. TRUE indicates busy state.
	//FALSE indicates free and can be used.
	void setBusy(Boolean state = true)
	{
		_isBusy = state;
	}
   
	//Verify if the Pipe is connected and ready to listen.
	bool isConnected()
	{
		return _isConnected;
	}

	//Once the pipe is disconnected, update the flag to indicate 
	//that the pipe is not listening.
	void disconnect()
	{
		_isConnected = false;
	}
	
	//Once connected, update the flag to indicate that the pipe is listening
	void connected()
	{
		_isConnected = true;
	}

protected:
   String _name;
   NamedPipeRep _pipe;
   bool _isBusy; // Flag to indicate read operation in progress
   bool _isConnected; // Flag to indicate Pipe Handle has valid data

};

//Logical Server end point of the NamedPipe that is used for CIMOperation
//Referred to as OPERATION_PIPE
class PEGASUS_COMMON_LINKAGE NamedPipeServerEndPoint : public NamedPipe
{
public:
    NamedPipeServerEndPoint(String name, NamedPipeRep pipeStruct);
    ~NamedPipeServerEndPoint(void);

};

//Logical Client end point of the NamedPipe that is used for CIMOperation
//Referred to as OPERATION_PIPE

class PEGASUS_COMMON_LINKAGE NamedPipeClientEndPoint : public NamedPipe
{
public:
    NamedPipeClientEndPoint(String name, NamedPipeRep pipeStruct);
    ~NamedPipeClientEndPoint(void);

};

//Logical Server end point of the NamedPipe that is used for Connection
//establishment. Referred to as CONNECTION_PIPE

class PEGASUS_COMMON_LINKAGE NamedPipeServer : public NamedPipe
{
public:
    NamedPipeServer(const String & name);
    ~NamedPipeServer(void);

    NamedPipeServerEndPoint accept(void);

private:

    Boolean _connectToNamedPipe(HANDLE pipe, LPOVERLAPPED overlap);
};

class PEGASUS_COMMON_LINKAGE NamedPipeClient : public NamedPipe
{
public:
    NamedPipeClient(const String & name);
    ~NamedPipeClient(void);

    NamedPipeClientEndPoint connect(void);
    void disconnect(HANDLE pipe) const;


private:

};
#endif /* Pegasus_NamedPipe_h */
