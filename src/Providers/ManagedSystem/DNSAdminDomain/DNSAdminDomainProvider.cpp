//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 The Open group, BMC Software, Tivoli Systems, IBM
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
// Author: Paulo F. Borges (pfborges@wowmail.com)
//
// Modified By: 
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Provider/ProviderException.h>    
#include "DNSAdminDomain.h"
#include "DNSAdminDomainProvider.h"

PEGASUS_USING_STD;

/* ==========================================================================
   Miscellaneous Defines
   ========================================================================== */
static const int MAX_KEYS = 2;  // The number of keys for the classe.
static const String CAPTION("DNS Admin Domain");
static const String DESCRIPTION("This is the PG_DNSAdminDomain object");
static const String NAME_FORMAT("IP");
static const String CLASS_HPUX_DNSADMINDOMAIN("PG_DNSAdminDomain");

/* ==========================================================================
   WBEM MOF property names.  These values are returned by the provider as
   the property names.
   ========================================================================== */
static const String PROPERTY_NAME("Name");
static const String PROPERTY_CAPTION("Caption");
static const String PROPERTY_DESCRIPTION("Description");    
static const String PROPERTY_NAMEFORMAT("NameFormat");
static const String PROPERTY_SEARCH_LIST("SearchList");
static const String PROPERTY_ADDRESSES("Addresses");
static const String PROPERTY_CREATION_CLASS_NAME("CreationClassName");


DNSAdminDomainProvider::DNSAdminDomainProvider(void)
{
}

DNSAdminDomainProvider::~DNSAdminDomainProvider(void)
{
}

void 
DNSAdminDomainProvider::getInstance(
                const OperationContext & context,
                const CIMObjectPath & instanceReference,
                const Boolean includeQualifiers,
                const Boolean includeClassOrigin,
                const CIMPropertyList & propertyList,
                InstanceResponseHandler & handler)
{
    DNSAdminDomain dns;
    String className;
    String dname;
    className = instanceReference.getClassName();
    Array<CIMKeyBinding> keys;
    CIMInstance instance;

    //-- make sure we're working on the right class
    className = instanceReference.getClassName();
    if (!String::equalNoCase(className, CLASS_HPUX_DNSADMINDOMAIN))
        throw CIMNotSupportedException("DNSAdminDomainProvider does not support class " + className);

    //-- make sure we're the right instance
    int keyCount;
    String keyName;

    keyCount = MAX_KEYS;
    keys = instanceReference.getKeyBindings();

    if ((unsigned int)keys.size() != (unsigned int)keyCount)
        throw CIMInvalidParameterException("Wrong number of keys");

    // doesn't seem as though this code will handle duplicate keys,
    // but it appears as though the CIMOM strips those out for us.
    // Despite test cases, don't get invoked with 2 keys of the same
    // name.

    if(!dns.getName(dname))
        throw CIMOperationFailedException("DNSProvider "
                  "can't determine name property");
    
    for (unsigned int ii = 0; ii < keys.size(); ii++)
    {
         keyName = keys[ii].getName();

         if (String::equalNoCase(keyName, PROPERTY_CREATION_CLASS_NAME) &&
              (String::equalNoCase(keys[ii].getValue(),
                                     CLASS_HPUX_DNSADMINDOMAIN) ||
             keys[ii].getValue() == ""))
         {
              keyCount--;
         }
         else if (String::equalNoCase(keyName, PROPERTY_NAME) &&
                    String::equalNoCase(keys[ii].getValue(), dname))
         {
              keyCount--;
         }
         else
         {
              throw CIMInvalidParameterException("DNSAdminDomainProvider "
                             " unrecognized key " + keyName);
         }
     }

     if (keyCount)
     {
        throw CIMInvalidParameterException("Wrong keys");
     }
    handler.processing();

    //-- fill 'er up...
    instance = _build_instance(className, 
                               instanceReference.getNameSpace(), 
                               instanceReference.getKeyBindings());
    handler.deliver(instance);
    handler.complete();
}

void 
DNSAdminDomainProvider::enumerateInstances(
                const OperationContext & context,
                const CIMObjectPath & classReference,
                const Boolean includeQualifiers,
                const Boolean includeClassOrigin,
                const CIMPropertyList & propertyList,
                InstanceResponseHandler & handler)
{
    String className;
    CIMInstance instance;
    CIMObjectPath newref;
   
    className = classReference.getClassName();

    // only return instances when enumerate on our subclass, CIMOM 
    // will call us as natural part of recursing through subtree on 
    // enumerate - if we return instances on enumerate of our superclass, 
    // there would be dups
    if (String::equalNoCase(className, CLASS_HPUX_DNSADMINDOMAIN))
    {
        handler.processing();
        newref = _fill_reference(classReference.getNameSpace(), className);
        instance = _build_instance(className, 
                                   classReference.getNameSpace(), 
                                   classReference.getKeyBindings());
        instance.setPath(newref);
        handler.deliver(instance);
        handler.complete();
    }
    else
    {
        throw CIMNotSupportedException("DNSAdminDomainProvider "
                "does not support class " + className);
    }
}

