#ifndef _Pegasus_Common_Atomic_LINUX_PPC_GNU_h
#define _Pegasus_Common_Atomic_LINUX_PPC_GNU_h

#include <Pegasus/Common/Config.h>

PEGASUS_NAMESPACE_BEGIN

//------------------------------------------------------------------------------
// WARNING: this implementation does not work on multi-processor PPC 
// architectures or on the older IBM-405 processor (Mike Brasher).
//------------------------------------------------------------------------------

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
    int t;

    __asm__ __volatile__(
	"1: lwarx %0,0,%2\n"
	"addic %0,%0,1\n"
	"stwcx.	%0,0,%2\n"
	"bne- 1b"
	: "=&r" (t), "=m" (atomic->n)
	: "r" (&atomic->n), "m" (atomic->n)
	: "cc");
}

static __inline__ int Atomic_dec_and_test(Atomic* atomic)
{
    int c;

    __asm__ __volatile__(
	"1: lwarx %0,0,%1\n"
	"addic %0,%0,-1\n"
	"stwcx.	%0,0,%1\n"
	"bne- 1b"
	: "=&r" (c)
	: "r" (&atomic->n)
	: "cc", "memory");

    return c == 0;
}

PEGASUS_NAMESPACE_END

#endif /* _Pegasus_Common_Atomic_LINUX_PPC_GNU_h */
