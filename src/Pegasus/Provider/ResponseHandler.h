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

#include <Pegasus/Common/CIMObject.h>
#include <Pegasus/Common/CIMObjectPath.h>
#include <Pegasus/Common/CIMValue.h>

#include <Pegasus/Common/CIMClass.h>
#include <Pegasus/Common/CIMInstance.h>
#include <Pegasus/Common/CIMIndication.h>

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
class PEGASUS_PROVIDER_LINKAGE BaseResponseHandler
{
public:

    /**
    Constructor.
    */
    BaseResponseHandler(void) : _rep(0)
    {
    }

    BaseResponseHandler(const BaseResponseHandler & handler)
    {
        Inc(_rep = handler._rep);
    }

    /**
    Destructor.
    */
    virtual ~BaseResponseHandler(void)
    {
        Dec(_rep);
    }

    BaseResponseHandler & operator=(const BaseResponseHandler & handler)
    {
        if(this == &handler)
        {
            return(*this);
        }

        Dec(_rep);
        Inc(_rep = handler._rep);

        return(*this);
    }

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
    //virtual void deliver(const T & object);

    /**
    Deliver a set of results to CIMOM.
    <p>This form of the <tt>deliver</tt> function may be used
    to return a set of elements to the CIMOM. The set is not
    required to be complete, and the provider may invoke this
    function multiple times, if necessary. This form should only
    be used when the operation requires a result consisting
    of more than one element, such as an enumeration.</p>
    */
    //virtual void deliver(const Array<T> & objects);

    /**
    Inform the CIMOM that delivery of results will begin.
    <p>The provider must call <tt>processing</tt> before
    attempting to call <tt>deliver</tt>.
    */
    virtual void processing(void)
    {
      if (_rep)  // Allow ResponseHandler<void> to use empty processing() method
        getRep()->processing();
    }

    /**
    Inform the CIMOM that delivery of results is complete.
    <p>The provider must call <tt>complete</tt> when all
    results have been delivered. It must not call <tt>deliver</tt>
    after calling <tt>complete</tt>.</p>
    */
    virtual void complete(void)
    {
      if (_rep)  // Allow ResponseHandler<void> to use empty complete() method
        getRep()->complete();
    }

protected:
    BaseResponseHandler(ResponseHandlerRep<T> * rep) : _rep(rep)
    {
    }

    ResponseHandlerRep<T> * getRep(void) const
    {
        if(_rep == 0)
        {
            throw UninitializedHandle();
        }

        return(_rep);
    }

protected:
    ResponseHandlerRep<T> * _rep;

};


template<class T>
class PEGASUS_PROVIDER_LINKAGE ResponseHandler : public BaseResponseHandler<T>
{
public:
};

//
// template specialization for ResponseHandler<CIMObject>
//
PEGASUS_TEMPLATE_SPECIALIZATION
class ResponseHandler<CIMObject> : public BaseResponseHandler<CIMObject>
{
public:
    ResponseHandler(void)
    {
    }
    ResponseHandler(const ResponseHandler & handler)
        : BaseResponseHandler<CIMObject>(handler)
    {
    }

    ResponseHandler & operator=(const ResponseHandler & handler)
    {
        BaseResponseHandler<CIMObject>::operator=(handler);

        return(*this);
    }

    virtual void deliver(const CIMObject & object)
    {
        getRep()->deliver(object);
    }
    virtual void deliver(const Array<CIMObject> & objects)
    {
        for(Uint32 i = 0, n = objects.size(); i < n; i++)
        {
            getRep()->deliver(objects[i]);
        }
    }

protected:
    ResponseHandler(ResponseHandlerRep<CIMObject> * rep)
        : BaseResponseHandler<CIMObject>(rep)
    {
    }

};


//
// template specialization for ResponseHandler<CIMClass>
//
PEGASUS_TEMPLATE_SPECIALIZATION
class ResponseHandler<CIMClass> : public BaseResponseHandler<CIMClass>
{
public:
    ResponseHandler(void)
    {
    }
    ResponseHandler(const ResponseHandler & handler)
        : BaseResponseHandler<CIMClass>(handler)
    {
    }

    ResponseHandler & operator=(const ResponseHandler & handler)
    {
        BaseResponseHandler<CIMClass>::operator=(handler);

        return(*this);
    }

