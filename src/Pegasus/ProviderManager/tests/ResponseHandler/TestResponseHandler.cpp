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

int main(void)
{
    // instantiate the primary response handler types
    {
        ResponseHandler<void> handler = SimpleResponseHandler<void>();

        handler.processing();
        handler.complete();
    }

    {
        ResponseHandler<CIMObject> handler = SimpleResponseHandler<CIMObject>();

        handler.processing();
        handler.deliver(CIMObject());
        handler.complete();
    }

    {
        SimpleResponseHandler<CIMClass> handler = SimpleResponseHandler<CIMClass>();

        handler.processing();
        handler.deliver(CIMClass());
        handler.complete();
    }

    {
        SimpleResponseHandler<CIMInstance> handler = SimpleResponseHandler<CIMInstance>();

        handler.processing();
        handler.deliver(CIMInstance());
        handler.complete();
    }

    {
        SimpleResponseHandler<CIMObjectPath> handler = SimpleResponseHandler<CIMObjectPath>();

        handler.processing();
        handler.deliver(CIMObjectPath());
        handler.complete();
    }

    {
        SimpleResponseHandler<CIMValue> handler = SimpleResponseHandler<CIMValue>();

        handler.processing();
        handler.deliver(CIMValue());
        handler.complete();
    }

    cout << "+++++ passed all tests." << endl;

    return(0);
}
