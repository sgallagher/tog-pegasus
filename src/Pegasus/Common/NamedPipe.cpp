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

#pragma warning(disable:4530)

#include <Pegasus/Common/NamedPipe.h>
#include <Pegasus/Common/Monitor.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

//Timeout for connection establishment
const static DWORD MAX_TIMEOUT = 30000;     // 30 seconds

//Used for connection establishment request by the client.
//Server verifies the request content
const static char* CONNECT_REQUEST = "<connect-request>";
const static char* CONNECT_RESPONSE = "<connect-response>";
const static char* DISCONNECT_REQUEST = "<disconnect-request>";
const static char* DISCONNECT_RESPONSE = "<disconnect-response>";

static inline String _CONNECTION_PIPE_NAME(const String & name)
{
    return(name + "0");
}

static inline String _OPERATION_PIPE_NAME(const String & name)
{
    return(name + "1");
}

////////////////////////////////////////////////////////////////////////////
// Method Name      : read
// Input Parameter  : pipe   - type HANDLE
// Output Parameter : buffer - type Reference to String
// Return Type      : bool
// -----------------------------------------------------------------------
// The method reads the data from the pipe and populates the data
// into buffer. If status of read is returned as the return value
///////////////////////////////////////////////////////////////////////////

bool NamedPipe::read(HANDLE pipe, Buffer & buffer)
{
    // clear buffer
    buffer.clear();
    Boolean readStatus = false;
	char raw[NAMEDPIPE_MAX_BUFFER_SIZE];
	DWORD size = 0;
    for ( ; ; )
    {
        // read all data in pipe
        strcpy(raw,"");
        BOOL rc = ::ReadFile(
                pipe,
                (void *)raw,
                NAMEDPIPE_MAX_BUFFER_SIZE,
                &size,
                0);

        // only fail is ::ReadFile returns false and some error other than more data
        if ((rc == FALSE) && (::GetLastError() != ERROR_MORE_DATA))
        {
			break;
        }

        buffer.append(raw, strlen(raw));

        // check for message complete
        if (rc == TRUE)
        {
			readStatus = true;
            break;
        }
    }
    return readStatus;
}

//////////////////////////////////////////////////////////////////////////////
// Method Name      : write
// Input Parameter  : pipe    - type HANDLE
//                    buffer  - type Reference to String
//                    overlap - LPOVERLAPPED
// Return Type      : bool
// ----------------------------------------------------------------------------
// The method writes the data from the buffer onto the pipe
// Once the data is written to pipe, the writer waits untill
// data is read by the peer. Status of write is returned as the return value
///////////////////////////////////////////////////////////////////////////////

bool NamedPipe::write(HANDLE pipe, String & buffer, LPOVERLAPPED overlap)
{
    DWORD size = 0;

    BOOL rc =
        ::WriteFile(
            pipe,
            buffer.getCString(),
            buffer.size(),
            &size,
            overlap);     //this should be the overlap

    if (!rc)
    {
		DWORD dwResult = GetLastError();
        if (dwResult != ERROR_NO_DATA)
        {
#ifdef PEGASUS_LOCALDOMAINSOCKET_DEBUG 
			const char* lpMsgBuf;
            LPVOID lpDisplayBuf;
            
            FormatMessage(
               FORMAT_MESSAGE_ALLOCATE_BUFFER |
               FORMAT_MESSAGE_FROM_SYSTEM,
               NULL,
               dwResult,
               MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
               (LPTSTR) &lpMsgBuf,
               0, NULL );

            lpDisplayBuf = LocalAlloc(LMEM_ZEROINIT,
                           (strlen(lpMsgBuf)+90)*sizeof(TCHAR));
			Tracer::trace(TRC_HTTP, Tracer::LEVEL2, "WriteFile in NamedPipe::write failed with error \
                   %d: %s", dwResult, lpMsgBuf);
 
            LocalFree(lpDisplayBuf);
#endif 
			return false;
		}
    }

    return true;
}

//////////////////////////////////////////////////////////////////////////////
// Method Name      : NamedPipeServer
// Input Parameter  : pipeName    - type String
// 
// ----------------------------------------------------------------------------
// The method Creates the Connection pipe. Creating pipe does not mean that it 
// is ready to accept requests since this only creates a file. Server has to 
// connect to the pipe and complete the initiation process
///////////////////////////////////////////////////////////////////////////////

