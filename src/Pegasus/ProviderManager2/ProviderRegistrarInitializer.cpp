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
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include "ProviderRegistrarInitializer.h"

#include <Pegasus/Repository/CIMRepository.h>
#include <Pegasus/Server/ProviderRegistrationManager/ProviderRegistrationManager.h>

#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/Logger.h>

PEGASUS_NAMESPACE_BEGIN

Array<RegistrationRecord> _globalRegistrationTable;

ProviderRegistrarInitializer::ProviderRegistrarInitializer(void)
{
}

ProviderRegistrarInitializer::~ProviderRegistrarInitializer(void)
{
}

void ProviderRegistrarInitializer::initialize(CIMRepository * repository)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "ProviderRegistrarInitializer::initialize()");

    Array<RegistrationRecord> records;

    //
    // get all relevant registration instances from the repository and cache them locally.
    //
    // validate instances and remove any that have dangling (incomplete) references. the simplest way to do this with
    // the current schema is to ensure that each provider capabilities instance has an associated provider module instance and
    // provider instance. Keep in mind that each class must have an entry. The provider capability instance is one-to-one
    // correlated to a class, so it makes sense to start there.
    //

    Array<CIMInstance> providerCapabilityInstances;
    Array<CIMInstance> providerInstances;
    Array<CIMInstance> providerModuleInstances;

    try
    {
        providerCapabilityInstances = repository->enumerateInstances("root/PG_interop", "PG_ProviderCapabilities");
        providerInstances = repository->enumerateInstances("root/PG_Interop", "PG_Provider");
        providerModuleInstances = repository->enumerateInstances("root/PG_Interop", "PG_ProviderModule");
    }
    catch(...)
    {
        // suppress exceptions
    }

    //
    // create registration records for each class entry
    //

    for(Uint32 i = 0, n = providerCapabilityInstances.size(); i < n; i++)
    {
        RegistrationRecord record;

        Uint32 pos = 0;

        // get class name
        if((pos = providerInstances[i].findProperty("Name")) != PEG_NOT_FOUND)
        {
            providerInstances[i].getProperty(pos).getValue().get(record.className);
        }

        // get namespace
        if((pos = providerInstances[i].findProperty("NameSpace")) != PEG_NOT_FOUND)
        {
            // ATTN: must create duplicate entries for each namespace. for now, only
            // one namespace is support (0 is invalid).

            //Array<String> temp;

            //providerInstances[i].getProperty(pos).getValue().get(temp);

            //record._namespace = temp[0];
        }

        // get provider name
        if((pos = providerInstances[i].findProperty("ProviderName")) != PEG_NOT_FOUND)
        {
            providerInstances[i].getProperty(pos).getValue().get(record.providerName);
        }

        // get provider module name
        if((pos = providerInstances[i].findProperty("ProviderModuleName")) != PEG_NOT_FOUND)
        {
            providerInstances[i].getProperty(pos).getValue().get(record.moduleName);
        }
    }

    // validate current registration records using cached instances.

    for(Uint32 i = 0, n = records.size(); i < n; i++)
    {
        Uint32 pos = 0;

        // find the provider for this record.
        for(Uint32 i = 0, n = providerInstances.size(); i < n; i++)
        {
            String s;

            // get name
            if((pos = providerInstances[i].findProperty("Name")) != PEG_NOT_FOUND)
            {
                providerInstances[i].getProperty(pos).getValue().get(s);
            }

            // compare record._providerName to PG_Provider.Name
            if(String::equalNoCase(s, records[i].providerName))
            {
                break;
            }
        }

        if(i == n)
        {
            // not found: must be an invalid entry

            // drop record and log
        }

        // find the provider module for this record.
        for(Uint32 i = 0, n = providerModuleInstances.size(); i < n; i++)
        {
            String s;

            // get name
            if((pos = providerModuleInstances[i].findProperty("Name")) != PEG_NOT_FOUND)
            {
                providerModuleInstances[i].getProperty(pos).getValue().get(s);
            }

            // compare record._moduleName to PG_ProviderModule.Name
            if(String::equalNoCase(s, records[i].moduleName))
            {
                break;
            }
        }

        if(i == n)
        {
            // not found: must be an invalid entry

            // drop record and log
        }

        // extract the location property, fully qualify and update the record._moduleName;
    }

    PEG_METHOD_EXIT();
}

PEGASUS_NAMESPACE_END
