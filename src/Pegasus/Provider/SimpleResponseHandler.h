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

#ifndef Pegasus_SimpleResponseHandlerRep_h
#define Pegasus_SimpleResponseHandlerRep_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Provider/ResponseHandler.h>

PEGASUS_NAMESPACE_BEGIN

template<class T>
class PEGASUS_PROVIDER_LINKAGE SimpleResponseHandlerRep : public ResponseHandlerRep<T>
{
public:
    SimpleResponseHandlerRep(void)
    {
    }

    virtual void deliver(const T & object)
    {
        _objects.append(object);
    }

    const Array<T> & getObjects(void) const
    {
        return(_objects);
    }

private:
    Array<T> _objects;

};

template<class T>
class PEGASUS_PROVIDER_LINKAGE SimpleResponseHandler : public ResponseHandler<T>
{
public:
    SimpleResponseHandler(void)
        : ResponseHandler<T>(new SimpleResponseHandlerRep<T>())
    {
    }

    const Array<T> getObjects(void) const
    {
        return(reinterpret_cast<SimpleResponseHandlerRep<T> *>(this->getRep())->getObjects());
    }

};

PEGASUS_TEMPLATE_SPECIALIZATION
class SimpleResponseHandler<void> : public ResponseHandler<void>
{
public:
    SimpleResponseHandler(void)
        : ResponseHandler<void>(new ResponseHandlerRep<void>())
    {
    }

};

PEGASUS_NAMESPACE_END

#endif
