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
// Author: Markus Mueller (sedgewick_de@yahoo.de)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/IPC.h>
#include <sys/types.h>
#include <unistd.h>
#include <cassert>
#include <iostream>
#include <Pegasus/Common/MessageQueue.h>
#include <Pegasus/Common/MT_MessageQueue.h>

#define D(X) /* empty */

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

#define MY_CANCEL_TYPE 1000

typedef struct {
    int first;
    int second;
    int count;
    SimpleThread * th;
    Condition * cond_start;
    MT_MessageQueue * mq;
} parmdef;

void * fibonacci(void * parm);
void * deq(void * parm);

static parmdef * setparm()
{
    parmdef * parm = new parmdef;
    parm->first = 0;
    parm->second = 1;
    parm->count = 20;
    parm->th = NULL;
    parm->cond_start = new Condition();
    parm->mq = NULL;
    return parm;
};
    
int main()
{
    void * retval;

    Uint32 rn,rn2;

    MT_MessageQueue * mq = new MT_MessageQueue();

    parmdef * parm[4];

    SimpleThread * thr[4];

    for (int i = 0; i < 4;i++)
    {
        parm[i] = setparm();
        parm[i]->mq = mq;
    }

    parm[3]->first = 4;
    parm[3]->second = 6;
    parm[3]->count = 10;

    thr[0] = new SimpleThread(fibonacci,parm[0],false);
    thr[1] = new SimpleThread(deq,parm[1],false);
    thr[2] = new SimpleThread(deq,parm[2],false);
    thr[3] = new SimpleThread(fibonacci,parm[3],false);

    for (int i = 0; i < 4;i++)
    {
       parm[i]->th = thr[i];
       parm[i]->cond_start->getMutex()->lock();
       thr[i]->run();
    }

    // Let the thread start and wait for Start Condition to be signaled
    for (int i = 0; i < 4;i++)
    {
        parm[i]->cond_start->unlocked_wait();
        parm[i]->cond_start->getMutex()->unlock();
    }

    // all fired up successfully

#ifndef PEGASUS_PLATFORM_HPUX_PARISC_ACC

    for (int i=0; i < 20; i++)
    {
        rn = (int) (4.0*rand()/(RAND_MAX+1.0)); 
        rn2 = (int) (2.0*rand()/(RAND_MAX+1.0)); 

        if (!rn2)
            parm[rn]->th->suspend();
        else 
            parm[rn]->th->resume();

        sleep(1);
        cout << "+++++ passed test round " << i << endl; 
    }

    // Cancel the enqueueing tasks

    parm[3]->th->cancel();
    parm[0]->th->cancel();

    parm[3]->th->kill(15);
    parm[0]->th->kill(15);

    // Enforce that the first dequeuing task receives the cancel message
    parm[2]->th->suspend();

    cout << "+++++ passed test round 20" << endl; 
#endif

    Message * message = new Message(MY_CANCEL_TYPE,0); 
    mq->enqueue(message);

    sleep(1);

    message = new Message(MY_CANCEL_TYPE,0); 
    mq->enqueue(message);
    
    parm[1]->th->join(&retval);

    cout << "+++++ passed test round 21" << endl; 

#ifndef PEGASUS_PLATFORM_HPUX_PARISC_ACC

    message = new Message(MY_CANCEL_TYPE,0); 
    mq->enqueue(message);

    //cout << "+++++ passed test round 22" << endl; 

    parm[2]->th->resume();

    parm[2]->th->join(&retval);

#endif
    
    cout << "+++++ passed all tests" << endl; 

    // Make sure all threads end
    for (int i = 0; i<4; i++) parm[i]->th->kill(9);

    return 0;
}

void * fibonacci(void * parm)
{
    parmdef * Parm = (parmdef *) parm;
    int first = Parm->first;
    int second = Parm->second;
    int count = Parm->count;
    Condition * condstart = Parm->cond_start;
    MT_MessageQueue * mq = Parm->mq;
    
    //cout << "fibonacci: " << pthread_self() << endl;

    condstart->signal();

    int zw = 0;
    int add_to_type = 0;
    if (count < 20) add_to_type = 100;

    Message * message;

    while (true)
    {
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
    Uint32 type, key;

    parmdef * Parm = (parmdef *) parm;

    int first = Parm->first;
    int second = Parm->second;
    int count = Parm->count;
    Condition * condstart = Parm->cond_start;
    MT_MessageQueue * mq = Parm->mq;
    
    //cout << "deq: " << pthread_self() << endl;

    condstart->signal();

    Message * message;
    type = 0;
    key = 0;

    while (type != MY_CANCEL_TYPE)
    {
        message = mq->dequeue();
        if (!message) break;

        key = message->getKey();
        type = message->getType();
        delete message;
        if (type == 19)
            assert(key == 10946);
    }
    cout << "Received Cancel Message, " << pthread_self() << " about to end\n";
    return NULL;
}
