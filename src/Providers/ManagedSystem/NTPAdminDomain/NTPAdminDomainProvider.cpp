//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software, Hewlett-Packard Company, IBM,
// The Open Group, Tivoli Systems
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
//
// Modified By: Jair Francisco T. dos Santos (t.dos.santos.francisco@non.hp.com)
//==============================================================================
// Based on DNSAdminDomainProvider.cpp file
//%////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// INCLUDES
//------------------------------------------------------------------------------
//Pegasus includes
#include <Pegasus/Common/Config.h>
#include <Pegasus/Provider/ProviderException.h>

#include "NTPAdminDomain.h"
#include "NTPAdminDomainProvider.h"

//------------------------------------------------------------------------------
PEGASUS_USING_STD;

//------------------------------------------------------------------------------
// GLOBAL VARIABLES
//------------------------------------------------------------------------------
//pointer to a instrumentation object
NTPAdminDomain *ntp;

//==============================================================================
//
// Class [NTPAdminDomainProvider] methods
//
//==============================================================================

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
NTPAdminDomainProvider::NTPAdminDomainProvider(void)
{
}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
NTPAdminDomainProvider::~NTPAdminDomainProvider(void)
{
}

//------------------------------------------------------------------------------
// getInstance
//------------------------------------------------------------------------------
void
NTPAdminDomainProvider::getInstance(const OperationContext & context,
                                   const CIMObjectPath & ref,
                                   const Boolean includeQualifiers,
                                   const Boolean includeClassOrigin,
                                   const CIMPropertyList & propertyList,
                                   InstanceResponseHandler & handler)
{
    Array<CIMKeyBinding> keys;
    CIMInstance instance;
    String className;
    
    //-- make sure we're working on the right class
    className = ref.getClassName();

    if (!String::equalNoCase(className, CLASS_NAME))
        throw CIMNotSupportedException("NTPAdminDomainProvider does not support class " + className);
    
    ntp = new NTPAdminDomain();

    //-- make sure we're the right instance
    int keyCount;
    String keyName;
    String keyValue;
    String nName;
    
    keyCount = MAX_KEYS;
    keys = ref.getKeyBindings();

    if ((unsigned int)keys.size() != (unsigned int)keyCount)
        throw CIMInvalidParameterException("Wrong number of keys");
    
    // Retrieve name property value
    if(!ntp->getName(nName))
        throw CIMObjectNotFoundException("NTPAdminDomainProvider "
            "can't create PG_NTPAdminDomain instance");

    for (unsigned int ii = 0; ii < keys.size(); ii++) {
         keyName = keys[ii].getName();
         keyValue = keys[ii].getValue();

        if (String::equalNoCase(keyName, PROPERTY_CREATION_CLASS_NAME) &&
            (String::equalNoCase(keyValue, CLASS_NAME) || 
             keyValue.size() == 0))
            keyCount--;
        else if (String::equalNoCase(keyName, PROPERTY_NAME) &&
                 String::equalNoCase(keyValue, nName)) 
            keyCount--;
    }

    if (keyCount)
        throw CIMInvalidParameterException("Wrong keys");

    handler.processing();
    instance = _build_instance(className, ref.getNameSpace(), keys);
    handler.deliver(instance);
    handler.complete();
    delete ntp;
    return;
}

//------------------------------------------------------------------------------
// enumerateInstances
//------------------------------------------------------------------------------
void
NTPAdminDomainProvider::enumerateInstances(const OperationContext & context,
                                         const CIMObjectPath & ref,
                                         const Boolean includeQualifiers,
                                         const Boolean includeClassOrigin,
                                         const CIMPropertyList & propertyList,
                                         InstanceResponseHandler & handler)
{
    String className;
    CIMInstance instance;
    CIMObjectPath newref;
    
    className = ref.getClassName();

    ntp = new NTPAdminDomain();
    
    // only return instances when enumerate on our subclass, CIMOM
    // will call us as natural part of recursing through subtree on
    // enumerate - if we return instances on enumerate of our superclass,
    // there would be dups
    if (!String::equalNoCase(className, CLASS_NAME))
        throw CIMNotSupportedException("NTPAdminDomainProvider does not support class " + className);

    handler.processing();
    newref = _fill_reference(ref.getNameSpace(), className);
    instance = _build_instance(className, ref.getNameSpace(), ref.getKeyBindings());
    instance.setPath(newref);
    handler.deliver(instance);
    handler.complete();
    delete ntp;
    return;
}

//------------------------------------------------------------------------------
// enumerateInstanceNames
//------------------------------------------------------------------------------
void
NTPAdminDomainProvider::enumerateInstanceNames(const OperationContext & context,
                                              const CIMObjectPath & ref,
                                             ObjectPathResponseHandler & handler)
{
    CIMObjectPath newref;
    String className;

    // only return instances when enumerate on our subclass, CIMOM
    // will call us as natural part of recursing through subtree on
    // enumerate - if we return instances on enumerate of our superclass,
    // there would be dups

    className = ref.getClassName();

    ntp = new NTPAdminDomain();
    
    if (!String::equalNoCase(className, CLASS_NAME))
        throw CIMNotSupportedException("NTPAdminDomainProvider does not support class " + className);

    handler.processing();
    newref = _fill_reference(ref.getNameSpace(), className);
    handler.deliver(newref);
    handler.complete();
    delete ntp;
    return;
}

