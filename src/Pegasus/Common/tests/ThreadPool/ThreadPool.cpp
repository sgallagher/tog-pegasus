//%/////////////////////////////////////////////////////////////////////////////
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

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/IPC.h>
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

PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL work_func(void *parm)
{
   Uint32 sleep_interval = (Uint32)parm;
      
   cout << "*";
   pegasus_sleep(sleep_interval);
   cout << "~";
   
   return 0; 
}  

int main(int argc, char **argv)
{
   
   struct timeval await = { 0, 40 };
   struct timeval dwait = { 10, 0 };
   struct timeval deadwait = { 5, 0 };

   ThreadPool tp(10, "test pool ", 5, 15, await, dwait, deadwait);

   int i = 0;
   
   for( ; i < 10; i++)
   { 
      try 
      {
	 tp.allocate_and_awaken((void *)0, work_func );
	 tp.allocate_and_awaken((void *)10, work_func );
	 tp.allocate_and_awaken((void *)20, work_func );
	 tp.allocate_and_awaken((void *)30, work_func );
	 tp.allocate_and_awaken((void *)40, work_func );
	 tp.allocate_and_awaken((void *)50, work_func );
	 tp.allocate_and_awaken((void *)60, work_func );
	 tp.allocate_and_awaken((void *)70, work_func );
	 tp.allocate_and_awaken((void *)80, work_func );
	 tp.allocate_and_awaken((void *)90, work_func );
	 tp.allocate_and_awaken((void *)10, work_func );
	 tp.allocate_and_awaken((void *)110, work_func );
	 tp.allocate_and_awaken((void *)120, work_func );
	 tp.allocate_and_awaken((void *)130, work_func );
	 tp.allocate_and_awaken((void *)140, work_func );
	 tp.allocate_and_awaken((void *)150, work_func );
	 tp.allocate_and_awaken((void *)1600, work_func );  
      }   
      catch(Deadlock & dl)
      {
	 cout << "thread " << (Uint32)dl.get_owner() << " timeout waiting for thread " << endl;
      }
      
   }
   cout << "deliberately causing deadlock detection to occur ..." << endl;
   pegasus_sleep( 7000 ) ;
   tp.kill_dead_threads( );  
   tp.kill_dead_threads( ) ;
    
   cout << "deadlock detection and cleanup successful, exiting" << endl;
   while(tp.running_count() )
   {
      pegasus_sleep(1);
   }
   
   return(1);
   
}



