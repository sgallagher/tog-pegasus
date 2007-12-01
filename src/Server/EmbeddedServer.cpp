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

//==============================================================================
//
// Local definitions:
//
//==============================================================================

extern void* (*lookupSymbolCallback)(
    const char* path, const char* name, void* data);

extern void* lookupSymbolData;

struct Symbol
{
    String path;
    String name;
    void* address;
    Symbol* next;
};

struct EmbeddedServerRep
{
    MemoryResidentRepository* mrr;

    static void initializeCallback(MemoryResidentRepository* mrr, void* data_)
    {
        EmbeddedServer* es = (EmbeddedServer*)data_;
        EmbeddedServerRep* rep = (EmbeddedServerRep*)es->_opaque;
        rep->mrr = mrr;
        es->initialize();
        rep->mrr = 0;
    }

    Symbol* symbols;
};

static void _logCallback(
    int type,
    const char* system,
    int level,
    const char* message,
    void* data)
{
    EmbeddedServer* es = (EmbeddedServer*)data;
    es->putLog(type, system, level, message);
}

static void _saveCallback(const Buffer& buffer, void* data_)
{
    EmbeddedServer* es = (EmbeddedServer*)data_;

    Array<Uint8> data((const Uint8*)buffer.getData(), buffer.size());
    es->saveRepository(data);
}

static void _loadCallback(Buffer& buffer, void* data_)
{
    EmbeddedServer* es = (EmbeddedServer*)data_;

    buffer.clear();
    Array<Uint8> data;
    es->loadRepository(data);

    if (data.size())
        buffer.append((const char*)data.getData(), data.size());
}

static void* _lookupSymbolCallback(
    const char* path, 
    const char* name,
    void* data)
{
    EmbeddedServerRep* rep = (EmbeddedServerRep*)data;

    for (Symbol* p = rep->symbols; p; p = p->next)
    {
        if (p->path == path && p->name == name)
            return p->address;
    }

    // Not found!
    return 0;
}

//==============================================================================
//
// class EmbeddedServer
//
//==============================================================================

const Uint32 EmbeddedServer::INSTANCE_PROVIDER_TYPE = 1;
const Uint32 EmbeddedServer::ASSOCIATION_PROVIDER_TYPE = 2;
const Uint32 EmbeddedServer::INDICATION_PROVIDER_TYPE = 4;
const Uint32 EmbeddedServer::METHOD_PROVIDER_TYPE = 8;
const Uint32 EmbeddedServer::INSTANCE_QUERY_PROVIDER_TYPE = 16;

EmbeddedServer::EmbeddedServer()
{
    // Initialize representation object:

    EmbeddedServerRep* rep = (EmbeddedServerRep*)_opaque;
    memset(rep, 0, sizeof(EmbeddedServerRep));

    // Install the callbacks:

    Logger::installLogCallback(_logCallback, this);

    MemoryResidentRepository::installSaveCallback(_saveCallback, this);

    MemoryResidentRepository::installLoadCallback(_loadCallback, this);

    MemoryResidentRepository::installInitializeCallback(
        EmbeddedServerRep::initializeCallback, this);

    lookupSymbolCallback = _lookupSymbolCallback;
    lookupSymbolData = rep;
}

EmbeddedServer::~EmbeddedServer()
{
}

void EmbeddedServer::initialize()
{
}

#if 0
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
#endif

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

static Boolean _providerModuleInstanceExists(
    MemoryResidentRepository* mrr, 
    const String& name)
{
    try
    {
        CIMObjectPath cop;
        Array<CIMKeyBinding> bindings;
        bindings.append(CIMKeyBinding("Name", name, CIMKeyBinding::STRING));
        cop.setClassName("PG_ProviderModule");
        cop.setKeyBindings(bindings);

        CIMInstance ci = mrr->getInstance(true, 
            "root/PG_InterOp", cop, true, false, false, CIMPropertyList());

        return true;
    }
    catch (...)
    {
        return false;
    }

    return false;
}

