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

/**
Callback to deliver results to the CIMOM.

<p><tt>ResponseHandler</tt> is a template class. A
ResponseHandler class may be created for the various
types of object that a provider can deliver to the CIMOM.
A <tt>ResponseHandler</tt> object of the appropriate type
is passed to provider
functions that are invoked to process client requests (it
is not passed to the <tt>{@link initialize initialize}</tt>
or <tt>{@link terminate terminate}</tt> functions). It
contains the following public member functions that
may be used to deliver results to the CIMOM:</p>
<ul>
<li><tt>{@link processing processing}</tt> - inform the CIMOM
that delivery of results is beginning</li>
<li><tt>{@link deliver deliver}</tt> - deliver an incremental
result to the CIMOM; the CIMOM accumulates results as
they are received from the provider</li>
<li><tt>{@link complete complete}</tt> - inform the CIMOM that
process of the request is complete and that no further
results will be delivered</li>
<li><tt>{@link reserve reserve}</tt> - allocate memory for
results to be delivered; this may be used if the provider
must deliver a large amount of information, and knows in
advance how much. The CIMOM will release this memory
after processing is complete</li>
</ul>
*/

template<class T>
class PEGASUS_COMMON_LINKAGE ResponseHandler
{
public:

    /**
    Constructor.
    */
    ResponseHandler(void);
    ResponseHandler(const ResponseHandler & handler);

    /**
    Destructor.
    */
    virtual ~ResponseHandler(void);

    /**
    */
    ResponseHandler & operator=(const ResponseHandler & handler);

    /**
    Deliver a possibly partial result to CIMOM.
    <p>The <tt>deliver</tt> function is used by providers to
    deliver results to the CIMOM. For operations that require a
    single element result (<tt>getInstance</tt>, for example),
    <tt>deliver</tt> should be called only once to deliver the
    entire result. For operations that involve
    enumeration, the single-element form shown here may be
    used, each iteration delivering an incremental element
    of the total result. The Array form below may be used
    to deliver a larger set of result elements.</p>
    */
    virtual void deliver(const T & object);

    /**
    This signature is reserved for future use.
    */
    virtual void deliver(const OperationContext & context, const T & object);

    /**
    Deliver a set of results to CIMOM.
    <p>This form of the <tt>deliver</tt> function may be used
    to return a set of elements to the CIMOM. The set is not
    required to be complete, and the provider may invoke this
    function multiple times, if necessary. This form should only
    be used when the operation requires a result consisting
    of more than one element, such as an enumeration.</p>
    */
    virtual void deliver(const Array<T> & objects);

    /**
    This signature is reserved for future use.
    */
    virtual void deliver(const OperationContext & context, const Array<T> & objects);

    /**
    Pre-allocate memory for planned delivery of results.
    <p>The <tt>reserve</tt> function allows a provider to
    pre-allocate memory for the delivery of results. Ordinarily,
    the CIMOM accumulates results delivered in calls to <tt>deliver</tt>,
    and forwards them to the client when processing is complete. If
    it is considered useful, the provider may use <tt>reserve</tt>
    to allocate memory before calling <tt>deliver</tt>. If the
    call to <tt>reserve</tt> succeeds, the requested memory has been
    allocated, and subsequent calls to <tt>deliver</tt> will not
    fail for lack of memory.</p>
    */
    virtual void reserve(const Uint32 size);

    /**
    Inform the CIMOM that delivery of results will begin.
    <p>The provider must call <tt>processing</tt> before
    attempting to call <tt>deliver</tt>.
    */
    virtual void processing(void);

    /**
    Inform the CIMOM that deliver of results is complete.
    <p>The provider must call <tt>complete</tt> when all
    results have been delivered. It must not call <tt>deliver</tt>
    after calling <tt>complete</tt>.</p>
    */
    virtual void complete(void);

    /**
    This signature is reserved for future use.
    */
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
