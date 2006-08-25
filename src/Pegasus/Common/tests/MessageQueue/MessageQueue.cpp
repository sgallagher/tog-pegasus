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

    Alarm(Uint32 key) : Message(MESSAGE_ALARM), _key(key) { }

    virtual ~Alarm();

    Uint32 getKey() const
    {
        return _key;
    }

private:

    Uint32 _key;
};

Alarm::~Alarm()
{
}

void TestMessageQueue1()
{
    MessageQueue q("Test1");

    for (Uint32 i = 1; i <= 4; i++)
    {
	q.enqueue(new Alarm(i));
    }

    // Test dequeue:
    Message* m = q.dequeue();
    PEGASUS_TEST_ASSERT(((const Alarm*)m)->getKey() == 1);
    PEGASUS_TEST_ASSERT(q.getCount() == 3);
    PEGASUS_TEST_ASSERT(!q.isEmpty());

    m = q.dequeue();
    PEGASUS_TEST_ASSERT(((const Alarm*)m)->getKey() == 2);
    PEGASUS_TEST_ASSERT(q.getCount() == 2);
    PEGASUS_TEST_ASSERT(!q.isEmpty());

    m = q.dequeue();
    PEGASUS_TEST_ASSERT(((const Alarm*)m)->getKey() == 3);
    PEGASUS_TEST_ASSERT(q.getCount() == 1);
    PEGASUS_TEST_ASSERT(!q.isEmpty());

    m = q.dequeue();
    PEGASUS_TEST_ASSERT(((const Alarm*)m)->getKey() == 4);
    PEGASUS_TEST_ASSERT(q.getCount() == 0);
    PEGASUS_TEST_ASSERT(q.isEmpty());
}

void TestMessageQueue2()
{
    MessageQueue q("Test2");

    Uint32 sum = 0;

    for (Uint32 i = 1; i <= 5; i++)
    {
	q.enqueue(new Alarm(i));
	sum += i;
    }
    PEGASUS_TEST_ASSERT(sum == 15);

    while (!q.isEmpty())
        q.dequeue();

    PEGASUS_TEST_ASSERT(q.getCount() == 0);
}

void TestMessageQueue3()
{
    MessageQueue q("Test3");

    Uint32 sum = 0;

    for (Uint32 i = 1; i <= 5; i++)
    {
	q.enqueue(new Alarm(i));
	sum += i;
    }
    PEGASUS_TEST_ASSERT(sum == 15);

    while (!q.isEmpty())
        q.dequeue();

    PEGASUS_TEST_ASSERT(q.getCount() == 0);
}


void TestMessageQueue4()
{
   MessageQueue q("a queue");
   MessageQueue r("another q");
   
   MessageQueue *found = MessageQueue::lookup("a queue");
   PEGASUS_TEST_ASSERT(found);
   PEGASUS_TEST_ASSERT(!strcmp(found->getQueueName(), "a queue"));

   found = MessageQueue::lookup("another q");
   PEGASUS_TEST_ASSERT(found);
   PEGASUS_TEST_ASSERT(!strcmp(found->getQueueName(), "another q"));

   found = MessageQueue::lookup("no q");
   PEGASUS_TEST_ASSERT(!found);
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