//------------------------------------------------------------------------------
// modifyInstance
//------------------------------------------------------------------------------
void
NTPAdminDomainProvider::modifyInstance(const OperationContext & context,
                                     const CIMObjectPath & ref,
                                     const CIMInstance & obj,
                                     const Boolean includeQualifiers,
                                     const CIMPropertyList & propertyList,
                                     ResponseHandler & handler)
{
    throw CIMNotSupportedException("NTPAdminDomainProvider "
                       "does not support modifyInstance");
}

//------------------------------------------------------------------------------
// createInstance
//------------------------------------------------------------------------------
void
NTPAdminDomainProvider::createInstance(const OperationContext & context,
                                     const CIMObjectPath & ref,
                                     const CIMInstance & obj,
                                     ObjectPathResponseHandler & handler)
{
    throw CIMNotSupportedException("NTPAdminDomainProvider "
                       "does not support createInstance");
}

//------------------------------------------------------------------------------
// deleteInstance
//------------------------------------------------------------------------------
void
NTPAdminDomainProvider::deleteInstance(const OperationContext & context,
                                     const CIMObjectPath & ref,
                                     ResponseHandler & handler)
{
    throw CIMNotSupportedException("NTPAdminDomainProvider "
                       "does not support deleteInstance");
}

//------------------------------------------------------------------------------
// invokeMethod
//------------------------------------------------------------------------------
void
NTPAdminDomainProvider::invokeMethod (const OperationContext& context,
                                    const CIMObjectPath& objectReference,
                                    const CIMName& methodName,
                                    const Array<CIMParamValue>& inParameters,
                                    MethodResultResponseHandler& handler)
{
    throw CIMNotSupportedException("NTPAdminDomainProvider "
                       "does not support invokeMethod");
}

//------------------------------------------------------------------------------
// initialize
//------------------------------------------------------------------------------
void 
NTPAdminDomainProvider::initialize(CIMOMHandle& handle)
{
}

//------------------------------------------------------------------------------
// terminate
//------------------------------------------------------------------------------
void
NTPAdminDomainProvider::terminate(void)
{
    delete this;
}

// This method insert new properties into NTPAdminDomain class.
//------------------------------------------------------------------------------
// _build_instance
//------------------------------------------------------------------------------
CIMInstance
NTPAdminDomainProvider::_build_instance(const String & className,
                                        const String & nameSpace,
                                        const Array<CIMKeyBinding> keys) {
    CIMInstance instance(className);
    String strValue;
    String hostName;
    Array<String> lst;

    if(!ntp->getLocalHostName(hostName))
        hostName.assign("localhost");

    instance.setPath(CIMObjectPath(hostName,
                                   nameSpace,
                                   className,    
                                   keys));
    
    if(!ntp->getCreationClassName(strValue)) {
        throw CIMOperationFailedException("NTPAdminDomainProvider "
              "can't determine CreationClassName property");
    }
    
    instance.addProperty(CIMProperty(PROPERTY_CREATION_CLASS_NAME, strValue));

    ntp->getName(strValue);

    instance.addProperty(CIMProperty(PROPERTY_NAME, strValue));

    if(!ntp->getCaption(strValue)) {
        throw CIMOperationFailedException("NTPAdminDomainProvider "
              "can't determine Caption property");
    }

    instance.addProperty(CIMProperty(PROPERTY_CAPTION, strValue));

    if(!ntp->getDescription(strValue)) {
        throw CIMOperationFailedException("NTPAdminDomainProvider "
              "can't determine Description property");
    }

    instance.addProperty(CIMProperty(PROPERTY_DESCRIPTION, strValue));

    if(!ntp->getServerAddress(lst)) {
        throw CIMOperationFailedException("NTPAdminDomainProvider "
              "can't determine ServerAddress property");
    }
    
    instance.addProperty(CIMProperty(PROPERTY_SERVER_ADDRESS, lst));

    if(!ntp->getNameFormat(strValue)) {
        throw CIMOperationFailedException("NTPAdminDomainProvider "
              "can't determine NameFormat property");
    }

    instance.addProperty(CIMProperty(PROPERTY_NAME_FORMAT, strValue));

    return instance;
}

// This method verify the property names. 
//------------------------------------------------------------------------------
// _fill_reference
//------------------------------------------------------------------------------
CIMObjectPath
NTPAdminDomainProvider::_fill_reference(const String &nameSpace,
                                         const String &className)
{
    Array<CIMKeyBinding> keys;
    String hostName;
    String param;
    
    if(!ntp->getCreationClassName(param))
        throw CIMOperationFailedException("NTPAdminDomainProvider "
            "can't determine CreationClassName property");

    keys.append(CIMKeyBinding(PROPERTY_CREATION_CLASS_NAME, param, CIMKeyBinding::STRING));

    ntp->getName(param);

    keys.append(CIMKeyBinding(PROPERTY_NAME, param, CIMKeyBinding::STRING));


    if(!ntp->getLocalHostName(hostName))
        hostName.assign("localhost");
       
    return CIMObjectPath(hostName, nameSpace, className, keys);
}
