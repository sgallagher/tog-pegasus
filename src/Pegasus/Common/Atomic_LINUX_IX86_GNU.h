#ifndef _Pegasus_Common_Atomic_LINUX_IX86_GNU_h
#define _Pegasus_Common_Atomic_LINUX_IX86_GNU_h

#include <Pegasus/Common/Config.h>

PEGASUS_NAMESPACE_BEGIN

struct Atomic
{ 
    volatile int counter; 
};

inline void Atomic_create(Atomic* v, int x)
{
    v->counter = x;
}

inline void Atomic_destroy(Atomic* v)
{
}

inline int Atomic_get(const Atomic* v)
{
    return v->counter;
}

inline void Atomic_set(Atomic* v, int x)
{
    v->counter = x;
}

static __inline__ void Atomic_inc(Atomic *v)
{
    __asm__ __volatile__(
	"lock ; incl %0"
	:"=m" (v->counter)
	:"m" (v->counter));
}

static __inline__ int Atomic_dec_and_test(Atomic *v)
{
    unsigned char c;

    __asm__ __volatile__(
	"lock ; decl %0; sete %1"
	:"=m" (v->counter), "=qm" (c)
	:"m" (v->counter) : "memory");

    return c != 0;
}

PEGASUS_NAMESPACE_END

#endif /* _Pegasus_Common_Atomic_LINUX_IX86_GNU_h */
