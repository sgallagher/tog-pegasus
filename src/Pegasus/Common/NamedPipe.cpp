#pragma warning(disable:4530)

#include "NamedPipe.h"

#include <iostream>
#include <Pegasus/Common/String.h>

//using namespace std;

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;


const static DWORD MAX_BUFFER_SIZE = 4096;  // 4 kilobytes
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

bool NamedPipe::read(HANDLE pipe, String & buffer)
{
    // clear buffer
    buffer.clear();


    for( ; ; )
    {
        // read all data in pipe
        string raw(MAX_BUFFER_SIZE, string::value_type(0));
        DWORD size = 0;

        BOOL rc = ::ReadFile(
                pipe,
                (void *)raw.data(),
                raw.size(),
                &size,
                0);

        // only fail is ::ReadFile returns false and some error other than more data
        if((rc == FALSE) && (::GetLastError() != ERROR_MORE_DATA))
        {
            cout << "::ReadFile() failed (RC = " << hex << ::GetLastError() << ")" << endl;

            return(false);                                                       }

        //string temp(const raw);
        buffer.assign(raw.data());

        // check for message complete
        if(rc == TRUE)
        {
            break;
        }
    }

    return(true);
}

// ATTN: need to update function to read data larger than MAX_BUFFER_SIZE
bool NamedPipe::write(HANDLE pipe, String & buffer)
{
    DWORD size = 0;

    BOOL rc =
        ::WriteFile(
            pipe,
            /*(void *)*/buffer.getCString(),
            buffer.size(),
            &size,
            0);

    if((rc == FALSE) || (size != buffer.size()))
    {
        cout << "::WriteFile() failed (RC = " << hex << ::GetLastError() << ")" << endl;

        return(false);
    }

    return(true);
}

//
// NamedPipeServer
//

NamedPipeServer::NamedPipeServer(const String & pipeName) 
{
    cout << "Entering NamedPipeServer::NamedPipeServer()" << endl;

    _name = pipeName;
    Boolean ConnectFailed = false;
    _pipe.hpipe = 0;

    // create a primary named pipe to listen for connection requests
    _pipe.hpipe  =
        ::CreateNamedPipe(
            _PRIMARY_PIPE_NAME(_name).getCString(),
            PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,   // read/write   
            PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
            MAX_PIPE_INSTANCES,                   //  PIPE_UNLIMITED_INSTANCES, // max. instances??
            MAX_BUFFER_SIZE,
            MAX_BUFFER_SIZE,
            MAX_TIMEOUT,
            0);   // NULL ??

    if(_pipe.hpipe == INVALID_HANDLE_VALUE)
    {
        cout << "NamedPipeServer::NamedPipeServer() - failed to create primary pipe" << endl;
        cout << "::CreateNamedPipe() rc = " << ::GetLastError() << endl;

        throw 0;
    }

    cout << "NamedPipeServer::NamedPipeServer() - primary pipe created" << _pipe.hpipe << endl;
    _pipe.overlap.hEvent = CreateEvent(NULL,    // default security attribute
                                       FALSE,
                                       FALSE,
                                       NULL);   // unnamed event object

    if (_pipe.overlap.hEvent == NULL)
    {
         cout << " NamedPipeServer::NamedPipeServer() - Failed to create event." << endl;
         cout << "CreateEvent failed with " << GetLastError() << "."<< endl;
         throw Exception("CreateEvet failed in NamedPipeServer constructor");    
    }

    cout << " NamedPipeServer::NamedPipeServer() - creating Event "<< (int) _pipe.overlap.hEvent << endl;

    Boolean fconnected = false;

    fconnected = _connectToNamedPipe( _pipe.hpipe, &_pipe.overlap);
    cout << "Primary pipe in NamePipeServer::NamePipeServer : " << _pipe.hpipe << endl;

    cout << "Primiary pipe overlap.hEvent in NamePipeServer::NamePipeServer : " << _pipe.overlap.hEvent << endl;
    if (fconnected)
    {
            cout << "NamedPipeServer::NamedPipeServer ConnectNamedPipe failed." << endl;

            //SHOULD THROW AN EXCEPTION HERE           
    }
    cout << "at the end of NamedPipeServer::NamedPipeServer" << endl;

}

NamedPipeServer::~NamedPipeServer(void)
{
    cout << "NamedPipeServer::~NamedPipeServer()" << endl;

    //::CloseHandle(_pipe.hpipe);
}

