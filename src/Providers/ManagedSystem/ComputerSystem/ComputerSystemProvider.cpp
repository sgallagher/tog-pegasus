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
// Author: Al Stone <ahs3@fc.hp.com>
//         Christopher Neufeld <neufeld@linuxcare.com>
//
// Modified By: David Kennedy       <dkennedy@linuxcare.com>
//              Christopher Neufeld <neufeld@linuxcare.com>
//              Al Stone            <ahs3@fc.hp.com>
//              Mike Glantz         <michael_glantz@hp.com>
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                (carolann_graves@hp.com)
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
// deepest class available.  All other
// operations take note of the specified class.
//
// Operations currently supported:
//
//  enumerateInstanceNames()
//  enumerateInstances()
//  getInstance()

// ==========================================================================
// includes
// ==========================================================================

#include "ComputerSystemProvider.h"
#include "ComputerSystem.h"
#include <Pegasus/Common/XmlWriter.h>
// ==========================================================================
// defines
// ==========================================================================

#define NUMKEYS_COMPUTER_SYSTEM              2


PEGASUS_USING_STD;
PEGASUS_USING_PEGASUS;



String globalNamespace = "root/cimv2";
static const char __NAMESPACE_NAMESPACE [] = "root";

static AtomicInt unload_flag;
static AtomicInt threads_running;


static void TestInstanceGetOperations(CIMOMHandle & handle)
{
   OperationContext context;
   
   Array<CIMName> classNames = 
      handle.enumerateClassNames(
	 context, 
	 globalNamespace, 
	 CIMName(), 
	 true);
   
   PEGASUS_STD(cout) << classNames.size() << " Classes found " << PEGASUS_STD(endl);
   
   Array<CIMObjectPath> instanceNames;
   Uint16 numberOfNotSupportedClassesFound = 0;
   
   for(Uint32 i = 0; i < classNames.size() ; i++, pegasus_sleep(1)) 
   {
      if (classNames[i].getString() == "PG_ShutdownService")
	 continue;
      try
      {
	 OperationContext context;

	 instanceNames = 
	    handle.enumerateInstanceNames(
	       context,
	       globalNamespace, 
	       classNames[i]);
	 
	 if( instanceNames.size())
	 {
	    
	    PEGASUS_STD(cout) << instanceNames.size() << " Instances found " << PEGASUS_STD(endl);
	    for(Uint32 i = 0; i < instanceNames.size() ; i++)
	    {
	       PEGASUS_STD(cout) << instanceNames[i] << PEGASUS_STD(endl);
	    }
	 }
      }
      catch(Exception & e)
      {
	 PEGASUS_STD(cout) << "Exception Instance Enumeration: " << e.getMessage() 
			   << PEGASUS_STD(endl);
      }
      catch(...)
      {
	 PEGASUS_STD(cerr) << "Unexpected Exception in Instance Enumeration" << PEGASUS_STD(endl);
      }
      
   }
   
}


static void TestNameSpaceOperations(CIMOMHandle & handle)
{
   String className = "__Namespace";
   Array<CIMObjectPath> instanceNames;
   OperationContext context;
   Uint32 i, n;
   if(unload_flag.value())
      return;
   
   PEGASUS_STD(cerr) << "Calling CIMOMHandle::enumerateInstanceNames()" << PEGASUS_STD(endl);
   try 
   {
      instanceNames = handle.enumerateInstanceNames(
	 context,
	 __NAMESPACE_NAMESPACE, 
	 className);
      
      for(i = 0, n = instanceNames.size(); i < n; i++)
      {
	 PEGASUS_STD(cout) << instanceNames[i].toString() << PEGASUS_STD(endl);
      }
      PEGASUS_STD(cout) << instanceNames.size() << " Namespaces" << PEGASUS_STD(endl);
   }
   catch(Exception & e)
   {
      PEGASUS_STD(cout) << "Exception Namespace Enumeration: " << e.getMessage() 
			<< PEGASUS_STD(endl);
   }
   catch(...)
   {
      PEGASUS_STD(cerr) << "Unexpected Exception in Namespace Enumeration" << PEGASUS_STD(endl);
   }
}


