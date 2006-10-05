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

const static DWORD MAX_TIMEOUT = 30000;     // 30 seconds

const static char* CONNECT_REQUEST = "<connect-request>";
const static char* CONNECT_RESPONSE = "<connect-response>";
const static char* DISCONNECT_REQUEST = "<disconnect-request>";
const static char* DISCONNECT_RESPONSE = "<disconnect-response>";

static inline String _PRIMARY_PIPE_NAME(const String & name)
{
    return(name + "0");
}

static inline String _SECONDARY_PIPE_NAME(const String & name)
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

bool NamedPipe::read(HANDLE pipe, String & buffer)
{
    // clear buffer
    buffer.clear();

    for ( ; ; )
    {
        // read all data in pipe
        string raw(NAMEDPIPE_MAX_BUFFER_SIZE, string::value_type(0));
        DWORD size = 0;

        BOOL rc = ::ReadFile(
                pipe,
                (void *)raw.data(),
                raw.size(),
                &size,
                0);

        // only fail is ::ReadFile returns false and some error other than more data
        if ((rc == FALSE) && (::GetLastError() != ERROR_MORE_DATA))
        {
            return false;
        }


        buffer.assign(raw.data());

        // check for message complete
        if (rc == TRUE)
        {
            break;
        }
    }

    return true;
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
// data is read by the peer. If status of write is returned as the return value
///////////////////////////////////////////////////////////////////////////////

// ATTN: need to update function to read data larger than MAX_BUFFER_SIZE
//bool NamedPipe::write(HANDLE pipe, String & buffer)
bool NamedPipe::write(HANDLE pipe, String & buffer, LPOVERLAPPED overlap)
{
    DWORD size = 0;

    BOOL rc =
        ::WriteFile(
            pipe,
            /*(void *)*/buffer.getCString(),
            buffer.size(),
            &size,
            overlap);     //this should be the overlap

    if (!rc)
    {
        if (GetLastError() != 232)
        {
            const char* lpMsgBuf;
            LPVOID lpDisplayBuf;
            DWORD dw = GetLastError();

            FormatMessage(
               FORMAT_MESSAGE_ALLOCATE_BUFFER |
               FORMAT_MESSAGE_FROM_SYSTEM,
               NULL,
               dw,
               MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
               (LPTSTR) &lpMsgBuf,
               0, NULL );

            lpDisplayBuf = LocalAlloc(LMEM_ZEROINIT, 
				           (strlen(lpMsgBuf)+90)*sizeof(TCHAR));
 #ifdef PEGASUS_LOCALDOMAINSOCKET_DEBUG
            printf("WriteFile in NamedPipe::write failed with error \
				   %d: %s", dw, lpMsgBuf);
 #endif
            //LocalFree(lpMsgBuf);
            LocalFree(lpDisplayBuf);
            //ExitProcess(dw);
            return false;
        }
    }
	// Wait for the client peer to read the data. This must be enabled
	// for Chunking support
    ::FlushFileBuffers (pipe);
    return(true);
}

//
// NamedPipeServer
//

NamedPipeServer::NamedPipeServer(const String & pipeName)
{
    _name = pipeName;
    Boolean ConnectFailed = false;
    isConnectionPipe = true;
    _pipe.hpipe = 0;

    // create a primary named pipe to listen for connection requests
    _pipe.hpipe  =
        ::CreateNamedPipe(
            _PRIMARY_PIPE_NAME(_name).getCString(),
            PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,   // read/write
            PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
            MAX_PIPE_INSTANCES,                   //  PIPE_UNLIMITED_INSTANCES, // max. instances??
            NAMEDPIPE_MAX_BUFFER_SIZE,
            NAMEDPIPE_MAX_BUFFER_SIZE,
            MAX_TIMEOUT,
            0);   // NULL ??
        
	if (_pipe.hpipe == INVALID_HANDLE_VALUE)
    {
        throw 0;
    }
 
	// Set the isConnected flag which shall be used in Monitor::run 
	connected();

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
    if (bIsconnected)
    {
            //SHOULD THROW AN EXCEPTION HERE
    }

}

NamedPipeServer::~NamedPipeServer(void)
{
}

NamedPipeServerEndPiont NamedPipeServer::accept(void)
{

    Boolean bIsconnected = false;
    Boolean ConnectFailed = false;

    // perform handshake
    String request(CONNECT_REQUEST);
    String response(CONNECT_RESPONSE);

    // get request
    if (!NamedPipe::read(_pipe.hpipe, request))
    {
        ::DisconnectNamedPipe(_pipe.hpipe);
        bIsconnected = _connectToNamedPipe( _pipe.hpipe, &_pipe.overlap);
        if (bIsconnected)
        {
            //should throw an exception here
             throw(Exception("NamedPipeServer::accept Primary - Pipe Failed to \
							 reconnect."));
        }
       
        throw(Exception("NamedPipeServer::accept Primary - Pipe Failed to \
						reconnect."));
    }


    if (request != CONNECT_REQUEST)
    {
        ::DisconnectNamedPipe(_pipe.hpipe);
        bIsconnected = _connectToNamedPipe( _pipe.hpipe, &_pipe.overlap);
       
        if (bIsconnected)
        {
           //should throw an exception here
            throw(Exception("NamedPipeServer::accept() - Primary Pipe Failed\
							 to reconnect."));
        }

        throw(Exception("NamedPipeServer::accept() - Primary Pipe Failed \
						to reconnect."));
    }


    PEGASUS_NAMEDPIPE* pipe2 = new PEGASUS_NAMEDPIPE;

    pipe2->overlap.Offset = 0;
    pipe2->overlap.OffsetHigh = 0;

    // create a secondary named pipe for processing requests
    pipe2->hpipe =
    ::CreateNamedPipe(
            _SECONDARY_PIPE_NAME(_name).getCString(),
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
        if (bIsconnected)
        {
           //should throw an exception here
           throw(Exception("NamedPipeServer::accept() - Primary Pipe Failed\
						    to reconnect."));
        }
        // temp debug code to detect pipe creating failures
        throw 0;

    }

    if(!NamedPipe::write(_pipe.hpipe, response))
    {
        ::DisconnectNamedPipe(_pipe.hpipe);
        ::CloseHandle(pipe2->hpipe);
        bIsconnected = _connectToNamedPipe( _pipe.hpipe, &_pipe.overlap);
        if (bIsconnected)
        {
            //should throw an exception here
            throw(Exception("NamedPipeServer::accept() - Primary Pipe Failed\
							to reconnect."));
        }
        throw(Exception("NamedPipeServer::accept() - Primary Pipe Failed to\
						reconnect."));
    }

    pipe2->overlap.hEvent = CreateEvent(NULL,    // default security attribute
                                       FALSE,
                                       FALSE,
                                       NULL);   // unnamed event object

    if (pipe2->overlap.hEvent == NULL)
    {
        throw 0;
    }


    bIsconnected = _connectToNamedPipe(pipe2->hpipe, &pipe2->overlap);
    if (bIsconnected)
    {
        ::DisconnectNamedPipe(_pipe.hpipe);
        ::CloseHandle(pipe2->hpipe);
        bIsconnected = _connectToNamedPipe( _pipe.hpipe, &_pipe.overlap);
        if (bIsconnected)
        {
            //should throw an exception here
            throw(Exception("NamedPipeServer::accept() - Primary Pipe Failed\
							to reconnect."));
        }

        throw(Exception("NamedPipeServer::accept() - Primary Pipe Failed to \
						reconnect."));
    }

    // disconnect primary pipe so it can respond to other requests
    ::DisconnectNamedPipe(_pipe.hpipe);

    bIsconnected = _connectToNamedPipe( _pipe.hpipe, &_pipe.overlap);
    if (bIsconnected)
    {
        //should throw an exception here
        throw(Exception("NamedPipeServer::accept() - Primary Pipe Failed to reconnect."));
    }

    // the caller is responsible for disconnecting the pipe
    // and closing the pipe

    //NOTE:: I am not sure how to give each new Pipe a new name
	connected();
    return(NamedPipeServerEndPiont(String("Operationpipe"), *pipe2));
}

Boolean NamedPipeServer::_connectToNamedPipe(HANDLE pipe, LPOVERLAPPED overlap)
{

    Boolean bIsconnected = false;
    Boolean ConnectFailed = false;

    bIsconnected = ConnectNamedPipe(pipe, overlap);
    if (bIsconnected)
    {
          ::CloseHandle(pipe);
          return bIsconnected;
     }
    switch (GetLastError())
    {
        //ABB: If the overlapped connection in progress.
        //ABB: Something meaningful needs to be added here
        case ERROR_IO_PENDING:
             break;

        //If the Client is already connected, so signal an event.
        case ERROR_PIPE_CONNECTED:
        {
            if (SetEvent(overlap->hEvent))
			{
                break;
			}
        }

        case ERROR_PIPE_LISTENING:
        {
            break;
        }

        //ABB: If an error occurs during the connect operation...
        default:
        {
            //return -1;
            ConnectFailed = true;
            break;
        }
    }

    if (ConnectFailed)
    {
        ::CloseHandle(pipe);
        return ConnectFailed;
    }
	connected();
    return bIsconnected;

}

//
// NamedPipeClient
//

NamedPipeClient::NamedPipeClient(const String & name)
{
    isConnectionPipe = false;
    _name = (name);
}

NamedPipeClient::~NamedPipeClient(void)
{
}

NamedPipeClientEndPiont NamedPipeClient::connect(void)
{
    // perform handshake
    string request(CONNECT_REQUEST);
    string response(CONNECT_RESPONSE);

    DWORD size = 0;

    BOOL rc =
        ::CallNamedPipe(
            _PRIMARY_PIPE_NAME(_name).getCString(),
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

    if (strcmp (response.data(), CONNECT_RESPONSE))
    {
        throw(Exception("NamedPipeClient::connect() - Incorrect response"));
    }

    PEGASUS_NAMEDPIPE* pipe2 = new PEGASUS_NAMEDPIPE;
    pipe2->overlap.Offset = 0;
    pipe2->overlap.OffsetHigh = 0;
    for ( ; ; )
    {
        pipe2->hpipe =
            ::CreateFile(
                _SECONDARY_PIPE_NAME(_name).getCString(),
                GENERIC_READ | GENERIC_WRITE,
                0,
                0,
                OPEN_EXISTING,
                FILE_FLAG_OVERLAPPED,
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

        if (::WaitNamedPipe(_SECONDARY_PIPE_NAME(_name).getCString(),
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

NamedPipeServerEndPiont::NamedPipeServerEndPiont(String name, PEGASUS_NAMEDPIPE pipeStruct)
{
    isConnectionPipe = false;
	connected();
    _name = name;
    _pipe = pipeStruct;

}

NamedPipeClientEndPiont::NamedPipeClientEndPiont(String name, PEGASUS_NAMEDPIPE pipeStruct)
{
    isConnectionPipe = false;
    _isConnected = false;
    _name = name;
    _pipe = pipeStruct;

}

NamedPipeServerEndPiont::~NamedPipeServerEndPiont()
{
	
}

NamedPipeClientEndPiont::~NamedPipeClientEndPiont()
{
}

