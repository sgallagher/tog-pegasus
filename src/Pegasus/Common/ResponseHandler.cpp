//%2005////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
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
// Author: Chip Vincent (cvincent@us.ibm.com)
//
// Modified By: Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include "ResponseHandler.h"
#include "ResponseHandlerRep.h"
#include "InternalException.h"
#include "HashTable.h"
#include "IPC.h"

PEGASUS_NAMESPACE_BEGIN

PEGASUS_TEMPLATE_SPECIALIZATION struct HashFunc<void*>
{
    static Uint32 hash(void* x) { return Uint32((unsigned long)x) + 13; }
};

typedef HashTable<ResponseHandler*, ResponseHandlerRep*,
                  EqualFunc<void*>,
                  HashFunc<void*> > RepTable;

static RepTable repTable(512);
static Mutex repTableMutex;

ResponseHandlerRep* _newRep(
    ResponseHandler* object)
{
    ResponseHandlerRep* newRep = new ResponseHandlerRep();

    AutoMutex lock(repTableMutex);
    repTable.insert(object, newRep);
    return newRep;
}

ResponseHandlerRep* _newRep(
    ResponseHandler* object,
    const ResponseHandlerRep* rep)
{
    ResponseHandlerRep* newRep = new ResponseHandlerRep(*rep);

    AutoMutex lock(repTableMutex);
    repTable.insert(object, newRep);
    return newRep;
}

ResponseHandlerRep* _getRep(
    const ResponseHandler* object)
{
    ResponseHandlerRep* rep;
    Boolean found;

    AutoMutex lock(repTableMutex);
    found = repTable.lookup(const_cast<ResponseHandler*>(object), rep);
    PEGASUS_ASSERT(found == true);
    return rep;
}

void _deleteRep(
    ResponseHandler* object)
{
    ResponseHandlerRep* rep;
    Boolean found;

    AutoMutex lock(repTableMutex);
    found = repTable.lookup(object, rep);
    PEGASUS_ASSERT(found == true);
    delete rep;
    repTable.remove(object);
}


ResponseHandler::ResponseHandler()
{
    _newRep(this);
}

ResponseHandler::ResponseHandler(const ResponseHandler& handler)
{
    _newRep(this, _getRep(&handler));
}

ResponseHandler& ResponseHandler::operator=(const ResponseHandler& handler)
{
    if (&handler != this)
    {
        _deleteRep(this);
        _newRep(this, _getRep(&handler));
    }
    return *this;
}

ResponseHandler::~ResponseHandler()
{
    _deleteRep(this);
}

OperationContext ResponseHandler::getContext(void) const
{
    return(_getRep(this)->getContext());
}

void ResponseHandler::setContext(const OperationContext & context)
{
    _getRep(this)->setContext(context);
}

PEGASUS_NAMESPACE_END