NamedPipeServerEndPiont NamedPipeServer::accept(void) 
{

    Boolean fconnected = false;
    Boolean ConnectFailed = false;

    cout << "NamedPipeServer::accept()" << endl;

    // perform handshake
    String request(CONNECT_REQUEST);
    String response(CONNECT_RESPONSE);

    // get request
    if(!NamedPipe::read(_pipe.hpipe, request))
    {
        cout << "NamedPipeServer::accept() - read failed" << endl;

        ::DisconnectNamedPipe(_pipe.hpipe);
        fconnected = _connectToNamedPipe( _pipe.hpipe, &_pipe.overlap);
        cout << "In NamePipeServer::accept() - Primary pipe reconnecting after failed read: " << _pipe.hpipe << endl;
        if (fconnected)
        {
             cout << "NamedPipeServer::accept Primary - Pipe Failed to reconnect." << endl;
            //should throw an exception here
             throw(Exception("NamedPipeServer::accept Primary - Pipe Failed to reconnect."));
            //return NULL;
        }
       // return NULL;
        throw(Exception("NamedPipeServer::accept Primary - Pipe Failed to reconnect."));
    }

    cout << "NamedPipeServer::accept() - recv " << request << endl;

    if(request != CONNECT_REQUEST)
    {
        cout << "NamedPipeServer::accept() - incorrect request type" << endl;

        ::DisconnectNamedPipe(_pipe.hpipe);
        fconnected = _connectToNamedPipe( _pipe.hpipe, &_pipe.overlap);
        cout << "In NamePipeServer::accept() - Primary Pipe reconnecting after incorrect request: " << _pipe.hpipe << endl;
        if (fconnected)
        {
            cout << "NamedPipeServer::accept() - Primary Pipe Failed to reconnect." << endl;
           //should throw an exception here
           // return NULL;
            throw(Exception("NamedPipeServer::accept() - Primary Pipe Failed to reconnect."));
        }
        //return NULL;
        throw(Exception("NamedPipeServer::accept() - Primary Pipe Failed to reconnect."));
    }

    cout << "NamedPipeServer::accept() - Creating Secondary Pipe " << request << endl;

    PEGASUS_NAMEDPIPE* pipe2 = new PEGASUS_NAMEDPIPE;

    cout << "NamedPipeServer::accept() - Creating Secondary Pipe " << request << endl;

    // create a secondary named pipe for processing requests
    pipe2->hpipe =
    ::CreateNamedPipe(
            _SECONDARY_PIPE_NAME(_name).getCString(),
            PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,   // PIPE_ACCESS_DUPLEX |  FILE_FLAG_OVERLAPPED, // read/write   ??
            PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
            PIPE_UNLIMITED_INSTANCES,            
            MAX_BUFFER_SIZE,
            MAX_BUFFER_SIZE,
            MAX_TIMEOUT,
            0);   // NULL ??

    cout << "NamedPipeServer::accept() - After creating secondary pipe" << endl;

    if(pipe2->hpipe == INVALID_HANDLE_VALUE)
    {
        cout << "NamedPipeServer::accept() - failed to create secondary pipe" << endl;
        cout << "::CreateNamedPipe() rc = " << ::GetLastError() << endl;

        ::DisconnectNamedPipe(_pipe.hpipe);
        fconnected = _connectToNamedPipe( _pipe.hpipe, &_pipe.overlap);
        cout << "In NamePipeServer::accept() - Primary Pipe reconnecting after failing to create secondary pipe. " << _pipe.hpipe << endl;
        if (fconnected)
        {
            cout << "NamedPipeServer::accept() - Primary Pipe Failed to reconnect." << endl;
           //should throw an exception here
           // return NULL;
           throw(Exception("NamedPipeServer::accept() - Primary Pipe Failed to reconnect."));
        }
        // temp debug code to detect pipe creating failures
        throw 0;

        //return NULL;
    }

    cout << "NamedPipeServer::accept() - secondary pipe successfully created" << endl;

       // send response
    cout << "NamedPipeServer::accept() - send "  << response << endl;

    if(!NamedPipe::write(_pipe.hpipe, response))
    {
        cout << "NamedPipeServer::accept() - write failed" << endl;

        ::DisconnectNamedPipe(_pipe.hpipe);
        ::CloseHandle(pipe2->hpipe);
        fconnected = _connectToNamedPipe( _pipe.hpipe, &_pipe.overlap);
        cout << "In NamePipeServer::accept() - Primary Pipe reconnecting after write failure." << _pipe.hpipe << endl;
        if (fconnected)
        {
            cout << "NamedPipeServer::accept() - Primary Pipe Failed to reconnect." << endl;
           //should throw an exception here
           // return NULL;
           throw(Exception("NamedPipeServer::accept() - Primary Pipe Failed to reconnect."));
        }
        //return NULL;
        throw(Exception("NamedPipeServer::accept() - Primary Pipe Failed to reconnect."));
    }


    cout << "NamedPipeServer::accept() - waiting for connection " << _SECONDARY_PIPE_NAME(_name) << endl;

    pipe2->overlap.hEvent = CreateEvent(NULL,    // default security attribute
                                       FALSE,
                                       FALSE,
                                       NULL);   // unnamed event object

    if (pipe2->overlap.hEvent == NULL)
    {
         cout << " NamedPipeServer::accept failed to create event for secondary pipe." << endl;
         cout << "CreateEvent failed with " << GetLastError() << "."<< endl;
         throw 0;    
    }

    cout << " NamedPipeServer::accept - Creating Event "<< pipe2->overlap.hEvent << endl;

    fconnected = _connectToNamedPipe(pipe2->hpipe, &pipe2->overlap);
    cout << "In NamePipeServer::accept() - Secondary Pipe connecting.... " << _pipe.hpipe << endl;
    if (fconnected)
    {
        cout << "NamedPipeServer::accept() - Secondary Pipe Failed to reconnect." << endl;
       //should throw an exception here
        ::DisconnectNamedPipe(_pipe.hpipe);
        ::CloseHandle(pipe2->hpipe);
        fconnected = _connectToNamedPipe( _pipe.hpipe, &_pipe.overlap);
        cout << "In NamePipeServer::accept() - Primary Pipe reconnecting after Secondary Pipe connection failure. " << _pipe.hpipe << endl;
        if (fconnected)
        {
            cout << "NamedPipeServer::accept() - Primary Pipe Failed to reconnect." << endl;
           //should throw an exception here
            //return NULL;
            throw(Exception("NamedPipeServer::accept() - Primary Pipe Failed to reconnect."));
        }

        //return NULL;
        throw(Exception("NamedPipeServer::accept() - Primary Pipe Failed to reconnect."));
    }

    // disconnect primary pipe so it can respond to other requests
    ::DisconnectNamedPipe(_pipe.hpipe);

    fconnected = _connectToNamedPipe( _pipe.hpipe, &_pipe.overlap);
    cout << "In NamePipeServer::accept() - Primary Pipe reconnecting after disconnecting from prev. client. " << _pipe.hpipe << endl;
    if (fconnected)
    {
        cout << "NamedPipeServer::accept() - Primary Pipe Failed to reconnect." << endl;
       //should throw an exception here
        //return NULL;
        throw(Exception("NamedPipeServer::accept() - Primary Pipe Failed to reconnect."));
    }

    // the caller is responsible for disconnecting the pipe
    // and closing the pipe

    //NOTE:: I am not sure how to give each new Pipe a new name
    return(NamedPipeServerEndPiont(String("Operationpipe"), *pipe2));
}




