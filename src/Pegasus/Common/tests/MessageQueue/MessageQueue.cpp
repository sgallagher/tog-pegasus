#include <cassert>
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

    assert(Sum(q) == sum);

    // Test removing from the middle:
    Message* m = q.findByKey(3);
    assert(m != 0);
    q.remove(m);
    assert(Sum(q) == sum - 3);
    assert(q.getCount() == 4);

    // Test removing from the front:
    q.remove(q.front());
    assert(Sum(q) == sum - 3 - 1);
    assert(q.getCount() == 3);

    // Test removing from the front:
    q.remove(q.back());
    assert(Sum(q) == sum - 3 - 1 - 5);
    assert(q.getCount() == 2);

    // Test dequeue:
    m = q.dequeue();
    assert(m->getKey() == 2);
    assert(Sum(q) == sum - 3 - 1 - 5 - 2);
    assert(q.getCount() == 1);

    // Test dequeue:
    m = q.dequeue();
    assert(m->getKey() == 4);
    assert(Sum(q) == sum - 3 - 1 - 5 - 2 - 4);
    assert(q.getCount() == 0);
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
    assert(sum == 15);

    while (!q.isEmpty())
	q.remove(q.front());

    assert(q.getCount() == 0);
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
    assert(sum == 15);

    while (!q.isEmpty())
	q.remove(q.back());

    assert(q.getCount() == 0);
}

int main()
{
    try
    {
	TestMessageQueue1();
	TestMessageQueue2();
	TestMessageQueue3();
	cout << "+++++ passed all tests" << endl;
    }
    catch (Exception& e)
    {
	cerr << e.getMessage() << endl;
    }
    
    return 0;
}
