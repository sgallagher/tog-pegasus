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
// Author: Chip Vincent (cvincent@us.ibm.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Lockable_h
#define Pegasus_Lockable_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/IPC.h>

PEGASUS_NAMESPACE_BEGIN

template<class T>
class Lockable
{
public:
    Lockable(void);
    virtual ~Lockable(void);

    operator const T &(void) const;
    operator T &(void);

    void lock(void);
    void unlock(void);

protected:
    T _object;
    Mutex _mutex;

};

template<class T>
inline Lockable<T>::Lockable(void)
{
}

template<class T>
inline Lockable<T>::~Lockable(void)
{
}

template<class T>
inline Lockable<T>::operator const T &(void) const
{
    return(_object);
}

template<class T>
inline Lockable<T>::operator T &(void)
{
    return(_object);
}

template<class T>
inline void Lockable<T>::lock(void)
{
    _mutex.lock(pegasus_thread_self());
}

template<class T>
inline void Lockable<T>::unlock(void)
{
    _mutex.unlock();
}

PEGASUS_NAMESPACE_END

#endif
