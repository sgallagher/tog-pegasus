//%/////-*-c++-*-//////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 The Open group, BMC Software, Tivoli Systems, IBM
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
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/OperationContext.h>

PEGASUS_NAMESPACE_BEGIN

template<class T>
class PEGASUS_COMMON_LINKAGE ResponseHandlerRep : public Sharable
{
public:
    ResponseHandlerRep(void);
    virtual ~ResponseHandlerRep(void);

    virtual void deliver(const OperationContext & context, const T & object) = 0;
    virtual void deliver(const OperationContext & context, const Array<T> & objects) = 0;

    virtual void reserve(const Uint32 size) = 0;

    virtual void processing(void) = 0;

    virtual void complete(const OperationContext & context) = 0;

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
