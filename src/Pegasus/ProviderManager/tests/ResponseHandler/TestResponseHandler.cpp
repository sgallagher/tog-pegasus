//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software, Hewlett-Packard Company, IBM,
// The Open Group, Tivoli Systems
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
// Author: Chip Vincent (cvincent@us.ibm.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMInstance.h>

#include <Pegasus/Provider/ResponseHandler.h>
#include <Pegasus/Provider/SimpleResponseHandler.h>

PEGASUS_USING_PEGASUS;

#include <iostream>

PEGASUS_USING_STD;

void main(void)
{
    // instantiate the primary response handler types
    {
        SimpleResponseHandler<void> handler;

        handler.processing();
        handler.complete();
    }

    {
        SimpleResponseHandler<CIMObject> handler;

        handler.processing();
        handler.deliver(CIMObject());
        handler.complete();
    }

    {
        SimpleResponseHandler<CIMClass> handler;

        handler.processing();
        handler.deliver(CIMClass());
        handler.complete();
    }

    {
        SimpleResponseHandler<CIMInstance> handler;

        handler.processing();
        handler.deliver(CIMInstance());
        handler.complete();
    }

    {
        SimpleResponseHandler<CIMObjectPath> handler;

        handler.processing();
        handler.deliver(CIMObjectPath());
        handler.complete();
    }

    {
        SimpleResponseHandler<CIMValue> handler;

        handler.processing();
        handler.deliver(CIMValue());
        handler.complete();
    }

    try
    {
        SimpleResponseHandler<CIMInstance> simpleHandler;

        try
        {
            ResponseHandler<CIMInstance> handler;

            handler = simpleHandler;

            handler.processing();

            handler.deliver(CIMInstance("instance1"));
            handler.deliver(CIMInstance("instance2"));
            handler.deliver(CIMInstance("instance3"));
            handler.deliver(CIMInstance("instance4"));

            cout << "delivered " << simpleHandler.getObjects().size() << " objects" << endl;
        }
        catch(...)
        {
            throw;
        }

        cout << "received " << simpleHandler.getObjects().size() << " objects" << endl;

        cout << "+++++ passed all tests." << endl;
    }
    catch(Exception &)
    {
        cout << "failed" << endl;
    }
    catch(...)
    {
        cout << "failed" << endl;
    }
}
