#ifndef _Pegasus_Common_Atomic_ZOS_ZSERIES_IBM_h
#define _Pegasus_Common_Atomic_ZOS_ZSERIES_IBM_h

#include <Pegasus/Common/Config.h>

PEGASUS_NAMESPACE_BEGIN

struct Atomic
{ 
    cs_t _rep;
};

inline void Atomic_create(Atomic* v, int x)
{
    v->_rep = x;

}

inline void Atomic_destroy(Atomic* v)
{
}

inline int Atomic_get(const Atomic* v)
{
    return v->_rep;
}

inline void Atomic_inc(Atomic *v)
{
    Uint32 x = (Uint32)v->_rep;
    Uint32 old = x;
    x++;
    while ( cs( (cs_t*)&old, &(v->_rep), (cs_t)x) )
    {
       x = (Uint32)v->_rep;
       old = x;
       x++;
    }
}

inline int Atomic_dec_and_test(Atomic *v)
{
    Uint32 x = (Uint32)v->_rep;
    Uint32 old = x;
    x--;
    while ( cs( (cs_t*)&old, &(v->_rep), (cs_t) x) )
    {
       x = (Uint32) v->_rep;
       old = x;
       x--;
    }
    return x==0;
}

PEGASUS_NAMESPACE_END

#endif /* _Pegasus_Common_Atomic_ZOS_ZSERIES_IBM_h */
