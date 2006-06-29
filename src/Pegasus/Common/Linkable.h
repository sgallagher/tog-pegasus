#ifndef _Pegasus_Common_Linkable_h
#define _Pegasus_Common_Linkable_h

#include <Pegasus/Common/Config.h>
#include <cstring>

#define PEGASUS_LINKABLE_SANITY

#define PEGASUS_LINKABLE_MAGIC 0xAABBCCDD

PEGASUS_NAMESPACE_BEGIN


/** Non-virtual base class for anything that can be placed into a List.
*/
struct Linkable
{
    Linkable() : next(0), prev(0), list(0)
    {
#ifdef PEGASUS_LINKABLE_SANITY
	magic = PEGASUS_LINKABLE_MAGIC;
#endif
    }

    ~Linkable() 
    { 
#ifdef PEGASUS_LINKABLE_SANITY
	memset(this, 0xDD, sizeof(Linkable)); 
#endif
    }

    // ATTN: consider making private and fixing all places that have
    // copy constructors.
    Linkable(const Linkable&) : next(0), prev(0), list(0)
    {
#ifdef PEGASUS_LINKABLE_SANITY
	magic = PEGASUS_LINKABLE_MAGIC;
#endif
    }

#ifdef PEGASUS_LINKABLE_MAGIC
    Uint32 magic;
#endif
    Linkable* next;
    Linkable* prev;
    class ListRep* list;
};

PEGASUS_NAMESPACE_END

#endif /* _Pegasus_Common_Linkable_h */