NamedPipeServer::NamedPipeServer(const String & pipeName)
{
    _name = pipeName;
    isConnectionPipe = true;
    _pipe.hpipe = 0;

    // create a primary named pipe to listen for connection requests
    _pipe.hpipe  =
        ::CreateNamedPipe(
            _CONNECTION_PIPE_NAME(_name).getCString(),
            PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
            PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
            MAX_PIPE_INSTANCES,
            NAMEDPIPE_MAX_BUFFER_SIZE,
            NAMEDPIPE_MAX_BUFFER_SIZE,
            MAX_TIMEOUT,
            0);

    if (_pipe.hpipe == INVALID_HANDLE_VALUE)
    {
        throw Exception("Could not Create Pipe");
    }

    _pipe.overlap.Offset = 0;
    _pipe.overlap.OffsetHigh = 0;

    _pipe.overlap.hEvent = CreateEvent(NULL,    // default security attribute
                                       FALSE,
                                       FALSE,
                                       NULL);   // unnamed event object

    if (_pipe.overlap.hEvent == NULL)
    {
        throw Exception("CreateEvet failed in NamedPipeServer constructor");
    }

    Boolean bIsconnected = false;

    bIsconnected = _connectToNamedPipe( _pipe.hpipe, &_pipe.overlap);
    if (!bIsconnected)
    {
        //SHOULD THROW AN EXCEPTION HERE
        throw(Exception("NamedPipeServer::accept Connection - Pipe Failed to \
                         reconnect."));

    }

}

NamedPipeServer::~NamedPipeServer(void)
{
}

//////////////////////////////////////////////////////////////////////////////
// Method Name      : accept
// Input Parameter  : void
// Return Type      : NamedPipeServerEndPoint
// ----------------------------------------------------------------------------
// The method reads the data written onto the Connection Pipe.
// Server checks if it is request for establishing the connection. If yes, it 
// creates an instance of Operation pipe. Once the Operation Pipe is created
// server then responds to client with a challenge response by writing 
// CONNECT-RESPONSE onto the Connection pipe indicating that the server has 
// created the server end of the Operation pipe and client can now start 
// placing the CIMOperation requests. The Server end point is returned to
// HTTPAcceptor.
///////////////////////////////////////////////////////////////////////////////

NamedPipeServerEndPoint NamedPipeServer::accept(void)
{

    Boolean bIsconnected = false;
    Boolean ConnectFailed = false;

    Buffer request(CONNECT_REQUEST, strlen(CONNECT_REQUEST));
    String response(CONNECT_RESPONSE);
    setBusy();
    
	// get request
    if (!NamedPipe::read(_pipe.hpipe, request))
    {
	    ::DisconnectNamedPipe(_pipe.hpipe);
        bIsconnected = _connectToNamedPipe( _pipe.hpipe, &_pipe.overlap);
        if (!bIsconnected)
        {
            //should throw an exception here
             throw(Exception("NamedPipeServer::accept Primary - Pipe Failed to \
                             reconnect."));
        }

        throw(Exception("NamedPipeServer::accept Connection - Pipe Failed to \
                        reconnect."));
    }

    //If client has requested for establishing the connection  
    if (strcmp(request.getData(), CONNECT_REQUEST))
    {
        ::DisconnectNamedPipe(_pipe.hpipe);
        bIsconnected = _connectToNamedPipe( _pipe.hpipe, &_pipe.overlap);

        if (!bIsconnected)
        {
           //should throw an exception here
            throw(Exception("NamedPipeServer::accept() - Primary Pipe Failed\
                             to reconnect."));
        }

        throw(Exception("NamedPipeServer::accept() - Primary Pipe Failed \
                        to reconnect."));
    }


    NamedPipeRep* pipe2 = new NamedPipeRep;

    pipe2->overlap.Offset = 0;
    pipe2->overlap.OffsetHigh = 0;

    // create a Operation pipe for processing requests
    pipe2->hpipe =
    ::CreateNamedPipe(
            _OPERATION_PIPE_NAME(_name).getCString(),
            PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
            PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
            PIPE_UNLIMITED_INSTANCES,
            NAMEDPIPE_MAX_BUFFER_SIZE,
            NAMEDPIPE_MAX_BUFFER_SIZE,
            MAX_TIMEOUT,
            0);

    if (pipe2->hpipe == INVALID_HANDLE_VALUE)
    {
        ::DisconnectNamedPipe(_pipe.hpipe);
        bIsconnected = _connectToNamedPipe( _pipe.hpipe, &_pipe.overlap);
        if (!bIsconnected)
        {
           //should throw an exception here
           throw(Exception("NamedPipeServer::accept() - Primary Pipe Failed\
                            to reconnect."));
        }
        // Failed to create Operation Pipe
        throw Exception("Failed to Create Operation Pipe");

    }
    
	// perform handshake
    if(!NamedPipe::write(_pipe.hpipe, response))
    {
        ::DisconnectNamedPipe(_pipe.hpipe);
        ::CloseHandle(pipe2->hpipe);
        bIsconnected = _connectToNamedPipe( _pipe.hpipe, &_pipe.overlap);
        if (!bIsconnected)
        {
            //should throw an exception here
            throw(Exception("NamedPipeServer::accept() - Primary Pipe Failed\
                            to reconnect."));
        }
        throw(Exception("NamedPipeServer::accept() - Primary Pipe Failed to\
                        reconnect."));
    }

	//Create an event object and associate this event object
    pipe2->overlap.hEvent = CreateEvent(NULL,    // default security attribute
                                       FALSE,
                                       FALSE,
                                       NULL);   // unnamed event object

    if (pipe2->overlap.hEvent == NULL)
    {
        throw(Exception("NamedPipeServer::accept() - Primary Pipe Failed to\
                        create event."));
    }

    //Operation Pipe successfully created. Connect to this pipe
	//to read the CIMOperation request
    bIsconnected = _connectToNamedPipe(pipe2->hpipe, &pipe2->overlap);

    if (!bIsconnected)
    {
        ::DisconnectNamedPipe(_pipe.hpipe);
        ::CloseHandle(pipe2->hpipe);
        bIsconnected = _connectToNamedPipe( _pipe.hpipe, &_pipe.overlap);
        if (!bIsconnected)
        {
            //should throw an exception here
            throw(Exception("NamedPipeServer::accept() - Primary Pipe Failed\
                            to reconnect."));
        }

        throw(Exception("NamedPipeServer::accept() - Primary Pipe Failed to \
                        reconnect."));
    }

    //If the pipe that is already connected to a client then disconnect 
	//primary pipe so it can respond to other requests. Unless DisconnectNamedPipe
	//is called, the pipe would still be associated with the previous client
    ::DisconnectNamedPipe(_pipe.hpipe);

    bIsconnected = _connectToNamedPipe( _pipe.hpipe, &_pipe.overlap);
    if (!bIsconnected)
    {
        //should throw an exception here
        throw(Exception("NamedPipeServer::accept() - Primary Pipe Failed to reconnect."));
    }

    // the caller is responsible for disconnecting the secondary pipe
    // and closing the pipe

    setBusy(FALSE);
    return(NamedPipeServerEndPoint(String("Operationpipe"), *pipe2));
}

