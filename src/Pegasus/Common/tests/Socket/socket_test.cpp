//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2003 BMC Software, Hewlett-Packard Company, IBM,
// The Open Group, Tivoli Systems
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
// Author: Mike Day (mdday@us.ibm.com)
//
// Modified By: 
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/IPC.h>
#include <Pegasus/Common/DQueue.h>
#include <Pegasus/Common/Thread.h>
#include <Pegasus/Common/Socket.h>
#include <sys/types.h>
#if defined(PEGASUS_PLATFORM_WIN32_IX86_MSVC)
#else
#include <unistd.h>
#include <signal.h>
#endif 
#include <cassert>
#include <iostream>
#include <stdio.h>
#include <string.h>



PEGASUS_USING_STD;
PEGASUS_USING_PEGASUS;

const ushort PORTNO = 6988;

const char* OK    = "ACK\n\0";
const char* CMD   = "COMMAND\n\0";
const char* QUIT  = "QUIT\n\0";

AtomicInt cmd_tx, cmd_rx;

void pipe_handler(int signum)
{
   return;
}


PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL remote_socket(void *parm)
{
   Thread * my_handle = reinterpret_cast<Thread *>(parm);
   
#ifdef PEGASUS_OS_TYPE_WINDOWS
#else
   signal(SIGPIPE, SIG_IGN);
#endif
   static bsd_socket_factory sf;
   pegasus_socket listener(&sf);
   
   // create the underlying socket
   listener.socket(PF_INET, SOCK_STREAM, 0);

   
   // initialize the address
   struct sockaddr_in addr;
   memset(&addr, 0, sizeof(addr));
   addr.sin_addr.s_addr = INADDR_ANY;
   addr.sin_family = AF_INET;
   addr.sin_port = htons(PORTNO);
   
   listener.bind((struct sockaddr *)&addr, sizeof(addr));
   listener.listen(15);
   
   // initialize select loop

   fd_set fd_listen;
   FD_ZERO(&fd_listen);
   FD_SET( (Sint32)listener, &fd_listen );
   
   int events = select(FD_SETSIZE, &fd_listen, NULL, NULL, NULL);
   
   struct sockaddr peer;
   size_t peer_size = sizeof(peer);
   
   pegasus_socket connected = listener.accept(&peer, &peer_size);
   
   while(1)
   {
      FD_ZERO(&fd_listen);
      FD_SET((Sint32)connected, &fd_listen);
      
      events = select(FD_SETSIZE, &fd_listen, NULL, NULL, NULL);

      unsigned char buf[256];
      memset(&buf, 0, 256);
      Sint32 bytes = connected.read((void *)&buf, 255);
      bytes = connected.write(&OK, 4);
      cmd_rx++;
      
      if(! strncmp(QUIT, (const char *)buf, 5))
	 break;
   }
   
   connected.shutdown(2);
   connected.close();
//   my_handle->exit_self( (PEGASUS_THREAD_RETURN) 1 );
   return 0;
}



int main(int argc, char** argv)
{

#ifdef PEGASUS_OS_TYPE_WINDOWS
#else
   signal(SIGPIPE, SIG_IGN);
#endif
   Thread th_listener(remote_socket, NULL, false);
   th_listener.run();
   

   static bsd_socket_factory sf;
   pegasus_socket connector(&sf);
   
   // create the underlying socket
   connector.socket(PF_INET, SOCK_STREAM, 0);

   
   // initialize the address
   struct sockaddr_in addr;
   memset(&addr, 0, sizeof(addr));
   addr.sin_addr.s_addr = INADDR_ANY;
   addr.sin_family = AF_INET;

   connector.bind((struct sockaddr *)&addr, sizeof(addr));

   struct sockaddr_in peer;
   size_t peer_size = sizeof(peer);
   memset(&peer, 0, peer_size);

   peer.sin_addr.s_addr = inet_addr("127.0.0.1");
   peer.sin_family= AF_INET;
   peer.sin_port = htons(PORTNO);

   connector.connect((struct sockaddr *)&peer, peer_size);
   cmd_tx = 0;
   
   while(cmd_tx.value() < 10 )
   {
      unsigned char buf[256];
      connector.write(CMD, 8);
      connector.read(buf, 255);
      cmd_tx++;
   }
   connector.write(QUIT, 5);
   cmd_tx++;

   while( cmd_rx.value() < cmd_tx.value() )
      pegasus_sleep(1);

   th_listener.cancel();
   th_listener.join();
   
   cout << argv[0] << " +++++ passed all tests" << endl;
   return 0;
}
