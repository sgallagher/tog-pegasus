#include <Pegasus/Common/Config.h>
#include <iostream>
#include <cassert>
#include "ACEComm.h"

using namespace Pegasus;
using namespace std;

class MyHandler : public Handler
{
public:

    MyHandler()
    {
	cout << "MyHandler::MyHandler()" << endl;
    }

    virtual ~MyHandler()
    {
	cout << "MyHandler::~MyHandler()" << endl;
    }

    virtual Boolean handleOpen(Connection* connection)
    {
	cout << "MyHandler::handleOpen()" << endl;

	const char MESSAGE[] = "GET / HTTP/1.0\r\n\r\n";
	const size_t MESSAGE_SIZE = sizeof(MESSAGE) - 1;
	return connection->writeN(MESSAGE, MESSAGE_SIZE) == MESSAGE_SIZE;
    }

    virtual Boolean handleInput(Connection* connection)
    {
	cout << "MyHandler::handleInput()" << endl;

	char buffer[16 * 1024];

	Sint32 size = connection->read(buffer, sizeof(buffer));

	cout << "size: " << size << endl;

	if (size > 0)
	{
	    buffer[size] = '\0';
	    cout << "buffer=" << buffer << endl;
	}

	return size != 0;
    }

    virtual Boolean handleClose(Connection* connection)
    {
	cout << "MyHandler::handleClose()" << endl;
	return true;
    }
};

class MyHandlerFactory : public HandlerFactory
{
public:
    virtual Handler* create() { return new MyHandler; }
};

int main(int argc, char** argv)
{
    ACECommFactory factory;

    Connector* connector = factory.createConnector(new MyHandlerFactory);
    Connection* connection = connector->connect("www.bmc.com:80");
    assert(connection != 0);
    connector->run();
    
    return 0;
}