//////////////////////////////////////////////////////////////////////////////
// Method Name      : _connectToNamedPipe
// Input Parameter  : pipe    - type HANDLE
//                    overlap - type LPOVERLAPPED
//
// Return Type      : Boolean
// ----------------------------------------------------------------------------
// 
// Server connects to the Connection pipe by invoking ConnectNamedPipe api. On
// success returns true, false otherwise. If the server fails to connect, it
// closes the file handle of the Connection Pipe.
///////////////////////////////////////////////////////////////////////////////

Boolean NamedPipeServer::_connectToNamedPipe(HANDLE pipe, LPOVERLAPPED overlap)
{

    Boolean bIsconnected = false;


    if (!ConnectNamedPipe(pipe, overlap))
    {
  
		switch (GetLastError())
		{
			//ABB: If the overlapped connection in progress.
			case ERROR_IO_PENDING:
				{
					bIsconnected = true;
					break;
				}
			///////////////////////////////////////////////////////////// 
			//There are two reason for this case
			//SCENARIO  1: If the Client is already connected at the other 
			//end of pipe and server connects later,so signal an event. 
			//NOTE: This is not an error condition
			//===========================================================
			//SCENARIO	2: If the Server executes ConnectNamedPipe without
			//disconnecting from the server end of previous client while the
			//client has already closed the pipe at its end.
			//////////////////////////////////////////////////////////////
			case ERROR_PIPE_CONNECTED:
				{
					bIsconnected = true;
					if (SetEvent(overlap->hEvent))
					{
						break;
					}
				}
	        
			//Server has already connected to the server end of the pipe. 
			//client has not connected at the other end while server executes
			//ConnectNamedPipe again.
			//NOTE: This is not an error condition

			case ERROR_PIPE_LISTENING:
				{
					bIsconnected = true;
					break;
				}

			//ABB: If an error occurs during the connect operation...
			default:
				{
					//return -1;
					bIsconnected = false;
					break;
				}
		}
    }

	//If the server was unable to connect, close the file handle
    if (!bIsconnected)
    {
        ::CloseHandle(pipe);
        return bIsconnected;
    }
    connected();
    return bIsconnected;

}

//Client end of the NamedPipe. We don't use any dedicated connection pipe
//at the Client end. We use a dedicated Operation Pipe at client, we connect
//to end point of Operation Pipe


