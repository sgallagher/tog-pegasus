#include <Pegasus/Common/Config.h>
#include <iostream>
#include <fstream>
#include <cassert>
#include <CommExample/Comm/Comm.h>

using namespace Pegasus;
using namespace std;

#define TRACE

#ifdef TRACE
#  define T(X) X
#else
#  define T(X)
#endif

class MyHandler : public Handler
{
public:

    MyHandler() : _firstTime(true) 
    { 
	T( cout << "MyHandler::MyHandler()" << endl;  )
    }

    virtual ~MyHandler() 
    { 
	T( cout << "MyHandler::~MyHandler()" << endl;  )
    }

    virtual Boolean handleOpen(Connection* connection)
    {
	T( cout << "MyHandler::handleOpen()" << endl; )

	const char MESSAGE[] = "GET / HTTP/1.0\r\n\r\n";
	const size_t MESSAGE_SIZE = sizeof(MESSAGE) - 1;
	return connection->writeN(MESSAGE, MESSAGE_SIZE) == MESSAGE_SIZE;
    }

    virtual Boolean handleInput(Connection* connection)
    {
	T( cout << "MyHandler::handleInput()" << endl; )

	// Open file if this is the first time:

	static const char DOC_NAME[] = "message.http";

	if (_firstTime)
	{
	    _firstTime = false;
	    _os.open(DOC_NAME);
	    assert(!!_os);
	}

	// Read from connection and write to file until there is no more:

	char buffer[512];
	Sint32 size = connection->read(buffer, sizeof(buffer));

	if (size > 0)
	{
	    _os.write(buffer, size);
	    _os << flush;
	}
	else if (size <= 0)
	{
	    cout << "Created " << DOC_NAME << endl;
	    _os.close();
	    exit(1);
	}

	cout << "size=" << size << endl;

	return size != 0;
    }

    virtual Boolean handleClose(Connection* connection)
    {
	T( cout << "MyHandler::handleClose()" << endl; )
	return true;
    }

private:

    ofstream _os;
    Boolean _firstTime;
};

class MyHandlerFactory : public HandlerFactory
{
public:
    virtual Handler* create() { return new MyHandler; }
};

int main(int argc, char** argv)
{
    CommFactory* factory = CreateCommFactory("ACE");

    Connector* connector = factory->createConnector(new MyHandlerFactory);
    Connection* connection = connector->connect("www.bmc.com:80");
    assert(connection != 0);
    connector->run();
    
    return 0;
}
