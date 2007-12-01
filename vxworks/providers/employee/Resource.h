#ifndef _Resource_h
#define _Resource_h

#include <cimple/cimple.h>
#include "Employee.h"

CIMPLE_NAMESPACE_BEGIN

class Resource
{
public:

    Array<Employee*> employees;
    Mutex mutex;

    Resource();

    ~Resource();
};

extern Resource resource;

CIMPLE_NAMESPACE_END

#endif /* _Resource_h */