static Boolean _providerInstanceExists(
    MemoryResidentRepository* mrr, 
    const String& providerModuleName,
    const String& name)
{
    try
    {
        CIMObjectPath cop;
        Array<CIMKeyBinding> bindings;
        bindings.append(CIMKeyBinding("Name", name, CIMKeyBinding::STRING));
        bindings.append(CIMKeyBinding(
            "ProviderModuleName", providerModuleName, CIMKeyBinding::STRING));
        cop.setClassName("PG_Provider");
        cop.setKeyBindings(bindings);

        CIMInstance ci = mrr->getInstance(true, 
            "root/PG_InterOp", cop, true, false, false, CIMPropertyList());

        return true;
    }
    catch (...)
    {
        return false;
    }

    return false;
}

Boolean EmbeddedServer::registerProviderModule(
    const String& moduleName,
    const String& location,
    ProviderInterface providerInterface)
{
    EmbeddedServerRep* rep = (EmbeddedServerRep*)_opaque;

    // Reject if repository is null at this point.

    if (!rep->mrr)
        return false;

    //
    // Create instance of PG_ProviderModule:
    //
    //     instance of PG_ProviderModule
    //     {
    //         Name = "...";
    //         Vendor = "OpenPegasus";
    //         Version = "2.5.0";
    //         InterfaceType = "C++Default";
    //         InterfaceVersion = "2.5.0";
    //         Location = "cimplePerson";
    //     };
    //

    CIMInstance ci("PG_ProviderModule");
    ci.addProperty(CIMProperty("Name", moduleName));
    ci.addProperty(CIMProperty("Vendor", String("OpenPegasus")));

    String version;
    String interfaceType;
    String interfaceVersion;

    if (providerInterface == PEGASUS_PROVIDER_INTERFACE)
    {
        version = "2.6.0";
        interfaceType = "C++Default";
        interfaceVersion = "2.6.0";
    }
    else
    {
        version = "2.0.0";
        interfaceType = "CMPI";
        interfaceVersion = "2.0.0";
    }
        
    ci.addProperty(CIMProperty("Version", version));
    ci.addProperty(CIMProperty("InterfaceType", interfaceType));
    ci.addProperty(CIMProperty("InterfaceVersion", interfaceVersion));
    ci.addProperty(CIMProperty("Location", location));

    try
    {
        CIMObjectPath cop = rep->mrr->createInstance(
            false, "root/PG_InterOp", ci, ContentLanguageList());
    }
    catch (Exception& e)
    {
        return false;
    }
    catch (...)
    {
        return false;
    }

    return true;
}

Boolean EmbeddedServer::registerProvider(
    const String& moduleName,
    const String& providerName)
{
    EmbeddedServerRep* rep = (EmbeddedServerRep*)_opaque;

    // Reject if repository is null at this point.

    if (!rep->mrr)
        return false;

    // Reject if provider module does not exist:

    if (!_providerModuleInstanceExists(rep->mrr, moduleName))
        return false;

    //
    // Create instance of PG_ProviderModule:
    //
    //     instance of PG_Provider
    //     {
    //         Name = "Person_Provider";
    //         ProviderModuleName = "Person_Module";
    //     };
    //

    CIMInstance ci("PG_Provider");
    ci.addProperty(CIMProperty("Name", providerName));
    ci.addProperty(CIMProperty("ProviderModuleName", moduleName));

    try
    {
        CIMObjectPath cop = rep->mrr->createInstance(
            false, "root/PG_InterOp", ci, ContentLanguageList());
    }
    catch (Exception& e)
    {
        return false;
    }
    catch (...)
    {
        return false;
    }

    return true;
}

