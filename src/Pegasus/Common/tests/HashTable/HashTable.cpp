#include <iostream>
#include <cassert>
#include <Pegasus/Common/HashTable.h>
#include <Pegasus/Common/String.h>

using namespace std;
using namespace Pegasus;

void test01()
{
    typedef HashTable<String, Uint32> HT;
    HT ht;

    assert(ht.insert("Red", 100));
    assert(ht.insert("Green", 200));
    assert(ht.insert("Blue", 300));
    assert(ht.getSize() == 3);

    Uint32 sum = 0;

    for (HT::Iterator i = ht.start(); i; i++)
    {
	// cout << "key[" << i.key() << "]" << endl;
	// cout << "value[" << i.value() << "]" << endl;
	sum += i.value();
    }

    HT ht2;
    ht2 = ht;
    sum = 0;

    for (HT::Iterator i = ht.start(); i; i++)
    {
	// cout << "key[" << i.key() << "]" << endl;
	// cout << "value[" << i.value() << "]" << endl;
	sum += i.value();
    }

    assert(sum == 600);

    Uint32 value = 0;
    assert(ht.lookup("Blue", value));
    assert(value == 300);

    assert(ht.remove("Red"));
    assert(ht.remove("Green"));
    assert(ht.getSize() == 1);

    ht.clear();
    assert(ht.getSize() == 0);

}

void test02()
{
    typedef HashTable<Uint32, Uint32> HT;
    HT ht;

    Uint32 sum = 0;
    const Uint32 N = 10000;

    for (Uint32 i = 0; i < N; i++)
    {
	sum += 2 * i;
	ht.insert(i, i);
    }

    HT ht2 = ht;

    assert(ht2.getSize() == N);

    Uint32 expectedSum = 0;
    Uint32 n = 0;

    for (HT::Iterator i = ht2.start(); i; i++)
    {
	n++;
	expectedSum += i.value() + i.key();
    }

    assert(expectedSum == sum);
    assert(n == N);
}

int main()
{
    test01();
    test02();

    cout << "+++++ passed all tests" << endl;

    return 0;
}