static void TestEnumerateClassNames (CIMOMHandle & handle )
{
   OperationContext context;
   Uint32 i, n;
   if(unload_flag.value())
      return;
   PEGASUS_STD(cout) << "TestEnumerateClassNames(CIMOMHandle)" << PEGASUS_STD(endl);
    try
    {
        CIMName className;

       Array<CIMName> classNames = handle.enumerateClassNames(
	  context, 
	  globalNamespace, 
	  className, true);
       for (i = 0, n = classNames.size(); i < n; i++)
       {
	  PEGASUS_STD(cout) << classNames[i] << PEGASUS_STD(endl);
       }
       
       PEGASUS_STD(cout) << classNames.size() << " ClassNames" << PEGASUS_STD(endl);
    }
    catch(Exception &e)
    {
       PEGASUS_STD(cout) << "Error Class Name Enumeration: " << PEGASUS_STD(endl);
       PEGASUS_STD(cout) << e.getMessage() << PEGASUS_STD(endl);
    }
   catch(...)
   {
      PEGASUS_STD(cerr) << "Unexpected Exception in Class Name Enumeration" << PEGASUS_STD(endl);
   }

}

static void TestGetClass(CIMOMHandle & handle)
{
   OperationContext context;
   CIMPropertyList list;
   CIMConstClass  c;
   if(unload_flag.value())
      return;
   try 
   {
      c = handle.getClass(
	 context, 
	 globalNamespace, 
	 "CIM_ComputerSystem", 
	 false, 
	 false, 
	 true,
	 list);
      try 
      {
	 XmlWriter::printClassElement(c, PEGASUS_STD(cout));
      }

      catch(...)
      {
      }
      
   }
   catch(Exception &e)
   {
      PEGASUS_STD(cout) << "Error Get Class: " << PEGASUS_STD(endl);
      PEGASUS_STD(cout) << e.getMessage() << PEGASUS_STD(endl);
   }
   catch(...)
   {
      PEGASUS_STD(cerr) << "Unexpected Exception in Get Class " << PEGASUS_STD(endl);
   }
}


 PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL test_cimom_handle(void *parm)
{
   
   Thread *th = reinterpret_cast<Thread *>(parm);
   ComputerSystemProvider *myself = 
      reinterpret_cast<ComputerSystemProvider *>(th->get_parm());
   pegasus_sleep(1000);
   if(unload_flag.value() == 0)
   {
      TestNameSpaceOperations(myself->_ch);
      pegasus_sleep(1000);
      if(unload_flag.value() == 0)   
	 TestEnumerateClassNames(myself->_ch);
      pegasus_sleep(1000);
      if(unload_flag.value() == 0)   
	 TestGetClass(myself->_ch);
      pegasus_sleep(1000);
      TestInstanceGetOperations(myself->_ch); 
   }
   threads_running--;
   PEGASUS_STD(cout) << "exiting test loop" << PEGASUS_STD(endl);
   
   exit_thread((PEGASUS_THREAD_RETURN)1);
   return(PEGASUS_THREAD_RETURN)0;
}


ComputerSystemProvider::ComputerSystemProvider(void)
{
}

ComputerSystemProvider::~ComputerSystemProvider(void)
{
}

