//%2003////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002  BMC Software, Hewlett-Packard Development
// Company, L. P., IBM Corp., The Open Group, Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L. P.;
// IBM Corp.; EMC Corporation, The Open Group.
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
//              Steve Hills (steve.hills@ncr.com)
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/IPC.h>
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
//#include <malloc>


PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

Boolean die = false;

PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL reading_thread(void *parm);
PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL writing_thread(void *parm);
PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL test1_thread( void* parm );
PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL test2_thread( void* parm );
//PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL test3_thread( void* parm );
PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL test4_thread( void* parm );


AtomicInt read_count ;
AtomicInt write_count ;
AtomicInt testval1 = 0;

int main(int argc, char **argv)
{
	int i;

	{
	// Test return code
	Thread t( test1_thread, 0, false );
	t.run();
	t.join();
	if( t.get_exit() != (PEGASUS_THREAD_RETURN)32 )
	{
		cerr << "Error test return code" << endl;
		return 1;
	}
	}

	{
	// There shouldn't be a memory leak here
	Thread* threads[1000];
	for( i = 0; i < 1000; i++ )
	{
		threads[i] = new Thread( test2_thread, 0, false );
		threads[i]->put_tsd( "test1", thread_data::default_delete, 20000, new char[ 20000 ] );
		threads[i]->run();
	}
	for( i = 0; i < 500; i++ )
		threads[i]->join();
	for( i = 0; i < 500; i++ )
		delete threads[i];
	// TODO: Programatically check
	//pegasus_sleep( 10000 );
	}

	// NOTE: see test3_thread() comments
	//{
	//// Test proper canceling/thread exit
	//testval1 = 0;
	//Thread t( test3_thread, 0, false );
	//t.run();
	//t.cancel();
	//t.join();
	//if( testval1.value() != 42 )
	//{
	//	cerr << "Thread probably incorrectly terminated!" << endl;
	//	return 1;
	//}
	//}

	{
	// Test deadlocked thread handling
	Thread t( test4_thread, 0, false );
	t.run();
	t.cancel();
	cout << "If this hangs here then there is a thread deadlock handling bug..." << endl;
	t.join();
	// Shouldn't hang forever
	cout << "Deadlock test finished." << endl;
	}

   ReadWriteSem *rw = new ReadWriteSem();
   Thread *readers[40];
   Thread *writers[10];
   
   for(i = 0; i < 40; i++)
   {
      readers[i] = new Thread(reading_thread, rw, false);
      readers[i]->run();
   }
   
   for( i = 0; i < 10; i++)
   {
      writers[i] = new Thread(writing_thread, rw, false);
      writers[i]->run();
   }
   pegasus_sleep(20000); 
   die = true;
  
   for(i = 0; i < 40; i++)
   {
     readers[i]->join();
      delete readers[i];
   }

   for(i = 0; i < 10; i++)
   {
      writers[i]->join();
      delete writers[i];
   }

   delete rw;
   cout << endl << "read operations: " << read_count.value() << endl;
   cout << "write operations: " << write_count.value() << endl;

   return(0);
}


void deref(void *parm)
{
   
   Thread *my_handle = (Thread *)parm;
   try 
   {
      my_handle->dereference_tsd();
   }
   catch(IPCException& e)
   {
      e = e;
      cout << "exception dereferencing the tsd " << endl;
      abort();
   }
   return;
}

void exit_one(void *parm)
{
   
   Thread *my_handle = (Thread *)parm;
   cout << "1";
}

void exit_two(void *parm)
{
   
   Thread *my_handle = (Thread *)parm;
   cout << "2";
}

PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL reading_thread(void *parm)
{
   Thread *my_handle = (Thread *)parm;
   ReadWriteSem * my_parm = (ReadWriteSem *)my_handle->get_parm();
   
   PEGASUS_THREAD_TYPE myself = pegasus_thread_self();
   
   cout << "r";
   
   const char *keys[] = 
      {
	 "one", "two", "three", "four"
      };
   
   try
   {
      my_handle->cleanup_push(exit_one , my_handle );
   }
   catch(IPCException& e)
   {
      e = e;
      cout << "Exception while trying to push cleanup handler" << endl;
      abort();
   }
   
   try
   {
      my_handle->cleanup_push(exit_two , my_handle );
   }
   
   catch(IPCException& e)
   {
      e = e;
      cout << "Exception while trying to push cleanup handler" << endl;
      abort();
   }
   
   while(die == false) 
   {
      int i = 0;
      
#ifndef PEGASUS_PLATFORM_ZOS_ZSERIES_IBM
      char *my_storage = (char *)calloc(256, sizeof(char));
#else
      char *my_storage = (char *)::operator new(256);
#endif
      //    sprintf(my_storage, "%ld", myself + i);
      try 
      {
#ifndef PEGASUS_PLATFORM_ZOS_ZSERIES_IBM
	 my_handle->put_tsd(keys[i % 4], free, 256, my_storage);
#else
         my_handle->put_tsd(keys[i % 4], ::operator delete,
                            256, my_storage);              
#endif
      }
      catch(IPCException& e)
      {
      e = e;
      cout << "Exception while trying to put local storage: " << myself << endl;
      abort();
      }
      try 
      {
	 my_parm->wait_read(myself);
      }
      catch(IPCException& e)
      {
	 e = e;
	 cout << "Exception while trying to get a read lock" << endl;
	 abort();
      }
      
      read_count++;
      cout << "+";
      my_handle->sleep(1);

      try
      {
	 my_handle->cleanup_push(deref , my_handle );
      }
      catch(IPCException& e)
      {
	 e = e;
	 cout << "Exception while trying to push cleanup handler" << endl;
	 abort();
      }

      try 
      {
	 my_handle->reference_tsd(keys[i % 4]);
      }
      
      catch(IPCException& e)
      {
	 e = e;
	 cout << "Exception while trying to reference local storage" << endl;
	 abort();
      }
      
      try
      {
	 my_handle->cleanup_pop(true);
      }
            catch(IPCException& e)
      {
	 e = e;
	 cout << "Exception while trying to pop cleanup handler" << endl;
	 abort();
      }
      try 
      {
	 my_parm->unlock_read(myself);
      }
      catch(IPCException& e)
	{
	 e = e;
	 cout << "Exception while trying to release a read lock" << endl;
	 abort();
      }
      
      try 
      {
	 my_handle->delete_tsd(keys[i % 4]);
      }
      catch(IPCException& e)
      {
	 e = e;
	 cout << "Exception while trying to delete local storage: " << myself << endl;
	 abort();
      }
      i++;
   }
   my_handle->exit_self((PEGASUS_THREAD_RETURN)1);
   return(0);
}


PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL writing_thread(void *parm)
{
   
   Thread *my_handle = (Thread *)parm;
   ReadWriteSem * my_parm = (ReadWriteSem *)my_handle->get_parm();
   
   PEGASUS_THREAD_TYPE myself = pegasus_thread_self();
   
   cout << "w";
   
   while(die == false) 
   {
      try 
      {
	 my_parm->wait_write(myself);
      }
      catch(IPCException& e)
      {
	 e = e;
	 cout << "Exception while trying to get a write lock" << endl;
	 abort();
      }
      write_count++;
      cout << "*";
      my_handle->sleep(1);
      try 
      {
	 my_parm->unlock_write(myself);
      }
      catch(IPCException& e)
      {
	 e = e;
	 cout << "Exception while trying to release a write  lock" << endl;
	 abort();
      }
   }

   my_handle->exit_self((PEGASUS_THREAD_RETURN)1);
   return(0);
}

PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL test1_thread( void* parm )
{
	pegasus_sleep( 1000 );
	return( (PEGASUS_THREAD_RETURN)32 );
}

PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL test2_thread( void* parm )
{
	Thread* thread = (Thread*)parm;
	thread->exit_self( (PEGASUS_THREAD_RETURN)33 );
	return( (PEGASUS_THREAD_RETURN)32 );
}

void test3_thread_cleanup1(void*)
{
	testval1 = 42;
}

// NOTE: I don't think Thread::cleanup_push (and pop) will work
// with pthreads because the implicit cancelation point or the 
// test_cancel() call (which calls pthread_cancel()) will exit the
// thread without performing the Thread cleanup routines.
//PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL test3_thread( void* parm )
//{
//	Thread* thread = (Thread*)parm;
//	while( true )
//	{
//		testval1 = 0;
//		thread->cleanup_push( test3_thread_cleanup1, 0 );
//		pegasus_sleep( 2000 );
//		thread->test_cancel();
//		thread->cleanup_pop( false );
//	}
//	thread->exit_self( (PEGASUS_THREAD_RETURN)42 );
//	return( (PEGASUS_THREAD_RETURN)42 );
//}

PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL test4_thread( void* parm )
{
	Thread* thread = (Thread*)parm;
	// Simulate a deadlocked thread
	while( true )
	{
		pegasus_sleep( 2000 );
	}
	thread->exit_self( (PEGASUS_THREAD_RETURN)52 );
	return( (PEGASUS_THREAD_RETURN)52 );
}
