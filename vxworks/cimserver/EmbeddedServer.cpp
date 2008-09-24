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
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Logger.h>
#include <Pegasus/Server/ProviderTable.h>
#include <Pegasus/Repository/MemoryResidentRepository.h>
#include <Pegasus/Common/PegasusVersion.h>
#include <slp/slp_agent/peg_slp_agent.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#define STATIC_MODULE "Static_Module"

PEGASUS_NAMESPACE_BEGIN

//==============================================================================
//
// Authentication
//
//==============================================================================

extern "C" int (*pegasusAuthCallback)(
    const char* user, const char* pass, void* data);

extern "C" void* pegasusAuthData;

static int _authCallback(const char* user, const char* pass, void* data)
{
    EmbeddedServer* es = (EmbeddedServer*)data;

    if (es)
        return es->authenticate(user, pass) ? 0 : -1;
    else
        return -1;
}

//==============================================================================
//
// SLP
//
//==============================================================================

static slp_service_agent* _slpAgent = 0;

#define SLP_SERVICE_HI_DESCRIPTION \
    PEGASUS_CIMOM_GENERIC_NAME " " \
    PEGASUS_PRODUCT_NAME " " \
    "Version " \
    PEGASUS_PRODUCT_VERSION " " \
    "Development"

#define SLP_NAMESPACE "root/cimv2,root/PG_Interop,root/PG_Internal"

#define SLP_REGISTERED_PROFILES_SUPPORTED "none"

// %s : http://192.168.1.190:5988
const char ATTRS[] =
    "(template-url-syntax=%s),\n"
    "(service-id=PG:1215719580200-127-0-0-1),\n"
    "(service-hi-name=Pegasus),\n"
    "(service-hi-description=" SLP_SERVICE_HI_DESCRIPTION "),\n"
    "(template-type=wbem),\n"
    "(template-version=1.0),\n"
    "(template-description=This template describes the attributes used for "
      "advertising Pegasus CIM Servers.),\n"
    "(InteropSchemaNamespace=root/PG_InterOp),\n"
    "(FunctionalProfilesSupported=Basic Read,Basic Write,Schema Manipulation,"
      "Instance Manipulation,Association Traversal,Qualifier Declaration,"
      "Indications),\n"
    "(MultipleOperationsSupported=FALSE),\n"
    "(AuthenticationMechanismsSupported=Basic),\n"
    "(AuthenticationMechanismDescriptions=Basic),\n"
    "(CommunicationMechanism=CIM-XML),\n"
    "(ProtocolVersion=1.0),\n"
    "(Namespace=" SLP_NAMESPACE "),\n"
    "(RegisteredProfilesSupported=" SLP_REGISTERED_PROFILES_SUPPORTED ")\n";

static bool _getIPAddress(char ipaddr[256])
{
    int sock;

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        return false;

    ifconf ifc;
    char buf[64 * sizeof(ifreq)];

    memset(&ifc, 0, sizeof(ifc));
    memset(&buf, 0, sizeof(buf));
    ifc.ifc_buf = buf;
    ifc.ifc_len = sizeof(buf);

    if (ioctl(sock, SIOCGIFCONF, (int)&ifc) < 0)
    {
        close(sock);
        return false;
    }

    for (char* p = buf; p < buf + ifc.ifc_len; )
    {
        ifreq* ifr = (struct ifreq*)p;

        // Move pointer to next ifreq element.

        size_t len;

        if (sizeof(sockaddr) > ifr->ifr_addr.sa_len)
            len = sizeof(sockaddr);
        else
            len = ifr->ifr_addr.sa_len;

        p += sizeof(ifr->ifr_name) + len;

        // Skip non-internet addresses:

        if (ifr->ifr_addr.sa_family != AF_INET)
            continue;


        // Append address to array and terminate with INADDR_ANY.

        sockaddr_in* addr = ((sockaddr_in*)&ifr->ifr_addr);

        // Skip loopback addresses:

        if (addr->sin_addr.s_addr == inet_addr("127.0.0.1"))
            continue;

        if (ifr->ifr_name[0] == 'l' && ifr->ifr_name[1] == 'o')
            continue;

        // We found one.

        strcpy(ipaddr, inet_ntoa(addr->sin_addr));
        return true;
    }

    close(sock);

    // Not found!
    return false;
}

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
        if (p->name == name)
            return p->address;

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

