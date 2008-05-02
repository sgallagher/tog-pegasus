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

#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/StringConversion.h>
#include <Pegasus/Server/ProviderRegistrationManager/\
ProviderRegistrationManager.h>

#include "ProviderIndicationCountTable.h"

PEGASUS_NAMESPACE_BEGIN

Uint32 ProviderIndicationCountTable::_ProviderIndicationCountHashFunc::hash(
    const String& key)
{
    Uint32 hashCode = 0;

    const Uint16* p = (const Uint16*)key.getChar16Data();
    Uint32 keySize = key.size();

    if (keySize > 1)
    {
        hashCode = p[0] + p[keySize/2] + 3*p[keySize - 1];
    }

    return hashCode;
}

ProviderIndicationCountTable::ProviderIndicationCountTable()
{
}

ProviderIndicationCountTable::~ProviderIndicationCountTable()
{
}

void ProviderIndicationCountTable::insertEntry(
    const CIMInstance& providerInstance) 
{
    PEG_METHOD_ENTER(TRC_INDICATION_SERVICE,
        "ProviderIndicationCountTable::insertEntry");

    String providerModuleName;
    String providerName;
    getProviderKeys(providerInstance, providerModuleName, providerName);

    String providerKey = _generateKey(providerModuleName, providerName);
    _ProviderIndicationCountTableEntry entry;

    WriteLock lock(_tableLock);

    if (!_table.lookup(providerKey, entry))
    {
        //
        // The entry is not in the table yet; insert a new entry.
        //
        _ProviderIndicationCountTableEntry newEntry;
        newEntry.providerModuleName = providerModuleName;
        newEntry.providerName = providerName;
        newEntry.indicationCount = 0;
        newEntry.orphanIndicationCount = 0;

        Boolean succeeded = _table.insert(providerKey, newEntry);
        PEGASUS_ASSERT(succeeded);
    }

    PEG_METHOD_EXIT();
}

void ProviderIndicationCountTable::incrementEntry(
    const CIMInstance& providerInstance,
    Boolean isOrphan)
{
    PEG_METHOD_ENTER(TRC_INDICATION_SERVICE,
        "ProviderIndicationCountTable::incrementEntry");

    String providerModuleName;
    String providerName;
    getProviderKeys(providerInstance, providerModuleName, providerName);

    String providerKey = _generateKey(providerModuleName, providerName);
    _ProviderIndicationCountTableEntry* entry = 0;

    WriteLock lock(_tableLock);

    if (_table.lookupReference(providerKey, entry))
    {
        entry->indicationCount++;

        if (isOrphan)
        {
            entry->orphanIndicationCount++;
        }
    }

    PEG_METHOD_EXIT();
}

void ProviderIndicationCountTable::removeEntry(
    const CIMInstance& providerInstance)
{
    PEG_METHOD_ENTER(TRC_INDICATION_SERVICE,
        "ProviderIndicationCountTable::removeEntry");

    String providerModuleName;
    String providerName;
    getProviderKeys(providerInstance, providerModuleName, providerName);
    String providerKey = _generateKey(providerModuleName, providerName);

    WriteLock lock(_tableLock);
    _table.remove(providerKey);

    PEG_METHOD_EXIT();
}

void ProviderIndicationCountTable::removeModuleEntries(
    const String& providerModuleName)
{
    PEG_METHOD_ENTER(TRC_INDICATION_SERVICE,
        "ProviderIndicationCountTable::removeModuleEntries");

    {
        WriteLock lock(_tableLock);
        Array<String> keysToRemove;

        // First collect a list of ProviderIndicationCountTable entries for
        // this provider module.
        for (_ProviderIndicationCountTable::Iterator i = _table.start(); i; i++)
        {
            if (i.value().providerModuleName == providerModuleName)
            {
                keysToRemove.append(i.key());
            }
        }

        // Now remove the entries, outside the Iterator scope.
        for (Uint32 i = 0; i < keysToRemove.size(); i++)
        {
            Boolean isRemoved = _table.remove(keysToRemove[i]);
            PEGASUS_ASSERT(isRemoved);
        }
    }

    PEG_METHOD_EXIT();
}

void ProviderIndicationCountTable::getProviderKeys(
    const CIMInstance& providerInstance,
    String& providerModuleName,
    String& providerName)
{
    Array<CIMKeyBinding> keys = providerInstance.getPath().getKeyBindings();

    for (Uint32 i = 0; i < keys.size(); i++)
    {
        if (keys[i].getName() == PEGASUS_PROPERTYNAME_NAME)
        {
            providerName = keys[i].getValue();
        }
        else if (keys[i].getName() == _PROPERTY_PROVIDERMODULENAME)
        {
            providerModuleName = keys[i].getValue();
        }
    }
}

Array<ProviderIndicationCountTable::_ProviderIndicationCountTableEntry>
    ProviderIndicationCountTable::_getAllEntries()
{
    PEG_METHOD_ENTER(TRC_INDICATION_SERVICE,
        "ProviderIndicationCountTable::_getAllEntries");

    Array <_ProviderIndicationCountTableEntry> providerIndicationCountEntries;

    //
    // Iterate through the ProviderIndicationCountTable to get all the entries.
    //

    {
        ReadLock lock(_tableLock);
        for (_ProviderIndicationCountTable::Iterator i = _table.start(); i; i++)
        {
            providerIndicationCountEntries.append(i.value());
        }
    }

    PEG_METHOD_EXIT();
    return providerIndicationCountEntries;
}

String ProviderIndicationCountTable::_generateKey(
    const String& providerModuleName,
    const String& providerName)
{
    PEG_METHOD_ENTER(TRC_INDICATION_SERVICE,
        "ProviderIndicationCountTable::_generateKey");

    String providerIndicationCountKey(providerName);
    providerIndicationCountKey.append(providerModuleName);
    providerIndicationCountKey.append(":");

    char buffer[22];
    Uint32 length;
    const char* providerNameSize =
        Uint32ToString(buffer, providerName.size(), length);
    providerIndicationCountKey.append(providerNameSize, length);

    PEG_METHOD_EXIT();
    return providerIndicationCountKey;
}

Array<CIMInstance>
    ProviderIndicationCountTable::enumerateProviderIndicationDataInstances()
{
    PEG_METHOD_ENTER(TRC_INDICATION_SERVICE,
        "ProviderIndicationCountTable::"
            "enumerateProviderIndicationDataInstances");

    Array<CIMInstance> instances;

    //
    // get entire provider indication count table entries
    //
    Array<_ProviderIndicationCountTableEntry> indicationCountEntries =
        _getAllEntries();

    for (Uint32 i = 0; i < indicationCountEntries.size(); i++)
    {
        CIMInstance providerIndDataInstance(PEGASUS_CLASSNAME_PROVIDERINDDATA);
        providerIndDataInstance.addProperty(CIMProperty(
            CIMName("ProviderModuleName"),
            indicationCountEntries[i].providerModuleName));
        providerIndDataInstance.addProperty(CIMProperty(
            CIMName("ProviderName"),
            indicationCountEntries[i].providerName));
        providerIndDataInstance.addProperty(CIMProperty(
            CIMName("IndicationCount"),
            indicationCountEntries[i].indicationCount));
        providerIndDataInstance.addProperty(CIMProperty(
            CIMName("OrphanIndicationCount"),
            indicationCountEntries[i].orphanIndicationCount));

        instances.append(providerIndDataInstance);
    }

    PEG_METHOD_EXIT();
    return instances;
}

PEGASUS_NAMESPACE_END