    virtual void deliver(const CIMClass & object)
    {
        getRep()->deliver(object);
    }
    virtual void deliver(const Array<CIMClass> & objects)
    {
        for(Uint32 i = 0, n = objects.size(); i < n; i++)
        {
            getRep()->deliver(objects[i]);
        }
    }

protected:
    ResponseHandler(ResponseHandlerRep<CIMClass> * rep)
        : BaseResponseHandler<CIMClass>(rep)
    {
    }

};


//
// template specialization for ResponseHandler<CIMInstance>
//
PEGASUS_TEMPLATE_SPECIALIZATION
class ResponseHandler<CIMInstance> : public BaseResponseHandler<CIMInstance>
{
public:
    ResponseHandler(void)
    {
    }
    ResponseHandler(const ResponseHandler & handler)
        : BaseResponseHandler<CIMInstance>(handler)
    {
    }

    ResponseHandler & operator=(const ResponseHandler & handler)
    {
        BaseResponseHandler<CIMInstance>::operator=(handler);

        return(*this);
    }

    virtual void deliver(const CIMInstance & object)
    {
        getRep()->deliver(object);
    }
    virtual void deliver(const Array<CIMInstance> & objects)
    {
        for(Uint32 i = 0, n = objects.size(); i < n; i++)
        {
            getRep()->deliver(objects[i]);
        }
    }

protected:
    ResponseHandler(ResponseHandlerRep<CIMInstance> * rep)
        : BaseResponseHandler<CIMInstance>(rep)
    {
    }

};


//
// template specialization for ResponseHandler<CIMIndication>
//
PEGASUS_TEMPLATE_SPECIALIZATION
class ResponseHandler<CIMIndication> : public BaseResponseHandler<CIMIndication>
{
public:
    ResponseHandler(void)
    {
    }
    ResponseHandler(const ResponseHandler & handler)
        : BaseResponseHandler<CIMIndication>(handler)
    {
    }

    ResponseHandler & operator=(const ResponseHandler & handler)
    {
        BaseResponseHandler<CIMIndication>::operator=(handler);

        return(*this);
    }

    virtual void deliver(const CIMIndication & object)
    {
        getRep()->deliver(object);
    }
    virtual void deliver(const Array<CIMIndication> & objects)
    {
        for(Uint32 i = 0, n = objects.size(); i < n; i++)
        {
            getRep()->deliver(objects[i]);
        }
    }

protected:
    ResponseHandler(ResponseHandlerRep<CIMIndication> * rep)
        : BaseResponseHandler<CIMIndication>(rep)
    {
    }

};


//
// template specialization for ResponseHandler<CIMValue>
//
PEGASUS_TEMPLATE_SPECIALIZATION
class ResponseHandler<CIMValue> : public BaseResponseHandler<CIMValue>
{
public:
    ResponseHandler(void)
    {
    }
    ResponseHandler(const ResponseHandler & handler)
        : BaseResponseHandler<CIMValue>(handler)
    {
    }

    ResponseHandler & operator=(const ResponseHandler & handler)
    {
        BaseResponseHandler<CIMValue>::operator=(handler);

        return(*this);
    }

    virtual void deliver(const CIMValue & object)
    {
        getRep()->deliver(object);
    }
    virtual void deliver(const Array<CIMValue> & objects)
    {
        for(Uint32 i = 0, n = objects.size(); i < n; i++)
        {
            getRep()->deliver(objects[i]);
        }
    }

protected:
    ResponseHandler(ResponseHandlerRep<CIMValue> * rep)
        : BaseResponseHandler<CIMValue>(rep)
    {
    }

};


//
// template specialization for ResponseHandler<CIMObjectPath>
//
PEGASUS_TEMPLATE_SPECIALIZATION
class ResponseHandler<CIMObjectPath> : public BaseResponseHandler<CIMObjectPath>
{
public:
    ResponseHandler(void)
    {
    }
    ResponseHandler(const ResponseHandler & handler)
        : BaseResponseHandler<CIMObjectPath>(handler)
    {
    }

    ResponseHandler & operator=(const ResponseHandler & handler)
    {
        BaseResponseHandler<CIMObjectPath>::operator=(handler);

        return(*this);
    }

    virtual void deliver(const CIMObjectPath & object)
    {
        getRep()->deliver(object);
    }
    virtual void deliver(const Array<CIMObjectPath> & objects)
    {
        for(Uint32 i = 0, n = objects.size(); i < n; i++)
        {
            getRep()->deliver(objects[i]);
        }
    }

protected:
    ResponseHandler(ResponseHandlerRep<CIMObjectPath> * rep)
        : BaseResponseHandler<CIMObjectPath>(rep)
    {
    }

};

PEGASUS_NAMESPACE_END

#endif
