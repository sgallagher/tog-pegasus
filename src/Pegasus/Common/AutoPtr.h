//%2005////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
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
// Author: Amit Arora, IBM (amita@in.ibm.com) (based on PEP101 by David Dillard)
//
// Modified By: Amit Arora, IBM (amita@in.ibm.com) for Bug#2168
//              Amit Arora, IBM (amita@in.ibm.com) for Bug#2480
//              David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_AutoPtr_h
#define Pegasus_AutoPtr_h

#include <Pegasus/Common/Config.h>

PEGASUS_NAMESPACE_BEGIN

/**
    This class provides a convenient way of disposing off a heap object.
    It automatically deletes the enclosed pointer on destruction. For
    example:

    <pre>
        A* a = new A;
        AutoPtr<A> dummy = a;
    </pre>

    When the AutoPtr object destructs, it frees the instance of A.
    This is particularly useful when a function has multiple returns.

    There are two classes here: AutoPtr<> and AutoArrayPtr<>. The
    AutoArrayPtr<> class is used when a pointer must be deleted using the
    array form as shown below:

    <pre>
        delete [] ptr;
    <pre>
*/

template<class X> class AutoPtr;

template<class X> class AutoPtrRef {
public:
    inline AutoPtrRef(AutoPtr<X> &ref) : _ref(ref)
    {
        // This block intentionally left blank
    }

    inline AutoPtr<X> get()
    {
        return(_ref);
    }

private:
    AutoPtr<X> &_ref;
};


template<class X> class AutoArrayPtr;

template<class X> class AutoArrayPtrRef {
public:
    inline AutoArrayPtrRef(AutoArrayPtr<X> &ref) : _ref(ref)
    {
       // This block intentionally left blank
    }

    inline AutoArrayPtr<X> get()
    {
        return(_ref);
    }

private:
    AutoArrayPtr<X> &_ref;
};



template<class X> class AutoPtr {
public:
    // This constructor helps this AutoPtr to take ownership of the memory
    // object pointed by p. It also acts as a default constructor (if no
    // argument is passed, it assigns a value of "0" to _ptr.
    // Example:  AutoPtr<SSLContext> sslContextA(new SSLContext());
    //           AutoPtr<SSLContext> sslContextB;
    //   sslContextB here has _ptr set to "0".
    inline explicit AutoPtr(X* p = 0) throw()
        : _ptr(p)
    {
        // This block intentionally left blank
    }

    // Destructor makes sure to delete the object pointed by _ptr thus
    // avoiding memory leaks
    inline ~AutoPtr() throw()
    {
        delete _ptr;
    }

    // The copy constructor takes the ownership of the heap object from
    // the source AutoPtr object. And since there should be only one
    // legitimate owner, it sets the _ptr of the source to "0"
    inline AutoPtr(AutoPtrRef<X> &a) throw()
        : _ptr((a.get()).release())
    {
        //a._ptr = 0;
    }

    inline AutoPtr(AutoPtr<X> &a) throw() : _ptr(a.release())
    {
    }

    // Overloading of "=" operator makes sure that the ownership of the memory
    // gets transferred properly to 'this' AutoPtr object.
    // Example:   AutoPtr<HTTPConnection> httpConnectionB = httpConnectionA;
    AutoPtr<X> &operator=(AutoPtr<X>& a) throw()
    {
        if ( this != &a )
            reset(a.release());
        return(*this);
    }

    inline AutoPtr<X> &operator=(AutoPtrRef<X> &a) throw()
    {
        reset((a.get()).release());
        return(*this);
    }

    // This method can be used to get the pointer to heap object encapsulated
    // in 'this' AutoPtr object
    // Example:   AutoPtr<classA> objA;
    //            func1(objA.get(), NULL);
    // Here func1() is a function which takes first argument as pointer to the
    // object of classA.
    inline X *get() const throw()
    {
        return(_ptr);
    }

    // Returns the heap object itself (not the pointer to it)
    inline X &operator*() const throw()
    {
        return(*_ptr);
    }

    // A very important overloading, which allows you to directly use 'this'
    // object as a pointer, whenever accessing a member variable/function of
    // the object pointed to by _ptr.
    // Example:   AutoPtr<classA> objA;
    //            objA->funcA();
    // funcA() is a function in the classA. Although objA is an AutoPtr, still
    // "->" operator would result in calling _ptr->funcA() because of this
    // overloading only.
    inline X *operator->() const throw()
    {
        return(_ptr);
    }

    // Relase the ownership of the memory object without deleting it !
    // Return the pointer to the heap object and set _ptr to "0".
    inline X* release() throw()
    {
        X *t = _ptr;
        _ptr = 0;
        return(t);
    }

    // Delete the heap object and thus release ownership
    inline void reset(X *p = 0) throw()
    {
        if (p != _ptr)
        {
            delete _ptr;
            _ptr = p;
        }
    }

    AutoPtr(AutoPtrRef<X> obj) throw()
        : _ptr((obj.get()).get())
    {
        // This block intentionally left blank
    }

    template<class Y> operator AutoPtrRef<Y>() throw()
    {
        return(AutoPtrRef<Y>(*this));
    }

    template<class Y> operator AutoPtr<Y>() throw()
    {
        return(AutoPtr<Y>(release()));
    }

private:
    // A pointer to the heap object
     X* _ptr;
};


template<class X> class AutoArrayPtr {
public:
    inline explicit AutoArrayPtr(X* p = 0) throw()
        : _ptr(p)
    {
        // This block intentionally left blank
    }

    inline ~AutoArrayPtr() throw()
    {
        delete [] _ptr;
    }

    inline AutoArrayPtr(AutoArrayPtrRef<X> &a) throw()
        : _ptr((a.get()).release())
    {
        //a._ptr = 0;
    }

    inline AutoArrayPtr(AutoArrayPtr<X> &a) throw() : _ptr(a.release())
    {
    }

    AutoArrayPtr<X> &operator=(AutoArrayPtr<X>& a) throw()
    {
        reset(a.release());
        return(*this);
    }

    inline AutoArrayPtr<X> &operator=(AutoArrayPtrRef<X> &a) throw()
    {
        if ( this != a )
            reset((a.get()).release());
        return(*this);
    }

    inline X *get() const throw()
    {
        return(_ptr);
    }

    inline X &operator*() const throw()
    {
        return(*_ptr);
    }

    inline X *operator->() const throw()
    {
        return(_ptr);
    }

    inline X* release() throw()
    {
        X *t = _ptr;
        _ptr = 0;
        return(t);
    }

    inline void reset(X *p = 0) throw()
    {
        if (p != _ptr)
        {
            // use "[]" since _ptr is pointer to an array
            delete [] _ptr;
            _ptr = p;
        }
    }

    AutoArrayPtr(AutoArrayPtrRef<X> obj) throw()
        : _ptr((obj.get()).get())
    {
        // This block intentionally left blank
    }

    template<class Y> operator AutoArrayPtrRef<Y>() throw()
    {
        return(AutoArrayPtrRef<Y>(*this));
    }

    template<class Y> operator AutoArrayPtr<Y>() throw()
    {
        return(AutoArrayPtr<Y>(release()));
    }


private:
    X* _ptr;
};


PEGASUS_NAMESPACE_END

#endif /* Pegasus_AutoPtr_h */
