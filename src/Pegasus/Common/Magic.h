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

#ifndef Pegasus_Magic_h
#define Pegasus_Magic_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Mutex.h>
#include <Pegasus/Common/Stack.h>

PEGASUS_NAMESPACE_BEGIN

/** The Magic class implements a convenient way to use magic numbers in
    user-defined classes. Magic numbers help detect use of uninitialized, 
    destructed, or corrupted objects.

    To instrument a class to use magic numbers simply add a class member 
    of type Magic<> as shown in thsi example:

	\code
	class MyClass
	{
	public:

	    MyClass();

	    ~MyClass();

	    void foo();

	private:
	    Magic<0xC531B144> _magic;
	};
	\endcode

    Choose whatever number you like for a magic number. The number above was
    generated by the Linux uuidgen utility (Windows has a utility with the 
    same name).

    To test magic number, add the following expression wherever necessary
    (usually as the first line of every member function).

	\code
	PEGASUS_ASSERT_DEBUG(_magic);
	\endcode

    Here's a typical example:

	\code
	MyClass::~MyClass()
	{
	    PEGASUS_ASSERT_DEBUG(_magic);
	}
	\endcode

    This tests whether the magic number is 0xC531B144 and asserts if it is
    not.

    Note that using a magic number makes the user-defined class 4 bytes larger
    but there is no run-time overhead unless you compile with PEGASUS_DEBUG.
    The Magic constructor and destructor are empty without PEGASUS_DEBUG and
    are discarded by the compiler.

    CAUTION: You may be tempted to compile out the magic member when
    PEGASUS_DEBUG undefined. However, this will causes unpredictable
    behavior when debug libraries are mixed with non-debug libraries. The
    structure alignment and size will be different and will lead to crashes.
    This is only safe if a class is internal to a library.
*/
template<Uint32 MAGIC_NUMBER>
class Magic
{
public:

    /** Default constructor. Sets the magic number.
    */
    Magic() 
    {
#ifdef PEGASUS_DEBUG
	_magic = MAGIC_NUMBER;
#endif
    }

    /** Destructor. Clears the magic number (with the pattern 0xDDDDDDDD).
    */
    ~Magic() 
    {
#ifdef PEGASUS_DEBUG
	_magic = 0xDDDDDDDD; 
#endif
    }

    /** Allows magic number to be used as a boolean expression. This function
	tests the magic number and return true if valid.
    */
    operator bool() const 
    { 
#ifdef PEGASUS_DEBUG
	return _magic == MAGIC_NUMBER; 
#else
	return true;
#endif
    }

private:
    Uint32 _magic;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Magic_h */
