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

#include "EmbeddedServer.h"
#include <Pegasus/Common/Logger.h>
#include <Pegasus/Server/ProviderTable.h>
#include <Pegasus/Repository/MemoryResidentRepository.h>

PEGASUS_NAMESPACE_BEGIN

static void _logCallback(
    int type,
    const char* system,
    int level,
    const char* message,
    void* clientData)
{
    EmbeddedServer* es = (EmbeddedServer*)clientData;
    es->putLog(type, system, level, message);
}

static void _saveCallback(const Buffer& buffer, void* clientData)
{
    EmbeddedServer* es = (EmbeddedServer*)clientData;

    Array<Uint8> data((const Uint8*)buffer.getData(), buffer.size());
    es->saveRepository(data);
}

static void _loadCallback(Buffer& buffer, void* clientData)
{
    EmbeddedServer* es = (EmbeddedServer*)clientData;

    buffer.clear();
    Array<Uint8> data;
    es->loadRepository(data);

    if (data.size())
        buffer.append((const char*)data.getData(), data.size());
}

EmbeddedServer::EmbeddedServer()
{
    // Install the callbacks:
    Logger::installLogCallback(_logCallback, this);
    MemoryResidentRepository::installSaveCallback(_saveCallback, this);
    MemoryResidentRepository::installLoadCallback(_loadCallback, this);
}

EmbeddedServer::~EmbeddedServer()
{
}

Boolean EmbeddedServer::addProvider(
    const String& moduleName,
    const String& providerName,
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    CIMProvider* (*createProvider)(const String& providerName))
{
    if (providerTableSize == MAX_PROVIDER_TABLE_SIZE)
        return false;

    ProviderTableEntry entry;
    entry.moduleName = strdup(moduleName.getCString());
    entry.providerName = strdup(providerName.getCString());
    entry.nameSpace = strdup(nameSpace.getString().getCString());
    entry.className = strdup(className.getString().getCString());
    entry.createProvider = createProvider;

    providerTable[providerTableSize++] = entry;

    return true;
}

Boolean EmbeddedServer::addNameSpace(const SchemaNameSpace* nameSpace)
{
    return MemoryResidentRepository::addNameSpace(nameSpace);
}

void EmbeddedServer::loadRepository(Array<Uint8>& data)
{
    // No implementation!
}

void EmbeddedServer::saveRepository(const Array<Uint8>& data)
{
    // No implementation!
}

void EmbeddedServer::putLog(
    int type,
    const char* system,
    int level,
    const char* message)
{
    // No implementation!
}

extern "C" int PegasusServerMain(int argc, char** argv);

Boolean EmbeddedServer::run(int argc, char** argv)
{
    try
    {
        PegasusServerMain(argc, argv);
        return true;
    }
    catch (...)
    {
        return false;
    }

    return true;
}

PEGASUS_NAMESPACE_END
