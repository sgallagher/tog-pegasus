//%/////////////////////////////////////////////////////////////////////////////
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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Stack_h
#define Pegasus_Stack_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Exception.h>

PEGASUS_NAMESPACE_BEGIN

/** The Stack class provides a simple stack implementation.
    This class provides a stack implementation which is based on the Array<>
    template class.
*/
template<class T>
class Stack
{
public:

    /** */
    Stack() { }

    /** */
    Stack(const Stack<T>& x) : _rep(x._rep) { }

    /** */
    ~Stack() { }

    /** */
    Stack<T>& operator=(const Stack<T>& x) { _rep = x._rep; return *this; }

    /** isEmpty Tests to determine that the stack is empty
    */
    Boolean isEmpty() const { return _rep.size() == 0; }

    /**push adds one new entry to the stack
    */
    void push(const T& x) { _rep.append(x); }

    /** Top - Return the top entry on the stack. However, the entry is not
        removed from the stack.	 This is a peek at the top entry.
    */
    T& top();

    /** Top - Return the top entry on the stack. However, the entry is not
        removed from the stack
    */
    const T& top() const { return ((Stack<T>*)this)->top(); }

    /** Remove the top entry from the stack.
    */
    void pop();

    /** size returns the number of entries in the stack
     */
    Uint32 size() const { return _rep.size(); }

    /** The [] operator provides a way to treat the stack as an
        array so that entries within the stack can be manipulated.
    */
    T& operator[](Uint32 i) { return _rep[i]; }

    /** */
    const T& operator[](Uint32 i) const { return _rep[i]; }

private:

    Array<T> _rep;
};

template<class T>
T& Stack<T>::top()
{
    if (!isEmpty())
	return _rep[_rep.size() - 1];
    else
    {
	static T dummy = T();
	return dummy;
    }
}

template<class T>
void Stack<T>::pop()
{
    if (_rep.size() == 0)
	throw StackUnderflow();

    _rep.remove(_rep.size() - 1);
}

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Stack_h */