extern "C" class CIMHandler* PegasusCreateHandler(const String& handlerName);

EmbeddedServer::EmbeddedServer() : _createdStaticProviderModule(false)
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

    pegasusAuthCallback = _authCallback;
    pegasusAuthData = this;

    _addSymbol(rep, "CIMXMLHandler", "PegasusCreateHandler", 
        (void*)PegasusCreateHandler);
}

EmbeddedServer::~EmbeddedServer()
{
}

void EmbeddedServer::initialize()
{
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

Boolean EmbeddedServer::_create_PG_ProviderModule(
    const String& moduleName,
    const String& location)
{
    EmbeddedServerRep* rep = (EmbeddedServerRep*)_opaque;

    // Reject if repository is null at this point.

    if (!rep->mrr)
    {
printf("ZZZ1\n");
        return false;
    }

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

    ci.addProperty(CIMProperty("Version", String("2.6.0")));
    ci.addProperty(CIMProperty("InterfaceType", String("C++Default")));
    ci.addProperty(CIMProperty("InterfaceVersion", String("2.6.0")));
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

Boolean EmbeddedServer::_create_PG_Provider(
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

Boolean EmbeddedServer::_create_PG_ProviderCapabilities(
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

Boolean EmbeddedServer::registerProvider(
    const Array<CIMNamespaceName>& nameSpaces,
    const CIMName& className,
    Uint32 providerTypes)
{
    // Register PG_ProviderModule:

    if (!_createdStaticProviderModule)
    {
        if (!_create_PG_ProviderModule(STATIC_MODULE, STATIC_MODULE))
        {
            return false;
        }

        _createdStaticProviderModule = true;
    }

    // Register PG_Provider:

    String providerName = className.getString() + "_Provider";

    if (!_create_PG_Provider(STATIC_MODULE, providerName))
    {
        return false;
    }

    // Register PG_ProviderCapabilities:

    String capabilityId = className.getString();

    if (!_create_PG_ProviderCapabilities(
        STATIC_MODULE, providerName, capabilityId,
        className, nameSpaces, providerTypes))
    {
        return false;
    }

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

Boolean EmbeddedServer::registerPegasusCreateProviderEntryPoint(
    PegasusCreateProviderEntryPoint entryPoint)
{
    return registerPegasusProviderEntryPoint(STATIC_MODULE, entryPoint);
}

bool EmbeddedServer::authenticate(const char* user, const char* pass)
{
    return true;
}

bool EmbeddedServer::registerSLPService(unsigned short port, bool https)
{
    char ipaddr[256];
    _slpAgent = new slp_service_agent;

    if (!_getIPAddress(ipaddr))
        strcpy(ipaddr, "0.0.0.0");

    try
    {
        _slpAgent->start_listener();
    }
    catch (Exception& e)
    {
        String msg = e.getMessage();
        CString cstr(msg.getCString());
        return false;
    }
    catch (...)
    {
        return false;
    }

    // URL (e.g., http://192.168.1.190:5988)
    char url[256];
    sprintf(url, "%s://%s:%u", (https ? "https" : "http"), ipaddr, port);

    // Service type (e.g., "service:wbem:http")
    char type[1024];
    sprintf(type, "service:wbem:%s", (https ? "https" : "http"));

    // Service name (e.g., service:wbem:http://192.168.1.190:5988)
    char svc[1024];
    sprintf(svc, "service:wbem:%s", url);

    // Attributes:
    char attrs[sizeof(ATTRS) + 1024];
    sprintf(attrs, ATTRS, url);

    Uint32 rc = _slpAgent->test_registration(svc, attrs, type, "DEFAULT");

    if (rc != 0)
        return false;

    Boolean flag = _slpAgent->srv_register(svc, attrs, type, "DEFAULT", 0xFFFF);

    if (!flag)
        return false;

    return true;
}

PEGASUS_NAMESPACE_END
