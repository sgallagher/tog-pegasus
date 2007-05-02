//%2006////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
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
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_SharedPtr_h
#define Pegasus_SharedPtr_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/AtomicInt.h>

PEGASUS_NAMESPACE_BEGIN

template<class T>
struct SharedPtrImpl
{
    AtomicInt refs;
    T* ptr;

    SharedPtrImpl(T* ptr_) : refs(1), ptr(ptr_)
    {
    }

    ~SharedPtrImpl()
    {
	delete ptr;
    }
};

template<class T>
inline void ref(const SharedPtrImpl<T>* impl_)
{
    SharedPtrImpl<T>* impl = (SharedPtrImpl<T>*)(impl_);

    if (impl)
    {
	impl->refs++;
    }
}

template<class T>
inline void unref(SharedPtrImpl<T>* impl_)
{
    SharedPtrImpl<T>* impl = (SharedPtrImpl<T>*)(impl_);

    if (impl && impl->refs.decAndTestIfZero())
	delete impl;
}

/** This class implements a shared pointer, similar to the one found in BOOST
    (and in countless other libraries). The implementation maintains a pointer
    to an intermediate object that contains the pointer to the target object
    as well as a reference counter. Because the target object need not have 
    a reference counter of its own, we say that this implementation is
    non-invasive (not imposing requirements on the target class). The instance
    diagram below depicts two SharedPtr objects sharing a target instance (of
    the class Monkey).

    \verbatim
    +-----------+
    | SharedPtr |
    +-----------+    +---------------+
    | _impl     |--->| SharedPtrImpl |
    +-----------+    +---------------+    +--------+
                     | ptr           |--->| Monkey | (target instance)
    +-----------+    +---------------+    +--------+
    | SharedPtr |--->| refs = 2      |
    +-----------+    +---------------+
    + _impl     |
    +-----------+
    \endverbatim

    The code snippet below constructs this instance diagram.

    \code
    SharedPtr p1(new Monkey);
    SharedPtr p2 = p1;
    \endcode

    The target instance is deleted when all shared pointers that refer to it
    are destroyed (or reset).

    A pointer may be reset, which causes the previous referent to be released.
    For example:

    \code
    SharedPtr p(new Monkey);

    // Release the old monkey (destroying it in this case).
    p.reset(new Monkey);
    \endcode

    SharedPtr implements pointer semantics by overloading the related operators.
    For example:

    \code
    SharedPtr p(new Monkey);

    if (p)
    {
	// Print the monkey object (print() is a member of the Monkey class).
	p->print();
    }
    \endcode

    Shared pointers may be dereferenced but beware that doing so to a null
    pointer is an error and will result in a crash.

    \code
    SharedPtr p(0);

    // Core dump! (dereferencing a null pointer).
    Monkey& monkey = *p;

    // Core dump! (dereferencing a null pointer).
    p->print();
    \endcode

    The get() method obtains the target pointer but beware that constructing
    a new SharedPtr with this pointer will result in double deletes (that is
    why the SharedPtr(T*) constructor is explicit). For example, avoid this:

    \code
    SharedPtr p1(new Monkey);
    SharedPtr p2(p1.get());
    \endcode
*/
template<class T>
class SharedPtr
{
public:

    /** Default constructor.
    */
    SharedPtr();

    /** Copy constructor.
    */
    SharedPtr(const SharedPtr<T>& x);

    /** Explicit constructor for creating a SharedPtr from a target pointer.
    */
    explicit SharedPtr(T* ptr);

    /** Destructor.
    */
    ~SharedPtr();

    /** Assignment operator.
    */
    SharedPtr& operator=(const SharedPtr& x);

    /** Reset the pointer to refer to the ptr argument. The previous target
	instance is released (deleted if this shared poitner was the only
	one referring to it).
    */
    void reset(T* ptr = 0);

    /** Operator.
    */
    const T* operator->() const;

    /** Operator.
    */
    T* operator->();

    /** Obtain a pointer to the target instance.
    */
    T* get() const;

    /** Operator.
    */
    T& operator*();

    /** Operator.
    */
    const T& operator*() const;

    /** Returns true if the target pointer is non-null.
    */
    operator bool() const;

    /** Returns true if the target pointer is null.
    */
    bool operator!() const;

    /** Returns the number of SharedPtr objects refering to the target
	object.
    */
    size_t count() const;

private:
    typedef SharedPtrImpl<T> Impl;
    Impl* _impl;
};

template<class T>
inline SharedPtr<T>::SharedPtr()
{
    _impl = new Impl(0);
}

template<class T>
inline SharedPtr<T>::SharedPtr(const SharedPtr<T>& x)
{
    ref(_impl = x._impl);
}

template<class T>
inline SharedPtr<T>::SharedPtr(T* ptr)
{
    _impl = new Impl(ptr);
}

template<class T>
inline SharedPtr<T>::~SharedPtr()
{
    unref(_impl);
}

template<class T>
SharedPtr<T>& SharedPtr<T>::operator=(const SharedPtr& x)
{
    if (_impl != x._impl)
    {
	unref(_impl);
	ref(_impl = x._impl);
    }
    return *this;
}

template<class T>
void SharedPtr<T>::reset(T* ptr)
{
    if (ptr == _impl->ptr)
	return;

    if (_impl->refs.get() == 1)
    {
	delete _impl->ptr;
	_impl->ptr = ptr;
    }
    else
    {
	unref(_impl);
	_impl = new Impl(ptr);
    }
}

template<class T>
inline const T* SharedPtr<T>::operator->() const
{
    return _impl->ptr;
}

template<class T>
inline T* SharedPtr<T>::operator->()
{
    return _impl->ptr;
}

template<class T>
inline T* SharedPtr<T>::get() const
{
    return _impl->ptr;
}

template<class T>
inline T& SharedPtr<T>::operator*()
{
    // Note: even with a shared pointer, it is possible to dereference a 
    // null pointer.
    return *_impl->ptr;
}

template<class T>
inline const T& SharedPtr<T>::operator*() const
{
    // Note: even with a shared pointer, it is possible to dereference a 
    // null pointer.
    return *_impl->ptr;
}

template<class T>
inline SharedPtr<T>::operator bool() const
{
    return _impl->ptr != 0;
}

template<class T>
inline bool SharedPtr<T>::operator!() const
{
    return _impl->ptr == 0;
}

template<class T>
inline size_t SharedPtr<T>::count() const
{
    return _impl->refs.get();
}

template<class T>
inline bool operator==(const SharedPtr<T>& x, const SharedPtr<T>& y)
{
    return x.ptr() == y.ptr();
}

PEGASUS_NAMESPACE_END

#endif /* Pegasus_SharedPtr_h */
