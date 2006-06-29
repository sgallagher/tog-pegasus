#include <cassert>
#include "List.h"

#ifdef PEGASUS_LINKABLE_SANITY
# define LIST_ASSERT(COND) assert(COND)
#else
# define LIST_ASSERT(COND) /* empty */
#endif

#define PEGASUS_LIST_MAGIC 0x1234ABCD

PEGASUS_NAMESPACE_BEGIN

ListRep::ListRep(void (*destructor)(Linkable*)) : 
    _magic(PEGASUS_LIST_MAGIC), _front(0), _back(0), _size(0)
{
    if (destructor)
	_destructor = destructor;
    else
	_destructor = 0;
}

ListRep::~ListRep()
{
    LIST_ASSERT(_magic == PEGASUS_LIST_MAGIC);

    clear();
#ifdef PEGASUS_LINKABLE_SANITY
    memset(this, 0xDD, sizeof(ListRep));
#endif
}

void ListRep::clear()
{
    LIST_ASSERT(_magic == PEGASUS_LIST_MAGIC);

    if (_destructor)
    {
	for (Linkable* p = _front; p; )
	{
	    LIST_ASSERT(p->magic == PEGASUS_LINKABLE_MAGIC);
	    Linkable* next = p->next;
	    _destructor(p);
	    p = next;
	}

	_front = 0;
	_back = 0;
	_size = 0;
    }
}

void ListRep::insert_front(Linkable* elem)
{
    LIST_ASSERT(_magic == PEGASUS_LIST_MAGIC);
    LIST_ASSERT(elem != 0);
    LIST_ASSERT(elem->magic == PEGASUS_LINKABLE_MAGIC);
    LIST_ASSERT(elem->list == 0);

    elem->list = this;
    elem->next = _front;
    elem->prev = 0;

    if (_front)
	_front->prev = elem;
    else
	_back = elem;

    _front = elem;
    _size++;
}

void ListRep::insert_back(Linkable* elem)
{
    LIST_ASSERT(_magic == PEGASUS_LIST_MAGIC);
    LIST_ASSERT(elem != 0);
    LIST_ASSERT(elem->magic == PEGASUS_LINKABLE_MAGIC);
    LIST_ASSERT(elem->list == 0);

    elem->list = this;
    elem->prev = _back;
    elem->next = 0;

    if (_back)
	_back->next = elem;
    else
	_front = elem;

    _back = elem;
    _size++;
}

void ListRep::insert_after(
    Linkable* pos, 
    Linkable* elem)
{
    LIST_ASSERT(_magic == PEGASUS_LIST_MAGIC);
    LIST_ASSERT(pos != 0);
    LIST_ASSERT(elem != 0);
    LIST_ASSERT(elem->magic == PEGASUS_LINKABLE_MAGIC);
    LIST_ASSERT(pos->magic == PEGASUS_LINKABLE_MAGIC);
    LIST_ASSERT(elem->list == 0);

    elem->list = this;
    elem->prev = pos;
    elem->next = pos->next;

    if (pos->next)
	pos->next->prev = elem;

    pos->next = elem;

    if (pos == _back)
	_back = elem;

    _size++;
}

void ListRep::insert_before(
    Linkable* pos, 
    Linkable* elem)
{
    LIST_ASSERT(_magic == PEGASUS_LIST_MAGIC);
    LIST_ASSERT(pos != 0);
    LIST_ASSERT(elem != 0);
    LIST_ASSERT(pos->magic == PEGASUS_LINKABLE_MAGIC);
    LIST_ASSERT(elem->magic == PEGASUS_LINKABLE_MAGIC);
    LIST_ASSERT(elem->list == 0);

    elem->list = this;
    elem->next = pos;
    elem->prev = pos->prev;

    if (pos->prev)
	pos->prev->next = elem;

    pos->prev = elem;

    if (pos == _front)
	_front = elem;

    _size++;
}

void ListRep::remove(Linkable* pos)
{
    LIST_ASSERT(_magic == PEGASUS_LIST_MAGIC);
    LIST_ASSERT(pos != 0);
    LIST_ASSERT(pos->magic == PEGASUS_LINKABLE_MAGIC);
    LIST_ASSERT(pos->list == this);
    LIST_ASSERT(_size != 0);

    if (_size == 0)
	return;

    if (pos->prev)
	pos->prev->next = pos->next;

    if (pos->next)
	pos->next->prev = pos->prev;

    if (pos == _front)
	_front = pos->next;

    if (pos == _back)
	_back = pos->prev;

    pos->list = 0;

    _size--;
}

bool ListRep::contains(const Linkable* elem)
{
    LIST_ASSERT(_magic == PEGASUS_LIST_MAGIC);
    LIST_ASSERT(elem != 0);
    LIST_ASSERT(elem->magic == PEGASUS_LINKABLE_MAGIC);

    return elem && elem->list == this;

#if 0
    for (const Linkable* p = _front; p; p = p->next)
    {
	if (p == elem)
	    return true;
    }

    // Not found!
    return false;
#endif
}

Linkable* ListRep::remove_front()
{
    LIST_ASSERT(_magic == PEGASUS_LIST_MAGIC);

    if (_size == 0)
	return 0;

    Linkable* elem = _front;
    remove(elem);

    return elem;
}

Linkable* ListRep::remove_back()
{
    LIST_ASSERT(_magic == PEGASUS_LIST_MAGIC);
    LIST_ASSERT(_size > 0);

    Linkable* elem = _back;
    remove(elem);

    return elem;
}

Linkable* ListRep::find(ListRep::Equal equal, const void* client_data)
{
    LIST_ASSERT(_magic == PEGASUS_LIST_MAGIC);
    LIST_ASSERT(equal != 0);

    for (Linkable* p = _front; p; p = p->next)
    {
	if ((*equal)(p, client_data))
	{
	    LIST_ASSERT(p->magic == PEGASUS_LINKABLE_MAGIC);
	    return p;
	}
    }

    // Not found!
    return 0;
}

Linkable* ListRep::remove(ListRep::Equal equal, const void* client_data)
{
    LIST_ASSERT(_magic == PEGASUS_LIST_MAGIC);
    LIST_ASSERT(equal != 0);

    Linkable* p = find(equal, client_data);

    if (p)
    {
	LIST_ASSERT(p->magic == PEGASUS_LINKABLE_MAGIC);
	remove(p);
    }

    return p;
}

void ListRep::apply(ListRep::Apply apply, const void* client_data)
{
    LIST_ASSERT(_magic == PEGASUS_LIST_MAGIC);
    LIST_ASSERT(apply != 0);

    for (Linkable* p = _front; p; p = p->next)
	(*apply)(p, client_data);
}

PEGASUS_NAMESPACE_END
