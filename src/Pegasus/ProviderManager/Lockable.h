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
// Author: Chip Vincent (cvincent@us.ibm.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Lockable_h
#define Pegasus_Lockable_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/IPC.h>

template<class object_type>
class Lockable
{
public:
	Lockable(void);
	virtual ~Lockable(void);

	operator const object_type &(void) const;
	operator object_type &(void);
	
	void lock(void);
	void unlock(void);

protected:
	//Mutex _mutex;

};

template<class object_type>
inline Lockable<object_type>::Lockable(void)
{
}

template<class object_type>
inline Lockable<object_type>::~Lockable(void)
{
	//_mutex.unlock();
}

template<class object_type>
inline Lockable<object_type>::operator const object_type &(void) const
{
	return(_object);
}

template<class object_type>
inline Lockable<object_type>::operator object_type &(void)
{
	return(_object);
}

template<class object_type>
inline void Lockable<object_type>::lock(void)
{
	//_mutex.lock(pegasus_thread_self());
}

template<class object_type>
inline void Lockable<object_type>::unlock(void)
{
	//_mutex.unlock();
}

#endif