void ComputerSystemProvider::getInstance(
    const OperationContext& context,
    const CIMObjectPath& ref,
    const Boolean includeQualifiers,
    const Boolean includeClassOrigin,
    const CIMPropertyList& propertyList,
    InstanceResponseHandler &handler)
{





    CIMName className = ref.getClassName();
    _checkClass(className);

    Array<CIMKeyBinding> keys = ref.getKeyBindings();

    //-- make sure we're the right instance
    unsigned int keyCount = NUMKEYS_COMPUTER_SYSTEM;
    CIMName keyName;
    String keyValue;

    if (keys.size() != keyCount)
        throw CIMInvalidParameterException("Wrong number of keys");

    for (unsigned int ii = 0; ii < keys.size(); ii++)
    {
         keyName = keys[ii].getName();
         keyValue = keys[ii].getValue();

         if (keyName.equal (PROPERTY_CREATION_CLASS_NAME) &&
              (String::equalNoCase(keyValue,CLASS_CIM_COMPUTER_SYSTEM) ||
               String::equalNoCase(keyValue,CLASS_CIM_UNITARY_COMPUTER_SYSTEM) ||
               String::equalNoCase(keyValue,CLASS_EXTENDED_COMPUTER_SYSTEM) ||
               String::equalNoCase(keyValue,String::EMPTY)) )
         {
              keyCount--;
         }
         else if (keyName.equal ("Name") &&
                   String::equalNoCase(keyValue,_cs.getHostName()) )
         {
              keyCount--;
         }
     }

     if (keyCount)
     {
        throw CIMInvalidParameterException(String::EMPTY);
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
			        const CIMObjectPath& ref,
				const Boolean includeQualifiers,
				const Boolean includeClassOrigin,
			        const CIMPropertyList& propertyList,
			        InstanceResponseHandler& handler)
{
    CIMName className = ref.getClassName();
    _checkClass(className);

    handler.processing();

    // Deliver instance only if request was for leaf class
    if (className.equal (CLASS_EXTENDED_COMPUTER_SYSTEM))
    {
      CIMInstance instance = _buildInstance(CLASS_EXTENDED_COMPUTER_SYSTEM);
      handler.deliver(instance);
    }

    handler.complete();
    return;
}

void ComputerSystemProvider::enumerateInstanceNames(
      				const OperationContext& context,
			  	const CIMObjectPath &ref,
			  	ObjectPathResponseHandler& handler )
{
   PEGASUS_STD(cout) << " TEST PROVIDER RECURSIVE" << PEGASUS_STD(endl);
   
    CIMName className = ref.getClassName();
    _checkClass(className);

    handler.processing();

    // Deliver instance only if request was for leaf class
    if (className.equal (CLASS_EXTENDED_COMPUTER_SYSTEM))
    {
      Array<CIMKeyBinding> keys;

      keys.append(CIMKeyBinding(PROPERTY_CREATION_CLASS_NAME,
                             CLASS_EXTENDED_COMPUTER_SYSTEM,
                             CIMKeyBinding::STRING));
      keys.append(CIMKeyBinding(PROPERTY_NAME,
                             _cs.getHostName(),
                             CIMKeyBinding::STRING));

      handler.deliver(CIMObjectPath(_cs.getHostName(),
                                   ref.getNameSpace(),
                                   CLASS_EXTENDED_COMPUTER_SYSTEM,
                                   keys));
    }

    handler.complete();
    return;
}

void
ComputerSystemProvider::modifyInstance(
      				const OperationContext& context,
			  	const CIMObjectPath& ref,
			  	const CIMInstance& instanceObject,
				const Boolean includeQualifiers,
			  	const CIMPropertyList& propertyList,
			  	ResponseHandler& handler )
{
    throw CIMNotSupportedException(String::EMPTY);
}

void
ComputerSystemProvider::createInstance(
      				const OperationContext& context,
			  	const CIMObjectPath& ref,
			  	const CIMInstance& instanceObject,
			  	ObjectPathResponseHandler& handler )
{
    throw CIMNotSupportedException(String::EMPTY);
}

void
ComputerSystemProvider::deleteInstance(
      				const OperationContext& context,
			  	const CIMObjectPath& ref,
			  	ResponseHandler& handler )
{
    throw CIMNotSupportedException(String::EMPTY);
}

void ComputerSystemProvider::initialize(CIMOMHandle& handle)
{
  _ch = handle;
  // platform-specific routine to initialize protected members
  _cs.initialize();

  PEGASUS_STD(cout) << "Initializing Test Thread " << PEGASUS_STD(endl);

  static Thread th(test_cimom_handle, this, true);
  threads_running++;
  th.run();
}


void ComputerSystemProvider::terminate(void)
{
   unload_flag = 1;
   while(threads_running.value())
      pegasus_yield();
   
   delete this;
}


CIMInstance ComputerSystemProvider::_buildInstance(const CIMName& className)
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
    if (className.equal (CLASS_CIM_COMPUTER_SYSTEM))
      return instance;

    // Fill in properties for CIM_UnitaryComputerSystem
    if (_cs.getInitialLoadInfo(p)) instance.addProperty(p);

    if (_cs.getLastLoadInfo(p)) instance.addProperty(p);

    if (_cs.getPowerManagementSupported(p)) instance.addProperty(p);

    if (_cs.getPowerState(p)) instance.addProperty(p);

    if (_cs.getWakeUpType(p)) instance.addProperty(p);

    // Done if we are servicing CIM_UnitaryComputerSystem
    if (className.equal (CLASS_CIM_UNITARY_COMPUTER_SYSTEM))
      return instance;

    // Fill in properties for <Extended>_ComputerSystem
    if (className.equal (CLASS_EXTENDED_COMPUTER_SYSTEM))
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

void ComputerSystemProvider::_checkClass(CIMName& className)
{
    if (!className.equal (CLASS_CIM_COMPUTER_SYSTEM) &&
        !className.equal (CLASS_CIM_UNITARY_COMPUTER_SYSTEM) &&
        !className.equal (CLASS_EXTENDED_COMPUTER_SYSTEM))
    {
        throw CIMNotSupportedException(String::EMPTY);
    }
}
