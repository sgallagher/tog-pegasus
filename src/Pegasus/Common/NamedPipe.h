#ifndef Pegasus_NamedPipe_h
#define Pegasus_NamedPipe_h


#include <windows.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Exception.h>
#include "Linkage.h"
PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

#define MAX_PIPE_INSTANCES  1
const DWORD NAMEDPIPE_MAX_BUFFER_SIZE = 4096; //8192; 

typedef struct
{
	HANDLE hpipe;
	OVERLAPPED overlap;
} PEGASUS_NAMEDPIPE;


//class PEGASUS_COMMON_LINKAGE NamedPipe
class PEGASUS_COMMON_LINKAGE NamedPipe
{
public:
    //void disconnect(HANDLE pipe) const;
    static bool read(HANDLE pipe, String & buffer);
    static bool write(HANDLE pipe, String & buffer, LPOVERLAPPED overlap = NULL);
    String getName(void) {return _name;}

    HANDLE getPipe(void){ return _pipe.hpipe;}
    LPOVERLAPPED getOverlap(void){ return &_pipe.overlap;}

    Boolean isConnectionPipe;
    char raw[NAMEDPIPE_MAX_BUFFER_SIZE];
    //string raw;
    DWORD bytesRead;
  
protected:
   String _name;
   PEGASUS_NAMEDPIPE _pipe;

};



//Need a better name for this class
class PEGASUS_COMMON_LINKAGE NamedPipeServerEndPiont : public NamedPipe
{
public:
    NamedPipeServerEndPiont(String name, PEGASUS_NAMEDPIPE pipeStruct);
    ~NamedPipeServerEndPiont(void);

};

class PEGASUS_COMMON_LINKAGE NamedPipeClientEndPiont : public NamedPipe
{
public:
    NamedPipeClientEndPiont(String name, PEGASUS_NAMEDPIPE pipeStruct);
    ~NamedPipeClientEndPiont(void);

};



//class PEGASUS_COMMON_LINKAGE NamedPipeServer : public NamedPipe 

class PEGASUS_COMMON_LINKAGE NamedPipeServer : public NamedPipe
{
public:
    NamedPipeServer(const String & name); 
    ~NamedPipeServer(void);

 //   HANDLE accept(void) const;
    NamedPipeServerEndPiont accept(void);
    
    

private:

   Boolean _connectToNamedPipe(HANDLE pipe, LPOVERLAPPED overlap);
   //String _name;
  // PEGASUS_NAMEDPIPE _pipe;

};

//class PEGASUS_COMMON_LINKAGE NamedPipeClient : public NamedPipe


class PEGASUS_COMMON_LINKAGE NamedPipeClient : public NamedPipe
{
public:
    NamedPipeClient(const String & name);
    ~NamedPipeClient(void);

    NamedPipeClientEndPiont connect(void);
    void disconnect(HANDLE pipe) const;
    //OVERLAPPED getOverlap(void){ return _pipe.overlap;}

private:

    //String _name;
    ///String _name;
    //PEGASUS_NAMEDPIPE _pipe;


};


#endif /* Pegasus_NamedPipe_h */