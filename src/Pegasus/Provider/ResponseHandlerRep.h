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

#ifndef Pegasus_ResponseHandlerRep_h
#define Pegasus_ResponseHandlerRep_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Sharable.h>

#include <Pegasus/Common/CIMObject.h>
#include <Pegasus/Common/CIMClass.h>
#include <Pegasus/Common/CIMInstance.h>
#include <Pegasus/Common/CIMIndication.h>
#include <Pegasus/Common/CIMValue.h>
#include <Pegasus/Common/CIMObjectPath.h>

PEGASUS_NAMESPACE_BEGIN

template<class T>
class PEGASUS_COMMON_LINKAGE ResponseHandlerRep : public Sharable
{
public:
    ResponseHandlerRep(void);
    virtual ~ResponseHandlerRep(void);

    virtual void processing(void) = 0;

    virtual void complete() = 0;

    virtual void deliver(const CIMObject & object)
    {
    }

    virtual void deliver(const Array<CIMObject> & objects)
    {
    }

    virtual void deliver(const CIMClass & object)
    {
    }

    virtual void deliver(const Array<CIMClass> & objects)
    {
    }

    virtual void deliver(const CIMInstance & object)
    {
    }

    virtual void deliver(const Array<CIMInstance> & objects)
    {
    }

    virtual void deliver(const CIMIndication & object)
    {
    }

    virtual void deliver(const Array<CIMIndication> & objects)
    {
    }

    virtual void deliver(const CIMValue & object)
    {
    }

    virtual void deliver(const Array<CIMValue> & objects)
    {
    }

    virtual void deliver(const CIMObjectPath & object)
    {
    }

    virtual void deliver(const Array<CIMObjectPath> & objects)
    {
    }

};

template<class T>
inline ResponseHandlerRep<T>::ResponseHandlerRep(void)
{
}

template<class T>
inline ResponseHandlerRep<T>::~ResponseHandlerRep(void)
{
}

PEGASUS_NAMESPACE_END

#endif
