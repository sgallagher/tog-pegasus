//%///////////////////////////////////////////////////////////////////////////
//
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


#include <Pegasus/Common/IPC.h>
#include <Pegasus/Common/Thread.h>
#include <sys/types.h>
#include <unistd.h>
#include <cassert>
#include <iostream>
#include <stdio.h>
#include <string.h>
//#include <malloc>


PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

Boolean die = false;

void *reading_thread(void *parm);
void *writing_thread(void *parm);


int main(int argc, char **argv)
{
   ReadWriteSem *rw = new ReadWriteSem();
   Thread *readers[40];
   Thread *writers[10];
   
   for(int i = 0; i < 40; i++)
   {
      readers[i] = new Thread(reading_thread, rw, false);
      readers[i]->run();
   }
   
   for( int i = 0; i < 10; i++)
   {
      writers[i] = new Thread(writing_thread, rw, false);
      writers[i]->run();
   }
   sleep(10);
   die = true;
   delete rw;
   for(int i = 0; i < 40; i++)
   {
      cout << " joining reader thread " << i << endl;
      readers[i]->join();
      delete readers[i];
   }

   for(int i = 0; i < 10; i++)
   {
      cout << " joining writer thread " << i << endl;
      writers[i]->join();
      delete writers[i];
   }
   
   return(0);
}


void *reading_thread(void *parm)
{
   Thread *my_handle = (Thread *)parm;
   ReadWriteSem * my_parm = (ReadWriteSem *)my_handle->get_parm();
   
   PEGASUS_THREAD_TYPE myself = pegasus_thread_self();
   
   cout << "I am a reader thread:" << myself  << endl;
   
   while(die == false) 
   {
      try 
      {
	 my_parm->wait_read(myself);
      }
      catch(IPCException& e)
      {
	 cout << "Exception while trying to get a read lock" << endl;
	 abort();
      }
      
      cout << "Thread " << myself << " reading..." << endl;
      my_handle->sleep(1);
      
      try 
      {
	 my_parm->unlock_read(myself);
      }
      catch(IPCException& e)
      {
	 cout << "Exception while trying to release a read lock" << endl;
	 abort();
      }
   }
   cout << "dying .." << myself << endl;
   my_handle->exit_self((PEGASUS_THREAD_RETURN)1);
   return(0);
}


void *writing_thread(void *parm)
{
   
   Thread *my_handle = (Thread *)parm;
   ReadWriteSem * my_parm = (ReadWriteSem *)my_handle->get_parm();
   
   PEGASUS_THREAD_TYPE myself = pegasus_thread_self();
   
   cout << "I am a writer  thread:" << myself  << endl;
   
   while(die == false) 
   {
      try 
      {
	 my_parm->wait_write(myself);
      }
      catch(IPCException& e)
      {
	 cout << "Exception while trying to get a write lock" << endl;
	 abort();
      }
      cout << "Thread " << myself << " writing...writing...writing....writing..." << endl;
      my_handle->sleep(1);
      try 
      {
	 my_parm->unlock_write(myself);
      }
      catch(IPCException& e)
      {
	 cout << "Exception while trying to release a write  lock" << endl;
	 abort();
      }
   }

   cout << "dying .." << myself << endl;
   
   my_handle->exit_self((void *)1);
   return(0);
}


void *tsd_thread(void *parm)
{
   Thread *my_handle = (Thread *)parm;
   PEGASUS_THREAD_TYPE myself = pegasus_thread_self();
   
   char *my_storage = (char *)calloc(256, sizeof(char));
   sprintf(my_storage, "%ld", myself);

   cout << "creating thread local storage: " << myself << endl;
   
   try 
   {
      my_handle->put_tsd(my_storage, free, 256, my_storage);
   }
   catch(IPCException& e)
   {
      cout << "Exception while trying to put local storage: " << myself << endl;
      abort();
   }
   
   cout << "deleting local storage: " << myself << endl;
   
   try 
   {
      my_handle->delete_tsd(my_storage);
   }
   catch(IPCException& e)
   {
      cout << "Exception while trying to delete local storage: " << myself << endl;
      abort();
   }
   cout << "dying .." << myself << endl;
   my_handle->exit_self((void *)1);
   return(0);
}