Boolean EmbeddedServer::registerProviderCapabilities(
    const String& moduleName,
    const String& providerName,
    const String& capabilityId,
    const CIMName& className,
    const Array<CIMNamespaceName>& nameSpaces,
    Uint32 providerTypes)
{
    EmbeddedServerRep* rep = (EmbeddedServerRep*)_opaque;

    // Reject if repository is null at this point.

    if (!rep->mrr)
        return false;

    // Reject if provider module instance does not exist:

    if (!_providerModuleInstanceExists(rep->mrr, moduleName))
        return false;

    // Reject if provider instance does not exist:

    if (!_providerInstanceExists(rep->mrr, moduleName, providerName))
        return false;

    // Validate the namespaces:
    {
        if (nameSpaces.size() == 0)
            return false;

        Array<CIMNamespaceName> tmp;

        try
        {
            tmp = rep->mrr->enumerateNameSpaces(false);
        }
        catch (...)
        {
            return false;
        }

        for (Uint32 i = 0; i < nameSpaces.size(); i++)
        {
            Boolean found = false;

            for (Uint32 j = 0; j < tmp.size(); j++)
            {
                if (nameSpaces[i] == tmp[j])
                {
                    found = true;
                    break;
                }
            }

            if (!found)
                return false;
        }
    }

    //
    // Create instance of PG_ProviderCapabilities:
    //
    //     instance of PG_ProviderCapabilities
    //     {
    //         CapabilityID = "Person";
    //         ProviderModuleName = "Person_Module";
    //         ProviderName = "Person_Provider";
    //         ClassName = "Person";
    //         Namespaces = {"root/cimv2"};
    //         ProviderType = {2};
    //         supportedProperties = NULL;
    //         supportedMethods = NULL;
    //     };
    //
    CIMInstance ci("PG_ProviderCapabilities");

    // PG_ProviderCapabilities.CapabilityID:
    ci.addProperty(CIMProperty("CapabilityID", capabilityId));

    // PG_ProviderCapabilities.ProviderModuleName:
    ci.addProperty(CIMProperty("ProviderModuleName", moduleName));

    // PG_ProviderCapabilities.ProviderName:
    ci.addProperty(CIMProperty("ProviderName", providerName));

    // PG_ProviderCapabilities.ClassName:
    ci.addProperty(CIMProperty("ClassName", className.getString()));

    // PG_ProviderCapabilities.NameSpaces:
    {
        Array<String> tmp;

        for (Uint32 i = 0; i < nameSpaces.size(); i++)
            tmp.append(nameSpaces[i].getString());

        ci.addProperty(CIMProperty("Namespaces", tmp));
    }

    // PG_ProviderCapabilities.supportedProperties:
    ci.addProperty(
        CIMProperty("supportedProperties", CIMValue(CIMTYPE_STRING, true)));

    // PG_ProviderCapabilities.supportedMethods:
    ci.addProperty(
        CIMProperty("supportedMethods", CIMValue(CIMTYPE_STRING, true)));

    // PG_ProviderCapabilities.Namespaces:
    {
        Array<Uint16> tmp;

        if (providerTypes & EmbeddedServer::INSTANCE_PROVIDER_TYPE)
            tmp.append(2);

        if (providerTypes & EmbeddedServer::ASSOCIATION_PROVIDER_TYPE)
            tmp.append(3);

        if (providerTypes & EmbeddedServer::INDICATION_PROVIDER_TYPE)
            tmp.append(4);

        if (providerTypes & EmbeddedServer::METHOD_PROVIDER_TYPE)
            tmp.append(5);

        if (providerTypes & EmbeddedServer::INSTANCE_QUERY_PROVIDER_TYPE)
            tmp.append(7);

        ci.addProperty(CIMProperty("ProviderType", tmp));
    }

    // Create the instance:
    try
    {
        CIMObjectPath cop = rep->mrr->createInstance(
            false, "root/PG_InterOp", ci, ContentLanguageList());
    }
    catch (Exception& e)
    {
        return false;
    }
    catch (...)
    {
        return false;
    }

    return true;
}

