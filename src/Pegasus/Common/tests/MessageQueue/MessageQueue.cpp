//%2006////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
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
// Author:
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/PegasusAssert.h>
#include <Pegasus/Common/MessageQueue.h>

PEGASUS_USING_STD;
PEGASUS_USING_PEGASUS;

enum MessageTypes
{
    MESSAGE_ALARM = 1
};

class Alarm : public Message
{
public:

    Alarm(Uint32 key) : Message(MESSAGE_ALARM, key) { }

    virtual ~Alarm();
};

Alarm::~Alarm()
{

}

Uint32 Sum(const MessageQueue& q)
{
    Uint32 sum = 0;

    for (const Message* m = q.front(); m; m = m->getNext())
    {
	const Alarm* a = (const Alarm*)m;
	sum += a->getKey();
    }
    return sum;
}

void TestMessageQueue1()
{
    MessageQueue q;

    Uint32 sum = 0;

    for (Uint32 i = 1; i <= 5; i++)
    {
	q.enqueue(new Alarm(i));
	sum += i;
    }

    PEGASUS_TEST_ASSERT(Sum(q) == sum);

    // Test removing from the middle:
    Message* m = q.findByKey(3);
    PEGASUS_TEST_ASSERT(m != 0);
    q.remove(m);
    PEGASUS_TEST_ASSERT(Sum(q) == sum - 3);
    PEGASUS_TEST_ASSERT(q.getCount() == 4);

    // Test removing from the front:
    q.remove(q.front());
    PEGASUS_TEST_ASSERT(Sum(q) == sum - 3 - 1);
    PEGASUS_TEST_ASSERT(q.getCount() == 3);

    // Test removing from the front:
    q.remove(q.back());
    PEGASUS_TEST_ASSERT(Sum(q) == sum - 3 - 1 - 5);
    PEGASUS_TEST_ASSERT(q.getCount() == 2);

    // Test dequeue:
    m = q.dequeue();
    PEGASUS_TEST_ASSERT(m->getKey() == 2);
    PEGASUS_TEST_ASSERT(Sum(q) == sum - 3 - 1 - 5 - 2);
    PEGASUS_TEST_ASSERT(q.getCount() == 1);

    // Test dequeue:
    m = q.dequeue();
    PEGASUS_TEST_ASSERT(m->getKey() == 4);
    PEGASUS_TEST_ASSERT(Sum(q) == sum - 3 - 1 - 5 - 2 - 4);
    PEGASUS_TEST_ASSERT(q.getCount() == 0);
}

void TestMessageQueue2()
{
    MessageQueue q;

    Uint32 sum = 0;

    for (Uint32 i = 1; i <= 5; i++)
    {
	q.enqueue(new Alarm(i));
	sum += i;
    }
    PEGASUS_TEST_ASSERT(sum == 15);

    while (!q.isEmpty())
	q.remove(q.front());

    PEGASUS_TEST_ASSERT(q.getCount() == 0);
}

void TestMessageQueue3()
{
    MessageQueue q;

    Uint32 sum = 0;

    for (Uint32 i = 1; i <= 5; i++)
    {
	q.enqueue(new Alarm(i));
	sum += i;
    }
    PEGASUS_TEST_ASSERT(sum == 15);

    while (!q.isEmpty())
	q.remove(q.back());

    PEGASUS_TEST_ASSERT(q.getCount() == 0);
}


void TestMessageQueue4()
{
   MessageQueue q("a queue");
   MessageQueue r("another q");
   
   MessageQueue *found = MessageQueue::lookup("a queue");
   PEGASUS_TEST_ASSERT(found);
//   cout << found->getQueueName() << endl;
   found = MessageQueue::lookup("another q");
   PEGASUS_TEST_ASSERT(found);
//   cout << found->getQueueName() << endl;
}


int main()
{
    try
    {
	TestMessageQueue1();
	TestMessageQueue2();
	TestMessageQueue3();
	TestMessageQueue4();
	cout << "+++++ passed all tests" << endl;
    }
    catch (Exception& e)
    {
	cerr << e.getMessage() << endl;
    }
    
    return 0;
}