Boolean NamedPipeServer::_connectToNamedPipe(HANDLE pipe, LPOVERLAPPED overlap)
{

    Boolean fconnected = false;
    Boolean ConnectFailed = false;
    
    fconnected = ConnectNamedPipe(pipe, overlap);
    if (fconnected)
    {
          cout << "Main:: ConnectNamedPipe failed." << endl;
          ::CloseHandle(pipe);
          //::DisconnectNamedPipe(namedPipes[0].hpipe);
          return fconnected;         
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
                 break;

        }
     // JA  listening
    case ERROR_PIPE_LISTENING:
        {
            cout << "in NamedPipe::_connectToNamedPipe server in connected and listening" << endl;
            break;
        }

            //ABB: If an error occurs during the connect operation...

       default:
       {
          cout << "ConnectNamedPipe failed with " << GetLastError() << "." << endl;
          //return -1;
          ConnectFailed = true;
          break;
       }
    }          

    if (ConnectFailed)
    {
        ::CloseHandle(pipe);
        //::DisconnectNamedPipe(namedPipes[0].hpipe);
        return ConnectFailed;
    }
    return fconnected;

}








//
// NamedPipeClient
//

NamedPipeClient::NamedPipeClient(const String & name) 
{
    _name = (name);
    cout << "NamedPipeClient::NamedPipeClient()" << endl;

}