Boolean EmbeddedServer::registerSingletonProvider(
    const Array<CIMNamespaceName>& nameSpaces,
    const CIMName& className,
    ProviderInterface providerInterface,
    Uint32 providerTypes)
{
    // Register PG_ProviderModule:

    String moduleName = className.getString() + "_Module";
    String location = moduleName;

    if (!registerProviderModule(moduleName, location, providerInterface))
    {
        return false;
    }

    // Register PG_Provider:

    String providerName = className.getString() + "_Provider";

    if (!registerProvider(moduleName, providerName))
    {
        return false;
    }

    // Register PG_ProviderCapabilities:


    String capabilityId = className.getString();

    if (!registerProviderCapabilities(moduleName, providerName, capabilityId,
        className, nameSpaces, providerTypes))
    {
        return false;
    }

    return true;
}

static Boolean _addSymbol(
    EmbeddedServerRep* rep,
    const String& path,
    const String& name,
    void* address)
{

    if (_lookupSymbolCallback(path.getCString(), name.getCString(), rep))
        return false;

    Symbol* symbol = new Symbol;
    symbol->path = path;
    symbol->name = name;
    symbol->address = address;

    symbol->next = rep->symbols;
    rep->symbols = symbol;
    
    return true;
}

Boolean EmbeddedServer::registerPegasusProviderEntryPoint(
    const String& location,
    class CIMProvider* (*entryPoint)(const String&))
{
    EmbeddedServerRep* rep = (EmbeddedServerRep*)_opaque;

    return _addSymbol(
        rep, location, "PegasusCreateProvider", (void*)entryPoint);
}

#ifdef PEGASUS_ENABLE_CMPI_PROVIDER_MANAGER

Boolean EmbeddedServer::registerCMPIProviderEntryPoint(
    const String& location,
    const String& providerName,
    CreateInstanceMIEntryPoint entryPoint)
{
    EmbeddedServerRep* rep = (EmbeddedServerRep*)_opaque;
    String name = providerName + "_Create_InstanceMI";
    return _addSymbol(rep, location, name, (void*)entryPoint);
}

Boolean EmbeddedServer::registerCMPIProviderEntryPoint(
    const String& location,
    const String& providerName,
    CreateAssociationMIEntryPoint entryPoint)
{
    EmbeddedServerRep* rep = (EmbeddedServerRep*)_opaque;
    String name = providerName + "_Create_AssociationMI";
    return _addSymbol(rep, location, name, (void*)entryPoint);
}

Boolean EmbeddedServer::registerCMPIProviderEntryPoint(
    const String& location,
    const String& providerName,
    CreateMethodMIEntryPoint entryPoint)
{
    EmbeddedServerRep* rep = (EmbeddedServerRep*)_opaque;
    String name = providerName + "_Create_MethodMI";
    return _addSymbol(rep, location, name, (void*)entryPoint);
}

Boolean EmbeddedServer::registerCMPIProviderEntryPoint(
    const String& location,
    const String& providerName,
    CreateIndicationMIEntryPoint entryPoint)
{
    EmbeddedServerRep* rep = (EmbeddedServerRep*)_opaque;
    String name = providerName + "_Create_IndicationMI";
    return _addSymbol(rep, location, name, (void*)entryPoint);
}

Boolean EmbeddedServer::registerCMPIProviderEntryPoint(
    const String& location,
    const String& providerName,
    CreatePropertyMIEntryPoint entryPoint)
{
    EmbeddedServerRep* rep = (EmbeddedServerRep*)_opaque;
    String name = providerName + "_Create_PropertyMI";
    return _addSymbol(rep, location, name, (void*)entryPoint);
}

#endif /* PEGASUS_ENABLE_CMPI_PROVIDER_MANAGER */

PEGASUS_NAMESPACE_END