void 
DNSAdminDomainProvider::enumerateInstanceNames(
                const OperationContext & context,
                const CIMObjectPath & classReference,
                ObjectPathResponseHandler & handler)
{
    CIMObjectPath newref;
    String className;

    // only return instances when enumerate on our subclass, CIMOM
    // will call us as natural part of recursing through subtree on
    // enumerate - if we return instances on enumerate of our superclass,
    // there would be dups
    className = classReference.getClassName();
    if (!String::equalNoCase(className, CLASS_HPUX_DNSADMINDOMAIN))
    {
        throw CIMNotSupportedException("DNSAdminDomainProvider "
                           "does not support class " + className);
    }

    handler.processing();
    // in terms of the class we use, want to set to what was requested
    newref = _fill_reference(classReference.getNameSpace(), className);
    handler.deliver(newref);
    handler.complete();
}

void 
DNSAdminDomainProvider::modifyInstance(
                const OperationContext & context,
                const CIMObjectPath & instanceReference,
                const CIMInstance & instanceObject,
                const Boolean includeQualifiers,
                const CIMPropertyList & propertyList,
                ResponseHandler & handler)
{
    throw CIMNotSupportedException("DNSAdminDomainProvider "
                       "does not support modifyInstance");
}

void 
DNSAdminDomainProvider::createInstance(
                const OperationContext & context,
                const CIMObjectPath & instanceReference,
                const CIMInstance & instanceObject,
                ObjectPathResponseHandler & handler)
{
    throw CIMNotSupportedException("DNSAdminDomainProvider "
                       "does not support createInstance");
}

void 
DNSAdminDomainProvider::deleteInstance(
                const OperationContext & context,
                const CIMObjectPath & instanceReference,
                ResponseHandler & handler)
{
    throw CIMNotSupportedException("DNSAdminDomainProvider "
                       "does not support deleteInstance");
}

void 
DNSAdminDomainProvider::initialize(CIMOMHandle & cimom)
{
}

void 
DNSAdminDomainProvider::terminate(void)
{
    delete this;
}

/***********************************************************************
 Create properties to provider
***********************************************************************/

CIMInstance
DNSAdminDomainProvider::_build_instance(const String & className,
                                        const String & nameSpace,
                                        const Array<CIMKeyBinding> keys)
{
    CIMInstance instance(className);
    String dname;
    String hostName;
    Array<String> srcl;
    Array<String> addr;
    DNSAdminDomain dns;

    if(!dns.getLocalHostName(hostName))
        hostName.assign("localhost");

    instance.setPath(CIMObjectPath(hostName,
                                   nameSpace,
                                   className,    
                                   keys));


    instance.addProperty(CIMProperty(PROPERTY_CREATION_CLASS_NAME,
                                     CLASS_HPUX_DNSADMINDOMAIN));

    if(!dns.getName(dname)) 
        throw CIMOperationFailedException("DNSProvider "
                  "can't determine name property");

    instance.addProperty(CIMProperty(PROPERTY_NAME, dname));

    instance.addProperty(CIMProperty(PROPERTY_CAPTION, 
                                     CAPTION));

    instance.addProperty(CIMProperty(PROPERTY_DESCRIPTION, 
                                     DESCRIPTION));

    instance.addProperty(CIMProperty(PROPERTY_NAMEFORMAT, 
                                     NAME_FORMAT));

    if(!dns.getSearchList(srcl)) 
        throw CIMOperationFailedException("DNSProvider "
                  "can't determine search list property");

    instance.addProperty(CIMProperty(PROPERTY_SEARCH_LIST, 
                                     srcl));

    if(!dns.getAddresses(addr)) 
        throw CIMOperationFailedException("DNSProvider "
                  "can't determine addresses list property");

    instance.addProperty(CIMProperty(PROPERTY_ADDRESSES, 
                                     addr));

    return instance;
}

/***********************************************************************
    Return CIMObjectPath instance of class valid keys
***********************************************************************/

CIMObjectPath
DNSAdminDomainProvider::_fill_reference(const String &nameSpace,
                                        const String &className)
{
    Array<CIMKeyBinding> keys;
    DNSAdminDomain dns;
    String csName;

    keys.append(CIMKeyBinding(PROPERTY_CREATION_CLASS_NAME, CLASS_HPUX_DNSADMINDOMAIN,
                           CIMKeyBinding::STRING));

    if(!dns.getName(csName)) 
        throw CIMOperationFailedException("DNSProvider "
                  "can't determine name property");
        
       keys.append(CIMKeyBinding(PROPERTY_NAME, csName, CIMKeyBinding::STRING));

    return CIMObjectPath("localhost", nameSpace, className, keys);
}
