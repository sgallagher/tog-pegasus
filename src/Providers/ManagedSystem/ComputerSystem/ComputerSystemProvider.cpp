//%////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2002 BMC Software, Hewlett-Packard Company, IBM,
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
//=============================================================================
//
// Author: Al Stone <ahs3@fc.hp.com>
//         Christopher Neufeld <neufeld@linuxcare.com>
//
// Modified By: David Kennedy       <dkennedy@linuxcare.com>
//              Christopher Neufeld <neufeld@linuxcare.com>
//              Al Stone            <ahs3@fc.hp.com>
//              Mike Glantz         <michael_glantz@hp.com>
//
//%////////////////////////////////////////////////////////////////////////////

// This file should be generic, and while it allows for a
// platform-specific extension to the CIM classes (by
// refering to EXTENDED capabilities), it should never
// explicitly name the extension.

// This provider is registered to support operations at
// several class levels:
//
//  CIM_ComputerSystem
//  CIM_UnitaryComputerSystem
//  <platform>_ComputerSustem
//
// Enumeration operations always return instances from the
// deepest class available (and one day, the localOnly and
// deepEnumeration flags will be respected). All other
// operations take note of the specified class.
//
// Operations currently supported:
//
//  enumerateInstanceNames()
//  enumerateInstances()
//  getInstance()

#include "ComputerSystemProvider.h"
#include "ComputerSystem.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

ComputerSystemProvider::ComputerSystemProvider(void)
{
}

ComputerSystemProvider::~ComputerSystemProvider(void)
{
}

void ComputerSystemProvider::getInstance(
    const OperationContext& context,
    const CIMReference& ref,
    const Uint32 flags,
    const Array<String>& propertyList,
    ResponseHandler<CIMInstance> &handler)
{
    String className = ref.getClassName();
    _checkClass(className);

    Array<KeyBinding> keys = ref.getKeyBindings();

    //-- make sure we're the right instance
    int keyCount = 2;
    String keyName;
    String keyValue;

    if (keys.size() != keyCount)
        throw CIMException(CIM_ERR_INVALID_PARAMETER);

    for (unsigned int ii = 0; ii < keys.size(); ii++)
    {
         keyName = keys[ii].getName();
         keyValue = keys[ii].getValue();

         if ( keyName == "CreationClassName" &&
              (keyValue == CLASS_CIM_COMPUTER_SYSTEM ||
               keyValue == CLASS_CIM_UNITARY_COMPUTER_SYSTEM ||
               keyValue == CLASS_EXTENDED_COMPUTER_SYSTEM) )
         {
              keyCount--;
         }
         else if ( keyName == "Name" && keyValue == _cs.getHostName() )
         {
              keyCount--;
         }
     }

     if (keyCount)
     {
        throw CIMException(CIM_ERR_INVALID_PARAMETER);
     }

    // return instance of specified class
    CIMInstance instance = _buildInstance(ref.getClassName());

    handler.processing();
    handler.deliver(instance);
    handler.complete();

    return;
}

void ComputerSystemProvider::enumerateInstances(
      				const OperationContext& context, 
			        const CIMReference& ref, 
			        const Uint32 flags, 
			        const Array<String>& propertyList,
			        ResponseHandler<CIMInstance>& handler)
{
    String className = ref.getClassName();
    _checkClass(className);

    handler.processing();
    // unlike getInstance, enumeration returns instance from
    // deepest class supported
    CIMInstance instance = _buildInstance(CLASS_EXTENDED_COMPUTER_SYSTEM);
    handler.deliver(instance);
    handler.complete();
    return;
}

void ComputerSystemProvider::enumerateInstanceNames(
      				const OperationContext& context,
			  	const CIMReference &ref,
			  	ResponseHandler<CIMReference>& handler )
{
    String className = ref.getClassName();
    _checkClass(className);
    KeyBindingArray keys;

    keys.append(KeyBinding(PROPERTY_CREATION_CLASS_NAME,
                           CLASS_EXTENDED_COMPUTER_SYSTEM,
                           KeyBinding::STRING));
    keys.append(KeyBinding(PROPERTY_NAME,
                           _cs.getHostName(),
                           KeyBinding::STRING));

    handler.processing();
    handler.deliver(CIMReference(_cs.getHostName(),
                                 ref.getNameSpace(),
                                 CLASS_EXTENDED_COMPUTER_SYSTEM,
                                 keys));
    handler.complete();

    return;
}

