
////////////////////////////////////////////////////////////////////////////////
//
// Triad.h
//
//	These classes provide simple representation of a template structures
//	three elements. Constructors are provided for initializing the
//	elements.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Triad_h
#define Pegasus_Triad_h

#include <Pegasus/Common/Config.h>

PEGASUS_NAMESPACE_BEGIN

template<class T, class U, class V>
struct Triad
{
    T first;
    U second;
    V third;

    Triad() : first(), second(), third() 
    { 

    }

    Triad(const Triad<T, U, V>& x)
	: first(x.first), second(x.second), third(x.third)
    {

    }

    Triad(const T& first_, const U& second_, const V& third_) 
	: first(first_), second(second_), third(third_)
    {

    }

    Triad& operator=(const Triad& x)
    {
	first = x.first;
	second = x.second;
	third = x.third;
	return *this;
    }
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Triad_h */
