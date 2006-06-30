
//%2006////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// THE ABOVE COPYRIGHT NOTICE AND THIS PERMISSION NOTICE SHALL BE INCLUDED IN
// ALL COPIES OR SUBSTANTIAL PORTIONS OF THE SOFTWARE. THE SOFTWARE IS PROVIDED
// "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
// LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//==============================================================================
//
//%/////////////////////////////////////////////////////////////////////////////

#include <new>
#include "IDFactory.h"

#define PEGASUS_IDFACTORY_MAGIC 0x94E91236

PEGASUS_NAMESPACE_BEGIN

IDFactory::IDFactory()
{
    IDFactoryRep* rep = (IDFactoryRep*)_rep;
    PEGASUS_ASSERT(rep->magic != PEGASUS_IDFACTORY_MAGIC);

    new (rep) IDFactoryRep();
    rep->magic = PEGASUS_IDFACTORY_MAGIC;
    rep->next = 1;
}

IDFactory::~IDFactory()
{
    IDFactoryRep* rep = (IDFactoryRep*)_rep;
    rep->magic = 0xDDDDDDDD;
    // Note: never destructor mutex!
}

Uint32 IDFactory::getNext()
{
    IDFactoryRep* rep = (IDFactoryRep*)_rep;

    if (rep->magic != PEGASUS_IDFACTORY_MAGIC)
    {
	fprintf(stderr, 
	    "**************************************************************\n"
	    "**************************************************************\n"
	    "**************************************************************\n"
	    "**************************************************************\n"
	    "**************************************************************\n"
	    "**************************************************************\n"
	    "**************************************************************\n"
	    "**************************************************************\n"
	    "**************************************************************\n"
	    "**************************************************************\n"
	    "*** WARNING: IDFactory::getNext(): using destructed object ***\n"
	    "**************************************************************\n"
	    "**************************************************************\n"
	    "**************************************************************\n"
	    "**************************************************************\n"
	    "**************************************************************\n"
	    "**************************************************************\n"
	    "**************************************************************\n"
	    "**************************************************************\n"
	    "**************************************************************\n"
	    "**************************************************************\n");
    }

    rep->mutex.lock();
    Uint32 tmp = rep->next++;
    rep->mutex.unlock();

    return tmp;
}

PEGASUS_NAMESPACE_END
