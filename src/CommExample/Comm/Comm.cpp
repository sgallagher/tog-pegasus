//BEGIN_LICENSE
//
// Copyright (c) 2000 The Open Group, BMC Software, Tivoli Systems, IBM
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
//END_LICENSE
//BEGIN_HISTORY
//
// Author:
//
// $Log: Comm.cpp,v $
// Revision 1.1  2001/02/18 02:59:21  mike
// new
//
// Revision 1.1  2001/02/17 02:20:17  mike
// new
//
//
//END_HISTORY

#include <cstring>
#include "Comm.h"
#include "ACEComm.h"

PEGASUS_NAMESPACE_BEGIN

Connection::~Connection()
{

}

Handler::~Handler()
{

}

Connector::~Connector()
{

}

Acceptor::~Acceptor()
{

}

CommFactory* CreateCommFactory(const char* type)
{
    if (strcmp(type, "ACE") == 0)
	return new ACECommFactory;
    else
	return 0;
}

PEGASUS_NAMESPACE_END
