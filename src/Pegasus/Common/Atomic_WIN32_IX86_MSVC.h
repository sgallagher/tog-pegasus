#ifndef _Pegasus_Common_Atomic_WIN32_IX86_MSVC_h
#define _Pegasus_Common_Atomic_WIN32_IX86_MSVC_h

#include <Pegasus/Common/Config.h>
#include <windows.h>

PEGASUS_NAMESPACE_BEGIN

typedef LONG Atomic;

inline void Atomic_create(Atomic* atomic, int x)
{
    *atomic = LONG(x);
}

inline void Atomic_destroy(Atomic* atomic)
{
}

inline int Atomic_get(const Atomic* atomic)
{
    return int(*atomic);
}

inline void Atomic_set(Atomic* atomic, int x)
{
    *atomic = LONG(x);
}

static __inline__ void Atomic_inc(Atomic* atomic)
{
    InterlockedIncrement(atomic);
}

static __inline__ int Atomic_dec_and_test(Atomic* atomic)
{
    return InterlockedDecrement(atomic) == 0;
}

PEGASUS_NAMESPACE_END

#endif /* _Pegasus_Common_Atomic_WIN32_IX86_MSVC_h */