NamedPipeClient::~NamedPipeClient(void)
{
    cout << "NamedPipeClient::~NamedPipeClient()" << endl;
}

HANDLE NamedPipeClient::connect(void) const
{
    cout << "NamedPipeClient::connect() - " << _PRIMARY_PIPE_NAME(_name) << endl;

    // perform handshake
    string request(CONNECT_REQUEST);
    string response(CONNECT_RESPONSE);

    DWORD size = 0;

    cout << "NamedPipeClient::connect() - send " << request.data() << endl;

    BOOL rc =
        ::CallNamedPipe(
            _PRIMARY_PIPE_NAME(_name).getCString(),
            (void *)request.data(),
            request.size(),
            (void *)response.data(),
            response.size(),
            &size,
            MAX_TIMEOUT);

    if(rc == 0)
    {
        cout << "NamedPipeClient::connect() - failed to call primary pipe" << endl;
        cout << "::CallNamedPipe() rc = " << ::GetLastError() << endl;

        return(0);
    }

    cout << "NamedPipeClient::connect() - recv " << response.data() << endl;
    if(strcmp(response.data(), CONNECT_RESPONSE))
   // if(response. != CONNECT_RESPONSE)
    {
        cout << "NamedPipeClient::connect() - incorrect response" << endl;

        return(0);
    }

    cout << "NamedPipeClient::connect() - connecting to " << _SECONDARY_PIPE_NAME(_name) << endl;

    HANDLE pipe2 = INVALID_HANDLE_VALUE;

    for( ; ; )
    {
        pipe2 =
            ::CreateFile(
                _SECONDARY_PIPE_NAME(_name).getCString(),
                GENERIC_READ | GENERIC_WRITE,
                0,
                0,
                OPEN_EXISTING,
                0,
                0);

        if(pipe2 != INVALID_HANDLE_VALUE)
        {
            break;
        }

        if(::GetLastError() != ERROR_PIPE_BUSY)
        {
            cout << "NamedPipeClient::connect() - failed to connect to secondary pipe" << endl;
            cout << "::CreateFile() rc = " << ::GetLastError() << endl;

            return(0);
        }

        cout << "NamedPipeClient::connect() - pipe is busy. waiting..." << endl;

        if(::WaitNamedPipe(_SECONDARY_PIPE_NAME(_name).getCString(), MAX_TIMEOUT) == FALSE)
        {
            cout << "NamedPipeClient::connect() - timed out waiting for secondary pipe" << endl;
            cout << "::WaitNamedPipe() rc = " << ::GetLastError() << endl;

            return(0);
        }
    }

    DWORD dwMode = PIPE_READMODE_MESSAGE | PIPE_WAIT;

    rc =
        ::SetNamedPipeHandleState(
            pipe2,
            &dwMode,
            0,
            0);

    if(rc == FALSE)
    {
        cout << "NamedPipeClient::connect() - failed to set state for primary pipe" << endl;
        cout << "::SetNamedPipeState() rc = " << ::GetLastError() << endl;

        ::CloseHandle(pipe2);

        return(0);
    }

    // the caller is responsible for disconnecting the pipe
    // and closing the pipe
    return(pipe2);
}

void NamedPipeClient::disconnect(HANDLE pipe) const
{
    cout << "NamedPipeClient::disconnect() - " << _SECONDARY_PIPE_NAME(_name) << endl;

    // perform handshake
    string request(DISCONNECT_REQUEST);
    string response(DISCONNECT_RESPONSE);

    DWORD size = 0;

    cout << "NamedPipeClient::disconnect() - send " << request.data() << endl;

    BOOL rc =
        ::TransactNamedPipe(
            pipe,
            (void *)request.data(),
            request.size(),
            (void *)response.data(),
            response.size(),
            &size,
            0);

    cout << "NamedPipeClient::disconnect() - recv " << response.data() << endl;

    if(rc = FALSE)
    {
        cout << "NamedPipeClient::disconnect() - failed to call primary pipe" << endl;
        cout << "::TransactNamedPipe() rc = " << ::GetLastError() << endl;

        //return(0);
    }

    ::CloseHandle(pipe);
}


NamedPipeServerEndPiont::NamedPipeServerEndPiont(String name, PEGASUS_NAMEDPIPE pipeStruct)
{
    cout << "in NamedPipeServerEndPiont constructor " << endl;
    _name = name;
    _pipe = pipeStruct;

}

NamedPipeServerEndPiont::~NamedPipeServerEndPiont()
{
}
