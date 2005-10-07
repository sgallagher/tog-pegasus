#ifndef _Pegasus_Common_Atomic_Generic_h
#define _Pegasus_Common_Atomic_Generic_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/IPC.h>

PEGASUS_NAMESPACE_BEGIN

struct Atomic
{
    union
    {
	Uint64 alignment;
	char _rep[sizeof(AtomicInt)];
    };
};

inline void Atomic_create(Atomic* v, int x)
{
    new(&v->_rep) AtomicInt(x);
}

inline void Atomic_destroy(Atomic* v)
{
    ((AtomicInt*)&v->_rep)->~AtomicInt();
}

inline int Atomic_get(const Atomic* v)
{
    return ((AtomicInt*)&v->_rep)->value();
}

inline void Atomic_inc(Atomic *v)
{
    ((AtomicInt*)&v->_rep)->operator++();
}

inline int Atomic_dec_and_test(Atomic *v)
{
    return ((AtomicInt*)&v->_rep)->DecAndTestIfZero();
}

PEGASUS_NAMESPACE_END

#endif /* _Pegasus_Common_Atomic_Generic_h */
