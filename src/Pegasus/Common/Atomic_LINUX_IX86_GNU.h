#ifndef _Pegasus_Common_Atomic_LINUX_IX86_GNU_h
#define _Pegasus_Common_Atomic_LINUX_IX86_GNU_h

#include <Pegasus/Common/Config.h>

PEGASUS_NAMESPACE_BEGIN

struct Atomic
{ 
    volatile int n; 
};

inline void Atomic_create(Atomic* atomic, int x)
{
    atomic->n = x;
}

inline void Atomic_destroy(Atomic* atomic)
{
}

inline int Atomic_get(const Atomic* atomic)
{
    return atomic->n;
}

inline void Atomic_set(Atomic* atomic, int x)
{
    atomic->n = x;
}

static __inline__ void Atomic_inc(Atomic* atomic)
{
    __asm__ __volatile__(
	"lock ; incl %0"
	:"=m" (atomic->n)
	:"m" (atomic->n));
}

static __inline__ int Atomic_dec_and_test(Atomic* atomic)
{
    unsigned char c;

    __asm__ __volatile__(
	"lock ; decl %0; sete %1"
	:"=m" (atomic->n), "=qm" (c)
	:"m" (atomic->n) : "memory");

    return c != 0;
}

PEGASUS_NAMESPACE_END

#endif /* _Pegasus_Common_Atomic_LINUX_IX86_GNU_h */
