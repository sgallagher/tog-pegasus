//%2005////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
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
//         Ramnath Ravindran (Ramnath.Ravindran@compaq.com)
//         Amit K Arora, IBM (amita@in.ibm.com) for PEP#101
//         Alagaraja Ramasubramanian (alags_raj@in.ibm.com) for Bug#1090
//
//%/////////////////////////////////////////////////////////////////////////////
#if defined(PEGASUS_REMOVE_TRACE)   
#undef PEGASUS_REMOVE_TRACE 
#endif 
#include <Pegasus/suballoc/suballoc.h>
#include <Pegasus/Common/DQueue.h>  
#include <Pegasus/Common/Thread.h>
#include <Pegasus/Common/Tracer.h> 
#include <Pegasus/Common/AutoPtr.h> 
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

PEGASUS_NAMESPACE_BEGIN
 
class FAKE_MESSAGE 
{
   public:
      FAKE_MESSAGE(void){} 
      FAKE_MESSAGE(void *key, int type) 
	  : _type(type)
      { 
	 _key = (Thread *)key;
      }
      
      ~FAKE_MESSAGE(void)
      {
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


peg_suballocator::SUBALLOC_HANDLE * dq_handle;

const Uint32 NUMBER_MSGS = 1000;
const int NUMBER_CLIENTS = 2 ;
const int NUMBER_SERVERS =  1; 
 
FAKE_MESSAGE *get_next_msg(void *key)
{
   AutoPtr<FAKE_MESSAGE> msg;

   AutoMutex autoMut(msg_mutex);
   if(requests.value() < NUMBER_MSGS)
   {
      msg.reset(PEGASUS_NEW(FAKE_MESSAGE, dq_handle) FAKE_MESSAGE(key, 0));

/*****
      check for corrupted memory 
      char *p = (char *)msg;
      p += sizeof(FAKE_MESSAGE);

      *p = 0x00;
      
      if(peg_suballocator::CheckMemory(msg))
	 abort();
******/      
      
      
      
      requests++;
   }
    
   return msg.release();  
}
 
PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL client_sending_thread(void *parm)
{
   Thread *my_handle = (Thread *)parm;
   read_write * my_qs = (read_write *)my_handle->get_parm();
   PEGASUS_THREAD_TYPE myself = pegasus_thread_self();

   AutoPtr<Thread> receiver(new Thread(client_receiving_thread, my_qs, false));
   receiver->run();
   FAKE_MESSAGE *msg = 0;
   while( 1 )
   {
      msg = get_next_msg((void *)receiver.get());
      if(msg == 0)
	 break;
      try
      {
	 my_qs->incoming->insert_last_wait(msg);
      }
      catch(ListClosed & )
      {
	 break;
      }
      catch(IPCException & )
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
      catch(ListClosed & )
      {
	 break;
      }
      catch(IPCException & )
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
	 catch(ListClosed & )
	 {
	    break;
	 }
	 catch(IPCException & )
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
   while( 1 )
   {
      FAKE_MESSAGE *msg = 0;
      try
      {
	 msg  = my_qs->outgoing->remove_wait((void *)my_handle);
      }
      catch(ListClosed & )
      {
	 break;
      }
      catch(IPCException & )
      {
	 abort();
      }

      if(msg != 0 )
      {
//	 PEGASUS_DELETE(msg);
	 replies++;  
      }
      pegasus_yield();
   }
   my_handle->exit_self((PEGASUS_THREAD_RETURN)1);
   return(0);
}

PEGASUS_NAMESPACE_END

int main(int argc, char **argv)
{
    PEGASUS_START_LEAK_CHECK();
   
    const char* tmpDir = getenv ("PEGASUS_TMP");
    if (tmpDir == NULL)
    {
        tmpDir = ".";
    }
    String traceFile (tmpDir);
    traceFile.append("/dq_memory_trace");

   Tracer::setTraceFile (traceFile.getCString()); 
   Tracer::setTraceComponents("Memory");  
   Tracer::setTraceLevel(Tracer::LEVEL4); 
   AutoPtr<dq_handle>(new peg_suballocator::SUBALLOC_HANDLE());

   read_write rw =
      { 
	 new AsyncDQueue<FAKE_MESSAGE>(true, 100), 
	 new AsyncDQueue<FAKE_MESSAGE>(true, 100)
      };

   Thread *client_sender[20];
   Thread *server[10];
   int i;
   FAKE_MESSAGE *test = PEGASUS_ARRAY_NEW(FAKE_MESSAGE, 10, dq_handle.get());
   PEGASUS_ARRAY_DELETE(test);
   
   
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

   while( requests.value() < NUMBER_MSGS || replies.value() < NUMBER_MSGS )
   {
      pegasus_sleep(1000);
   }

   rw.incoming->shutdown_queue();
   rw.outgoing->shutdown_queue();

   PEGASUS_STOP_LEAK_CHECK();
   PEGASUS_CHECK_FOR_LEAKS(dq_handle.get());

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
