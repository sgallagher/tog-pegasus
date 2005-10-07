#ifndef _Pegasus_Common_Atomic_LINUX_PPC_GNU_h
#define _Pegasus_Common_Atomic_LINUX_PPC_GNU_h

#include <Pegasus/Common/Config.h>

#if 0
# define PEGASUS_DCBT "dcbt " #ra "," #rb ";"
# else
# define PEGASUS_DCBT /* */
#endif

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

static __inline__ void Atomic_inc(Atomic* v)
{
    int t;

    __asm__ __volatile__(
	"1: lwarx %0,0,%2\n"
	"addic %0,%0,1\n"
	PEGASUS_DCBT
	"stwcx.	%0,0,%2\n"
	"bne- 1b"
	: "=&r" (t), "=m" (v->counter)
	: "r" (&v->counter), "m" (v->counter)
	: "cc");
}

static __inline__ int Atomic_dec_test(Atomic* v)
{
    int c;

    __asm__ __volatile__(
	"1: lwarx %0,0,%1\n"
	"addic %0,%0,-1\n"
	PEGASUS_DCBT
	"stwcx.	%0,0,%1\n\
	bne- 1b"
	: "=&r" (t)
	: "r" (&v->counter)
	: "cc", "memory");

    return c == 0;
}

PEGASUS_NAMESPACE_END

#endif /* _Pegasus_Common_Atomic_LINUX_PPC_GNU_h */