void 
ComputerSystemProvider::modifyInstance(
      				const OperationContext& context,
			  	const CIMReference& ref,
			  	const CIMInstance& instanceObject,
			  	const Uint32 flags, 
			  	const Array<String>& propertyList,
			  	ResponseHandler<CIMInstance>& handler )
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

void 
ComputerSystemProvider::createInstance(
      				const OperationContext& context,
			  	const CIMReference& ref,
			  	const CIMInstance& instanceObject,
			  	ResponseHandler<CIMReference>& handler )
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

void 
ComputerSystemProvider::deleteInstance(
      				const OperationContext& context,
			  	const CIMReference& ref,
			  	ResponseHandler<CIMInstance>& handler )
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

void ComputerSystemProvider::initialize(CIMOMHandle& handle)
{
  _ch = handle;
  // platform-specific routine to initialize protected members
  _cs.initialize();
}


void ComputerSystemProvider::terminate(void)
{
}


CIMInstance ComputerSystemProvider::_buildInstance(const String& className) 
{
    CIMInstance instance(className);
    CIMProperty p;

    //-- fill in properties for CIM_ComputerSystem
    if (_cs.getCaption(p)) instance.addProperty(p);

    if (_cs.getDescription(p)) instance.addProperty(p);

    if (_cs.getInstallDate(p)) instance.addProperty(p);

    if (_cs.getStatus(p)) instance.addProperty(p);

    if (_cs.getCreationClassName(p)) instance.addProperty(p);

    if (_cs.getName(p)) instance.addProperty(p);

    if (_cs.getNameFormat(p)) instance.addProperty(p);

    if (_cs.getPrimaryOwnerName(p)) instance.addProperty(p);

    if (_cs.getPrimaryOwnerContact(p)) instance.addProperty(p);

    if (_cs.getRoles(p)) instance.addProperty(p);

    if (_cs.getOtherIdentifyingInfo(p)) instance.addProperty(p);

    if (_cs.getIdentifyingDescriptions(p)) instance.addProperty(p);

    if (_cs.getDedicated(p)) instance.addProperty(p);

    if (_cs.getResetCapability(p)) instance.addProperty(p);

    if (_cs.getPowerManagementCapabilities(p)) instance.addProperty(p);

    // Done if we are servicing CIM_ComputerSystem
    if (String::equalNoCase(className,CLASS_CIM_COMPUTER_SYSTEM))
      return instance;

    // Fill in properties for CIM_UnitaryComputerSystem
    if (_cs.getInitialLoadInfo(p)) instance.addProperty(p);

    if (_cs.getLastLoadInfo(p)) instance.addProperty(p);

    if (_cs.getPowerManagementSupported(p)) instance.addProperty(p);

    if (_cs.getPowerState(p)) instance.addProperty(p);

    if (_cs.getWakeUpType(p)) instance.addProperty(p);

    // Done if we are servicing CIM_UnitaryComputerSystem
    if (String::equalNoCase(className,CLASS_CIM_UNITARY_COMPUTER_SYSTEM))
      return instance;

    // Fill in properties for <Extended>_ComputerSystem
    if (String::equalNoCase(className,CLASS_EXTENDED_COMPUTER_SYSTEM))
    {
       if(_cs.getPrimaryOwnerPager(p)) instance.addProperty(p);
       if(_cs.getSecondaryOwnerName(p)) instance.addProperty(p);
       if(_cs.getSecondaryOwnerContact(p)) instance.addProperty(p);
       if(_cs.getSecondaryOwnerPager(p)) instance.addProperty(p);
       if(_cs.getSerialNumber(p)) instance.addProperty(p);
       if(_cs.getIdentificationNumber(p)) instance.addProperty(p);
    }
    return instance;
}

void ComputerSystemProvider::_checkClass(String& className)
{
    if (!String::equalNoCase(className, CLASS_CIM_COMPUTER_SYSTEM) &&
        !String::equalNoCase(className, CLASS_CIM_UNITARY_COMPUTER_SYSTEM) &&
        !String::equalNoCase(className, CLASS_EXTENDED_COMPUTER_SYSTEM))
    {
        throw CIMException(CIM_ERR_NOT_SUPPORTED);
    }
}
PEGASUS_NAMESPACE_END

