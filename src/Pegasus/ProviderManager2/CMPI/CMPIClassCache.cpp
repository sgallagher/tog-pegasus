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

#include <Pegasus/ProviderManager2/CMPI/CMPIClassCache.h>
#include <Pegasus/ProviderManager2/CMPI/CMPI_Broker.h>
#include <Pegasus/Provider/CIMOMHandle.h>
#include <Pegasus/Common/Tracer.h>

PEGASUS_NAMESPACE_BEGIN

CMPIClassCache::~CMPIClassCache()
{
    // Cleanup the class cache
    ClassCache::Iterator i=_clsCache->start();
    for (; i; i++)
    {
        delete i.value();
    }
    delete _clsCache;
}

CIMClass* CMPIClassCache::getClass(
    const CMPI_Broker *mb,
    const CIMObjectPath & cop)
{
    String clsId =
        cop.getNameSpace().getString()+":"+cop.getClassName().getString();

    CIMClass *ccp;

    {
        ReadLock readLock(_rwsemClassCache);

        if (_clsCache->lookup(clsId,ccp))
        {
            return ccp;
        }
    }

    try
    {
        WriteLock writeLock(_rwsemClassCache);

        if (_clsCache->lookup(clsId,ccp))
        {
            return ccp;
        }
                      
        CIMClass cc = ((CIMOMHandle*)mb->hdl)->getClass(
            OperationContext(),
            cop.getNameSpace(),
            cop.getClassName(),
            (bool)0,
            (bool)1,
            (bool)0,
            CIMPropertyList());

        ccp = new CIMClass(cc);
        _clsCache->insert(clsId,ccp);
        return ccp;
    }
    catch (const CIMException &e)
    {
        PEG_TRACE_STRING(
            TRC_CMPIPROVIDERINTERFACE,
            Tracer::LEVEL1,
            "Exception: " + e.getMessage());
    }
    return 0;
}

PEGASUS_NAMESPACE_END
