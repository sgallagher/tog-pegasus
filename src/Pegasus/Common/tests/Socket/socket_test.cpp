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
#include <Pegasus/Common/pegasus_socket.h>
#include <Pegasus/Common/Monitor.h>
#include <Pegasus/Common/HTTPAcceptor.h>
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
# ifdef PEGASUS_LOCAL_DOMAIN_SOCKET
#  include <unistd.h>
#  include <sys/un.h>
#  include <Pegasus/Common/Constants.h>
# endif


PEGASUS_USING_STD;
PEGASUS_USING_PEGASUS;

const unsigned short PORTNO = 6988;

const char* OK    = "ACK\n\0";
const char* CMD   = "COMMAND\n\0";
const char* QUIT  = "QUIT\n\0";

AtomicInt cmd_tx, cmd_rx, ready, domain_ready, accept_error;

monitor_2 mon;
void pipe_handler(int signum)
{
   return;
}



void test_dispatch(monitor_2_entry* entry)
{
  unsigned char buf[256];
  memset(&buf, 0, 256);
  Sint32 bytes = entry->get_sock().read((void *)&buf, 255);
  bytes = entry->get_sock().write(&OK, 4);
  cmd_rx++;
  entry->set_accept((void*)cmd_rx.value());
  entry->set_dispatch((void*)cmd_rx.value());

  if(! strncmp(QUIT, (const char *)buf, 5)) {
    // close the socket
  }
}


PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL remote_socket(void *parm)
{
   Thread * my_handle = reinterpret_cast<Thread *>(parm);
   ready = 1;
   mon.run();
   return 0;
}

# ifdef PEGASUS_LOCAL_DOMAIN_SOCKET
// << Thu Aug 14 15:01:10 2003 mdd >> domain sockets work
PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL domain_socket(void *parm)
{
   Thread * my_handle = reinterpret_cast<Thread *>(parm);

#ifdef PEGASUS_OS_TYPE_WINDOWS
#else
   signal(SIGPIPE, SIG_IGN);
#endif
   static unix_socket_factory sf;
   pegasus_socket listener(&sf);

   // create the underlying socket
   listener.socket(AF_UNIX, SOCK_STREAM, 0);


   // initialize the address
   struct sockaddr_un addr;
   memset(&addr, 0, sizeof(addr));
   addr.sun_family = AF_UNIX;
   strcpy(addr.sun_path, PEGASUS_LOCAL_DOMAIN_SOCKET_PATH);

   listener.bind((struct sockaddr *)&addr, sizeof(addr));
   listener.listen(15);

   

   // initialize select loop
   
   fd_set fd_listen;
   FD_ZERO(&fd_listen);
   if((Sint32)listener >= 0 )
   {
      FD_SET( (Sint32)listener, &fd_listen );
   }
   
   domain_ready++;
   int events = select(FD_SETSIZE, &fd_listen, NULL, NULL, NULL);

   struct sockaddr peer;
   PEGASUS_SOCKLEN_SIZE peer_size = sizeof(peer);

   pegasus_socket connected = listener.accept(&peer, &peer_size);
   if((Sint32)connected == -1) 
   {
      accept_error = 1;
   }
   
   while((Sint32)connected >= 0)
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
   return 0;
}

#endif // domain socket

int main(int argc, char** argv)
{

#ifdef PEGASUS_OS_TYPE_WINDOWS
#else
   signal(SIGPIPE, SIG_IGN);
#endif


   // set up the monitor and acceptor, start the service thread
   #ifdef PEGASUS_OS_TYPE_WINDOWS
#else
   signal(SIGPIPE, SIG_IGN);
#endif
   mon.set_session_dispatch(&test_dispatch);
   MessageQueue* output_queue = 0;
   pegasus_acceptor accept(&mon, output_queue, false, PORTNO, 0);
   accept.bind();

   pegasus_acceptor* found = pegasus_acceptor::find_acceptor(false, PORTNO);

   Thread th_listener(remote_socket, NULL, false);
   th_listener.run();


   // set up my connecting socket
   bsd_socket_factory sf;
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
   PEGASUS_SOCKLEN_SIZE peer_size = sizeof(peer);
   memset(&peer, 0, peer_size);
#ifdef PEGASUS_OS_ZOS
   peer.sin_addr.s_addr = inet_addr_ebcdic("127.0.0.1");
#else
   peer.sin_addr.s_addr = inet_addr("127.0.0.1");
#endif
   peer.sin_family= AF_INET;
   peer.sin_port = htons(PORTNO);


   //    wait for the monitor to start running
   while(ready.value() == 0){
     pegasus_sleep(10);
   }
   
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

   mon.stop();

   th_listener.cancel();
   th_listener.join();

# ifdef PEGASUS_LOCAL_DOMAIN_SOCKET
   
   Thread th_domain(domain_socket, NULL, false);
   th_domain.run();

   static unix_socket_factory uf;
   pegasus_socket domain_connector(&uf);
   domain_connector.socket(AF_UNIX, SOCK_STREAM, 0);

   // initialize the address
   struct sockaddr_un un_addr;
   memset(&un_addr, 0, sizeof(un_addr));
   un_addr.sun_family = AF_UNIX;
   strcpy(un_addr.sun_path, PEGASUS_LOCAL_DOMAIN_SOCKET_PATH);

//   domain_connector.bind((struct sockaddr *)&un_addr, sizeof(un_addr));

   struct sockaddr_un un_peer;
   peer_size = sizeof(un_peer);

   strcpy(un_peer.sun_path, PEGASUS_LOCAL_DOMAIN_SOCKET_PATH);
   un_peer.sun_family = AF_UNIX;

   while(domain_ready.value() == 0 )
   {
      pegasus_sleep(10);
      
   }

   
   domain_connector.connect((struct sockaddr *)&un_peer, peer_size);


   if(accept_error.value() )
   {
      th_domain.cancel();
      th_domain.join();
			
      cout << argv[0] << " failed" << endl;
      return 1;
   }
   cmd_tx = 0;
   while(cmd_tx.value() < 10 )
   {
      unsigned char buf[256];
      domain_connector.write(CMD, 8);
      domain_connector.read(buf, 255);
      cmd_tx++;
   }
   domain_connector.write(QUIT, 5);
   cmd_tx++;

   while( cmd_rx.value() < cmd_tx.value() )
      pegasus_sleep(1);

   th_domain.cancel();
   th_domain.join();
			
#endif // domain socket

   cout << argv[0] << " +++++ passed all tests" << endl;
   return 0;
}
