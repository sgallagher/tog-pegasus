//%/////-*-c++-*-////   /////////////////////////////////////////////////////
// Copyright (c) 2000, 2001 The Open group, BMC Software, Tivoli Systems, IBM
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



#include <Pegasus/Common/DQueue.h>
#include <Pegasus/Common/Thread.h>
#include <sys/types.h>
#if defined(PEGASUS_PLATFORM_WIN32_IX86_MSVC)
#else
#include <unistd.h>
#endif 
#include <cassert>
#include <iostream>
#include <stdio.h>
#include <string.h>

PEGASUS_USING_STD;
PEGASUS_USING_PEGASUS;

class FAKE_MESSAGE  
{
   public:
      FAKE_MESSAGE(void *key, int type) : _type(type)
      {
	 _key = (Thread *)key;
      }

      char buffer[128];

      Boolean operator==(const void *key) const
      {
	 if(_key == (const Thread *)key)
	    return true;
	 return false;
      }

      Boolean operator==(const FAKE_MESSAGE & msg) const
      {
	 return ( this->operator==((const void *)msg._key));
      }

   private:
      FAKE_MESSAGE(void);
      Thread * _key;
      int _type;
};


PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL client_sending_thread(void *parm);
PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL client_receiving_thread(void *parm);
PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL server_thread(void *parm);

typedef struct 
{
      AsyncDQueue<FAKE_MESSAGE> *incoming; // to server
      AsyncDQueue<FAKE_MESSAGE> *outgoing; // from server
}read_write;


AtomicInt replies;
AtomicInt requests;
Mutex msg_mutex;

const Uint32 NUMBER_MSGS = 100000; 
const int NUMBER_CLIENTS = 20;
const int NUMBER_SERVERS = 10; 

FAKE_MESSAGE *get_next_msg(void *key )
{
   FAKE_MESSAGE *msg = 0;
   
   msg_mutex.lock(pegasus_thread_self());
   if(requests.value() < NUMBER_MSGS)
   {
      msg = new FAKE_MESSAGE(key, 0);
      requests++;
   }
   msg_mutex.unlock();
   return msg;
}

int main(int argc, char **argv)
{
   
   read_write rw =
      {
	 new AsyncDQueue<FAKE_MESSAGE>(true, 100), 
	 new AsyncDQueue<FAKE_MESSAGE>(true, 100)
      };

   Thread *client_sender[20];
   Thread *server[10];
   int i;


   
   for( i = 0; i < NUMBER_CLIENTS; i++)
   {
      client_sender[i] = new Thread(client_sending_thread, &rw, false );
      client_sender[i]->run();
   }
  
   for( i = 0; i < NUMBER_SERVERS; i++)
   {
      server[i] = new Thread(server_thread, &rw, false);
      server[i]->run();
   }

   while(requests.value() < NUMBER_MSGS || replies.value() < NUMBER_MSGS)
   {
      pegasus_sleep(1000);

      cout << "total requests: " << requests.value() << "; total replies: " << replies.value() << endl;
   }
   rw.incoming->shutdown_queue();
   rw.outgoing->shutdown_queue();

   for( i = 0; i < NUMBER_CLIENTS; i++)
   {
      client_sender[i]->join();
      delete client_sender[i];
   }
  
   for( i = 0; i < NUMBER_SERVERS; i++)
   {
      server[i]->join();
      delete server[i];
   }
   
   cout << NUMBER_MSGS << " messages using " << NUMBER_CLIENTS << " clients and " << NUMBER_SERVERS << " servers" << endl;
   cout << endl << "total requests: " << requests.value() << "; total replies: " << replies.value() << endl;
   cout << "unclaimed requests: " <<  rw.outgoing->count() << endl;
   delete rw.incoming;
   delete rw.outgoing;


   return 0;
}

PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL client_sending_thread(void *parm)
{
   Thread *my_handle = (Thread *)parm;
   read_write * my_qs = (read_write *)my_handle->get_parm();
   PEGASUS_THREAD_TYPE myself = pegasus_thread_self();

   Thread *receiver = new Thread(client_receiving_thread, my_qs, false);
   receiver->run();
   FAKE_MESSAGE *msg = 0;
   while( 1 )
   {
      msg = get_next_msg((void *)receiver);
      if(msg == 0)
	 break;
      try
      {
	 my_qs->incoming->insert_last_wait(msg);
      }
      catch(ListClosed & lc)
      {
	 break;
      }
      catch(IPCException & e)
      {
	 cout << endl << "IPC exception sending client msg" << endl;
	 abort();
      } 
      pegasus_yield();
   }
   
   while(my_qs->incoming->count() > 0 || my_qs->outgoing->count() > 0)
   {
      my_handle->sleep(1);
   }
   
   receiver->join();
   my_handle->exit_self((PEGASUS_THREAD_RETURN)1);
   return(0);
}

PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL server_thread(void *parm)
{

   Thread *my_handle = (Thread *)parm;
   read_write * my_qs = (read_write *)my_handle->get_parm();
   PEGASUS_THREAD_TYPE myself = pegasus_thread_self();
//   while( requests.value() < NUMBER_MSGS  || replies.value() < NUMBER_MSGS )
   while(1)
   {
      FAKE_MESSAGE *msg = 0;

      try
      {
	 msg = my_qs->incoming->remove_first_wait();
      }
      catch(ListClosed & lc ) 
      { 
//	 cout << endl << "client ---> server q is shutting down" << endl;
	 break;
      }
      catch(IPCException & e)
      {
	 cout << endl << "IPC exception retrieving client msg" << endl;
	 abort();
      } 
      if(msg != 0)
      {
	 try
	 {
	    my_qs->outgoing->insert_last_wait(msg);
	    msg = 0;
	 }
	 catch(ListClosed & lc)
	 {
	    cout << endl << "server ---> client q is shutting down" << endl;
	    break;
	 }
	 catch(IPCException & e)
	 {
	    cout << endl << "IPC exception dispatching client msg" << endl;
	    abort();
	 } 
      }
      pegasus_yield();
      
   }
   my_handle->exit_self((PEGASUS_THREAD_RETURN)1);
   return(0);
}

PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL client_receiving_thread(void *parm)
{

   Thread *my_handle = (Thread *)parm;
   read_write * my_qs = (read_write *)my_handle->get_parm();
   PEGASUS_THREAD_TYPE myself = pegasus_thread_self();
   while(replies.value() < NUMBER_MSGS )
   {
      FAKE_MESSAGE *msg = 0;
      try
      {
	 msg  = my_qs->outgoing->remove_wait((void *)my_handle);
      }
      catch(ListClosed & lc)
      {
//	 cout << endl << "server ---> client q is shutting down" << endl;
	 break;
      }
      catch(IPCException & e)
      {
	 cout << endl << "IPC exception receiving client msg" << endl;
	 abort();
      } 
      
      if(msg != 0 )
      {
	 delete msg;
	 replies++;
      }
      pegasus_yield();
   }
   my_handle->exit_self((PEGASUS_THREAD_RETURN)1);
   return(0);
}
