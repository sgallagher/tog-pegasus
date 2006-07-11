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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////


#include <Pegasus/Common/List.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/RecursiveMutex.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

class Person : public Linkable
{
public:

    Person(const String& name) : _name(name) { }

    ~Person() { }

    const String& name() const { return _name; }

    void print() const { cout << "Person(" << _name << ")" << endl; }

    static bool equal(const Person* person, const void* client_data)
    {
	return *((String*)client_data) == person->name();
    }

private:
    Person(const Person& x);
    Person& operator=(const Person& x);
    String _name;
};

typedef List<Person,NullLock> PersonList;
//typedef List<Person,RecursiveMutex> PersonList;

int main(int argc, char** argv)
{
    // Create list of persons:

    PersonList list;
    assert(list.size() == 0);
    list.insert_back(new Person("John"));
    assert(list.size() == 1);
    list.insert_back(new Person("Jane"));
    assert(list.size() == 2);
    list.insert_back(new Person("Joe"));
    assert(list.size() == 3);
    list.insert_back(new Person("Bob"));
    assert(list.size() == 4);

    // Print all elements of the list:

    {
	PersonList::AutoLock autoLock(list);

	for (Person* p = list.front(); p; p = list.next_of(p))
	{
	    // p->print();
	}
    }

    // Find "John":

    {
	const String JOHN = "John";
	Person* john = list.find(Person::equal, &JOHN);
	assert(john);
	// john->print();
    }

    // Remove "John" and "Jane":
    {
	const String JOHN = "John";
	Person* john = list.remove(Person::equal, &JOHN);
	assert(john->name() == "John");
	delete john;
	assert(list.size() == 3);

	const String JANE = "Jane";
	Person* jane = list.remove(Person::equal, &JANE);
	assert(jane->name() == "Jane");
	delete jane;
	assert(list.size() == 2);
    }

    // Clear the list:
    {
	list.clear();
	assert(list.size() == 0);
    }

    cout << argv[0] << " +++++ passed all tests" << endl;

    return 0;
}