NamedPipeClient::NamedPipeClient(const String & name)
{
	// Set a flag to indicate that we are creating the Operation Pipe
    isConnectionPipe = false;
    _name = (name);
}

NamedPipeClient::~NamedPipeClient(void)
{
}

NamedPipeClientEndPiont NamedPipeClient::connect(void)
{
    
    string request(CONNECT_REQUEST);
    string response(CONNECT_RESPONSE);

    DWORD size = 0;
    //Try connecting to the Primary Pipe requesting for connection
	//This would invoke connect and write in a single call.
	//Perform a handshake of connection establishment.
    BOOL rc =
        ::CallNamedPipe(
            _CONNECTION_PIPE_NAME(_name).getCString(),
            (void *)request.data(),
            request.size(),
            (void *)response.data(),
            response.size(),
            &size,
            MAX_TIMEOUT);

    if (rc == 0)
    {
        throw(Exception("NamedPipeClient::connect() - failed to call \
                        primary pipe"));
    }

    // Write the Challenge request the Server requesting the connection to
	// be accepted
    if (strcmp (response.data(), CONNECT_RESPONSE))
    {
        throw(Exception("NamedPipeClient::connect() - Incorrect response"));
    }

    NamedPipeRep* pipe2 = new NamedPipeRep;
    pipe2->overlap.Offset = 0;
    pipe2->overlap.OffsetHigh = 0;
    for ( ; ; )
    {
        //Create the Client end point of Operation Pipe
		pipe2->hpipe =
            ::CreateFile(
                _OPERATION_PIPE_NAME(_name).getCString(),
                GENERIC_READ | GENERIC_WRITE,
                0,
                0,
                OPEN_EXISTING,
                FILE_FLAG_OVERLAPPED, //for non blocking mode
                0);

        if (pipe2->hpipe != INVALID_HANDLE_VALUE)
        {
            break;
        }

        if (::GetLastError() != ERROR_PIPE_BUSY)
        {
            throw(Exception("NamedPipeClient::connect() - failed to connect \
                            to secondary pipe"));
        }
       
		//Wait for server to connect for a maximum of MAX_TIMEOUT period.
        if (::WaitNamedPipe(_OPERATION_PIPE_NAME(_name).getCString(),
                            MAX_TIMEOUT) == FALSE)
        {
            throw(Exception("NamedPipeClient::connect() - timed out waiting\
                            for secondary pipe"));
        }
    }

    DWORD dwMode = PIPE_READMODE_MESSAGE | PIPE_WAIT;

    rc =
        ::SetNamedPipeHandleState(
            pipe2->hpipe,
            &dwMode,
            0,
            0);

    if (rc == FALSE)
    {
        ::CloseHandle(pipe2->hpipe);
        throw(Exception("NamedPipeClient::connect() - failed to set state for primary pipe"));

    }
    //Create an event object to track the I/O
    pipe2->overlap.hEvent = CreateEvent(NULL,    // default security attribute
                                       FALSE,
                                       FALSE,
                                       NULL);   // unnamed event object

    if (pipe2->overlap.hEvent  == NULL)
    {
         throw(Exception("NamedPipeServer::connect failed to create event for secondary pipe."));
    }

    // the caller is responsible for disconnecting the pipe
    // and closing the pipe
    NamedPipeClientEndPiont* nPCEPoint = new NamedPipeClientEndPiont(String("Operationpipe"), *pipe2);
    nPCEPoint->connected();
    return(*nPCEPoint);
}

void NamedPipeClient::disconnect(HANDLE pipe) const
{
    // perform handshake
    string request(DISCONNECT_REQUEST);
    string response(DISCONNECT_RESPONSE);

    DWORD size = 0;
    //Make a request for disconnect
    BOOL rc =
        ::TransactNamedPipe(
            pipe,
            (void *)request.data(),
            request.size(),
            (void *)response.data(),
            response.size(),
            &size,
            0);

    if(rc = FALSE)
    {
        //return(0);
    }


}

NamedPipeServerEndPoint::NamedPipeServerEndPoint(String name, NamedPipeRep pipeStruct)
{
    isConnectionPipe = false;
    connected();
    _name = name;
    _pipe = pipeStruct;

}

NamedPipeClientEndPiont::NamedPipeClientEndPiont(String name, NamedPipeRep pipeStruct)
{
    isConnectionPipe = false;
    _isConnected = false;
    _name = name;
    _pipe = pipeStruct;

}

NamedPipeServerEndPoint::~NamedPipeServerEndPoint()
{

}

NamedPipeClientEndPiont::~NamedPipeClientEndPiont()
{
}

