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
// Author: Markus Mueller (sedgewick_de@yahoo.de)
//
// Modified By: Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//              Sean Keenan, Hewlett-Packard Company (sean.keenan@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/IPC.h>
#include <Pegasus/Common/Thread.h>
#include <Pegasus/Common/MessageQueue.h>
#include <sys/types.h>
#include <cassert>
#include <iostream>
#ifdef PEGASUS_OS_TYPE_WINDOWS
# include <windows.h>
#else
# include <unistd.h>
#endif

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

const Uint32 MY_CANCEL_TYPE=1000;

Boolean verbose = false;

// Fibonacci test parameter definition
class parmdef
{
public:
    parmdef()
    {
        th = NULL;
        cond_start = new Condition();
        mq = NULL;
    }

    ~parmdef()
    {
        delete cond_start;
    }

    int first;
    int second;
    int count;
    Thread * th;
    Condition * cond_start;
    MessageQueue * mq;
};


PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL fibonacci(void * parm)
{
    Thread* my_thread = (Thread *)parm;
    parmdef * Parm = (parmdef *)my_thread->get_parm();
    int first = Parm->first;
    int second = Parm->second;
    int count = Parm->count;
    Condition * condstart = Parm->cond_start;
    MessageQueue * mq = Parm->mq;
    
    condstart->signal(my_thread->self());

    int add_to_type = 0;
    if (count < 20)
        add_to_type = 100;

    for (int i=0; i < count; i++)
    {
        int sum = first + second;
        first = second;
        second = sum;
        Message * message = new Message(i+add_to_type, 0, sum);
        mq->enqueue(message);
    }

    if (!add_to_type)
        Parm->th->thread_switch();

    my_thread->exit_self(0);
    return NULL;
}

PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL deq(void * parm)
{
    Thread* my_thread = (Thread *)parm;
  
    parmdef * Parm = (parmdef *)my_thread->get_parm();
    Uint32 type, key;

    int first = Parm->first;
    int second = Parm->second;
    int count = Parm->count;
    Condition * condstart = Parm->cond_start;
    MessageQueue * mq = Parm->mq;
    
    condstart->signal(my_thread->self());

    Message * message;
    type = 0;
    key = 0;

    while (type != MY_CANCEL_TYPE)
    {
        message = mq->dequeue();
        while (!message) {
#if defined PEGASUS_OS_SOLARIS && defined SUNOS_5_6
	    pegasus_sleep(1);
#endif
            message = mq->dequeue();
        }

        key = message->getKey();
        type = message->getType();
        delete message;
        if (type == 19)
            assert(key == 10946);
    }

    if (verbose)
#if defined (PEGASUS_OS_VMS)
      // 
      // pegasus_thread_self returns long-long-unsigned.
      // 
      printf("Received Cancel Message, %llu about to end\n", pegasus_thread_self());
#else
        cout << "Received Cancel Message, " << pegasus_thread_self() <<
            " about to end\n";
#endif
    my_thread->exit_self(0);
    return NULL;
}

// Test Thread, MessageQueue, and Condition
int test01()
{
    MessageQueue * mq = new MessageQueue("testQueue", true);
    parmdef * parm[4];

    for (int i = 0; i < 4;i++)
    {
        parm[i] = new parmdef();
        parm[i]->mq = mq;
    }

    parm[0]->first = 0;
    parm[0]->second = 1;
    parm[0]->count = 20;

    parm[3]->first = 4;
    parm[3]->second = 6;
    parm[3]->count = 10;

    parm[0]->th = new Thread(fibonacci,parm[0],false);
    parm[1]->th = new Thread(deq,parm[1],false);
    parm[2]->th = new Thread(deq,parm[2],false);
    parm[3]->th = new Thread(fibonacci,parm[3],false);

    for (int i = 0; i < 4;i++)
    {
       parm[i]->cond_start->lock_object(pegasus_thread_self());
       parm[i]->th->run();
    }

    // Let the thread start and wait for Start Condition to be signaled
    for (int i = 0; i < 4;i++)
    {
        parm[i]->cond_start->unlocked_wait( pegasus_thread_self() );
        parm[i]->cond_start->unlock_object( );
    }

    // all fired up successfully

    // Finish the enqueueing tasks
    parm[0]->th->join();
    parm[3]->th->join();

    // Tell one of the dequeueing tasks to finish
    Message * message;
    message = new Message(MY_CANCEL_TYPE,0); 
    mq->enqueue(message);
    
    // Tell the other dequeueing task to finish
    message = new Message(MY_CANCEL_TYPE,0); 
    mq->enqueue(message);

    // Finish the dequeueing tasks
    parm[1]->th->join();
    parm[2]->th->join();

    // Clean up
    for (int i = 0; i < 4; i++)
    {
        delete parm[i]->th;
        delete parm[i];
    }

    delete mq;

    return 0;
}

PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL atomicIncrement(void * parm)
{
    Thread* my_thread  = (Thread *)parm;
    AtomicInt * atom = (AtomicInt *)my_thread->get_parm();

    (*atom)++;
    (*atom)+=4;
    (*atom)-=2;
    (*atom)--;
    Boolean zero = atom->DecAndTestIfZero();
    assert(zero == false);

    my_thread->exit_self(0);
    return 0;
}

// Test Thread and AtomicInt
void test02()
{
    const Uint32 numThreads = 64;
    AtomicInt * atom = new AtomicInt(0);
    Thread* threads[numThreads];

    (*atom)++;
    Boolean zero = atom->DecAndTestIfZero();
    assert(zero);

    for (Uint32 i=0; i<numThreads; i++)
    {
        threads[i] = new Thread(atomicIncrement, atom, false);
    }

    for (Uint32 i=0; i<numThreads; i++)
    {
        threads[i]->run();
    }

    for (Uint32 i=0; i<numThreads; i++)
    {
        threads[i]->join();
        delete threads[i];
    }

    assert(atom->value() == numThreads);
    delete atom;
}

int main(int argc, char** argv)
{
    verbose = (getenv("PEGASUS_TEST_VERBOSE")) ? true : false;

    for (Uint32 loop=0; loop<10; loop++)
    {
        test01();
    }
    if (verbose)
        cout << "+++++ passed test 1" << endl; 

    for (Uint32 loop=0; loop<10; loop++)
    {
        test02();
    }
    if (verbose)
        cout << "+++++ passed test 2" << endl; 

    cout << argv[0] << " +++++ passed all tests" << endl;

    return 0;
}
