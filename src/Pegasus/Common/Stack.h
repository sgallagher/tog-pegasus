//BEGIN_LICENSE
//
// Copyright (c) 2000 The Open Group, BMC Software, Tivoli Systems, IBM
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
//END_LICENSE
//BEGIN_HISTORY
//
// Author:
//
// $Log: Stack.h,v $
// Revision 1.1.1.1  2001/01/14 19:53:13  mike
// Pegasus import
//
//
//END_HISTORY

////////////////////////////////////////////////////////////////////////////////
//
// Stack.h
//
//	Simple stack implementation based on the Array<> class.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Stack_h
#define Pegasus_Stack_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Array.h>

PEGASUS_NAMESPACE_BEGIN

template<class T>
class Stack
{
public:

    Stack() { }

    Stack(const Stack& x) : _rep(x._rep) { }

    ~Stack() { }

    Stack& operator=(const Stack& x) { _rep = x._rep; return *this; }

    Boolean isEmpty() const { return _rep.getSize() == 0; }

    void push(const T& x) { _rep.append(x); }

    T& top();

    const T& top() const { return ((Stack<T>*)this)->top(); }

    void pop();

    Uint32 getSize() const { return _rep.getSize(); }

    T& operator[](Uint32 i) { return _rep[i]; }

    const T& operator[](Uint32 i) const { return _rep[i]; }

private:

    Array<T> _rep;
};

template<class T>
T& Stack<T>::top()
{
    if (!isEmpty())
	return _rep[_rep.getSize() - 1];
    else
    {
	static T dummy = T();
	return dummy;
    }
}

template<class T>
void Stack<T>::pop()
{
    if (_rep.getSize() == 0)
	throw StackUnderflow();

    _rep.remove(_rep.getSize() - 1);
}

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Stack_h */
