//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software, Hewlett-Packard Company, IBM,
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
// Author: Markus Mueller (sedgewick_de@yahoo.de)
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
#include <Pegasus/Common/MessageQueue.h>

#define D(X) /* empty */

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

#define MY_CANCEL_TYPE 1000

typedef struct {
    int first;
    int second;
    int count;
    Thread * th;
    Condition * cond_start;
    MessageQueue * mq;
    AtomicInt * aiPtr;
} parmdef;

void * fibonacci(void * parm);
void * deq(void * parm);

static AtomicInt ai;

static parmdef * setparm()
{
    parmdef * parm = new parmdef;
    parm->first = 0;
    parm->second = 1;
    parm->count = 20;
    parm->th = NULL;
    parm->cond_start = new Condition();
    parm->mq = NULL;
    parm->aiPtr = &ai;
    return parm;
};
    
Boolean die = false;


int main()
{
    void * retval;
    int ai2,ai3;

    Uint32 rn,rn2;

    MessageQueue * mq = new MessageQueue("testQueue", true);

    parmdef * parm[4];

    Thread * thr[4];

    for (int i = 0; i < 4;i++)
    {
        parm[i] = setparm();
        parm[i]->mq = mq;
    }

    parm[3]->first = 4;
    parm[3]->second = 6;
    parm[3]->count = 10;

    thr[0] = new Thread(fibonacci,parm[0],false);
    thr[1] = new Thread(deq,parm[1],false);
    thr[2] = new Thread(deq,parm[2],false);
    thr[3] = new Thread(fibonacci,parm[3],false);

    for (int i = 0; i < 4;i++)
    {
       parm[i]->th = thr[i];
       parm[i]->cond_start->lock_object(pegasus_thread_self());
       thr[i]->run();
    }

    // Let the thread start and wait for Start Condition to be signaled
    for (int i = 0; i < 4;i++)
    {
        parm[i]->cond_start->unlocked_wait( pegasus_thread_self() );
        parm[i]->cond_start->unlock_object( );
    }

    // all fired up successfully

    for (int i=0; i < 20; i++)
    {
        rn = (int) (4.0*rand()/(RAND_MAX+1.0)); 
        rn2 = (int) (2.0*rand()/(RAND_MAX+1.0)); 

	//        if (!rn2)
	//            parm[rn]->th->suspend();
	//        else 
	//            parm[rn]->th->resume();


	sched_yield();
//        sleep(1);
        cout << "+++++ passed test round " << i << endl; 

        ai2=ai.value();ai++;ai3=ai.value()-1;
        if (ai2 != ai3) cout << "thr 0: someone touched ai" << endl;
    }

    // Cancel the enqueueing tasks

//    sometimes these threads die leaving a conditional object
//    locked. kill them using our own means. 
    die = true;

//    parm[3]->th->cancel();
//    parm[0]->th->cancel();
//    parm[3]->th->cancel();
//    parm[0]->th->cancel();

    cout << "+++++ passed test round 20" << endl; 

    Message * message = new Message(MY_CANCEL_TYPE,0); 
    mq->enqueue(message);

#ifdef PEGASUS_PLATFORM_LINUX_IX86_GNU

    // Enforce that the first dequeuing task receives the cancel message
    // What happens if this thread holds a message queue lock ???
    parm[2]->th->suspend();

#endif


    //sleep(1); 

    message = new Message(MY_CANCEL_TYPE,0); 
    mq->enqueue(message);
    
    cout << "+++++ passed test round 21" << endl; 

    message = new Message(MY_CANCEL_TYPE,0); 
    mq->enqueue(message);

    cout << "+++++ passed test round 22" << endl; 
#ifdef PEGASUS_PLATFORM_LINUX_IX86_GNU
    parm[2]->th->resume();
#endif

    cout << "+++++ passed all tests" << endl; 

    // Make sure all threads end
    for (int i = 0; i<4; i++) parm[i]->th->cancel();

    parm[0]->th->join();
    parm[1]->th->join();
    parm[2]->th->join();
    parm[3]->th->join();
    return 0;
}

void * fibonacci(void * parm)
{
  Thread* my_thread  = (Thread *)parm;
  int ai2,ai3;
  
    parmdef * Parm = (parmdef *)my_thread->get_parm();
    int first = Parm->first;
    int second = Parm->second;
    int count = Parm->count;
    Condition * condstart = Parm->cond_start;
    MessageQueue * mq = Parm->mq;
    
    //cout << "fibonacci: " << pthread_self() << endl;

    condstart->signal( my_thread->self());

    int zw = 0;
    int add_to_type = 0;
    if (count < 20) add_to_type = 100;

    Message * message;

    while (die == false)
    {
        ai2=ai.value();ai++;ai3=ai.value()-1;
        if (ai2 != ai3) cout << "thr fib: someone touched ai" << endl;

        for (int i=0; i < count; i++)
        {
            zw = first + second;
            first = second;
            second = zw;
            message = new Message(i+add_to_type,zw);
            mq->enqueue(message);
        }
        zw = 0;
        first = Parm->first;
        second = Parm->second;
        if (!add_to_type)
            Parm->th->thread_switch();
    }

    return NULL;
}

void * deq(void * parm)
{
  Thread* my_thread  = (Thread *)parm;
  int ai2,ai3;
  
    parmdef * Parm = (parmdef *)my_thread->get_parm();
    Uint32 type, key;

    int first = Parm->first;
    int second = Parm->second;
    int count = Parm->count;
    Condition * condstart = Parm->cond_start;
    MessageQueue * mq = Parm->mq;
    
    //cout << "deq: " << pthread_self() << endl;

    condstart->signal(my_thread->self());

    Message * message;
    type = 0;
    key = 0;

    while (type != MY_CANCEL_TYPE)
    {
        ai2=ai.value();ai--;ai3=ai.value()+1;
        if (ai2 != ai3) cout << "thr deq: someone touched ai" << endl;

        message = mq->dequeue();
        if (!message) {
            break;
        }

        key = message->getKey();
        type = message->getType();
        delete message;
        if (type == 19)
            assert(key == 10946);
    }
    cout << "Received Cancel Message, " << pegasus_thread_self() << " about to end\n";
    return NULL;
}
