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

#ifndef Pegasus_ResponseHandler_h
#define Pegasus_ResponseHandler_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/OperationContext.h>

#include <Pegasus/Provider/ResponseHandlerRep.h>

PEGASUS_NAMESPACE_BEGIN

template<class T>
class PEGASUS_COMMON_LINKAGE ResponseHandler
{
public:
    /**
    ATTN:
    */
    ResponseHandler(void);

    /**
    ATTN:
    */
    ResponseHandler(const ResponseHandler & handler);

    /**
    ATTN:
    */
    virtual ~ResponseHandler(void);

    /**
    ATTN:
    */
    ResponseHandler & operator=(const ResponseHandler & handler);

    /**
    ATTN:
    */
    virtual void deliver(const T & object);
    virtual void deliver(const OperationContext & context, const T & object);

    /**
    ATTN:
    */
    virtual void deliver(const Array<T> & objects);
    virtual void deliver(const OperationContext & context, const Array<T> & objects);

    /**
    ATTN:
    */
    virtual void reserve(const Uint32 size);

    /**
    ATTN:
    */
    virtual void processing(void);

    /**
    ATTN:
    */
    virtual void complete(void);
    virtual void complete(const OperationContext & context);

protected:
    ResponseHandler(ResponseHandlerRep<T> * rep);

    ResponseHandlerRep<T> * getRep(void) const;

protected:
    ResponseHandlerRep<T> * _rep;
};

template<class T>
inline ResponseHandler<T>::ResponseHandler(void) : _rep(0)
{
}

template<class T>
inline ResponseHandler<T>::ResponseHandler(const ResponseHandler<T> & handler)
{
    Inc(_rep = handler._rep);
}

template<class T>
inline ResponseHandler<T>::ResponseHandler(ResponseHandlerRep<T> * rep) : _rep(rep)
{
}

template<class T>
inline ResponseHandler<T>::~ResponseHandler(void)
{
    Dec(_rep);
}

template<class T>
inline ResponseHandler<T> & ResponseHandler<T>::operator=(const ResponseHandler<T> & handler)
{
    if(this == &handler)
    {
        return(*this);
    }

    Dec(_rep);
    Inc(_rep = handler._rep);

    return(*this);
}

template<class T>
inline ResponseHandlerRep<T> * ResponseHandler<T>::getRep(void) const
{
    if(_rep == 0)
    {
        throw UninitializedHandle();
    }

    return(_rep);
}

template<class T>
inline void ResponseHandler<T>::deliver(const T & object)
{
    getRep()->deliver(OperationContext(), object);
}

template<class T>
inline void ResponseHandler<T>::deliver(const OperationContext & context, const T & object)
{
    getRep()->deliver(context, object);
}

template<class T>
inline void ResponseHandler<T>::deliver(const Array<T> & objects)
{
    getRep()->deliver(OperationContext(), objects);
}

template<class T>
inline void ResponseHandler<T>::deliver(const OperationContext & context, const Array<T> & objects)
{
    getRep()->deliver(context, objects);
}

template<class T>
inline void ResponseHandler<T>::reserve(const Uint32 size)
{
    getRep()->reserve(size);
}

template<class T>
inline void ResponseHandler<T>::processing(void)
{
    getRep()->processing();
}

template<class T>
inline void ResponseHandler<T>::complete(void)
{
    getRep()->complete(OperationContext());
}

template<class T>
inline void ResponseHandler<T>::complete(const OperationContext & context)
{
    getRep()->complete(context);
}

PEGASUS_NAMESPACE_END

#endif
