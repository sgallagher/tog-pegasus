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
// Author: Mike Glantz, Hewlett-Packard Company <michael_glantz@hp.com>
//
//%////////////////////////////////////////////////////////////////////////////


// ==========================================================================
// try out all client operations on both providers
//
// The order of operations tested is:
//   enumerateInstanceNames
//   getInstance
//   createInstance
//   deleteInstance
//   enumerateInstances
//   modifyInstance
//
// getInstance() needs some names from enumerateInstanceNames()
// but will not iterate through all of them. Instead, it will
// just pick the middle one. modifyInstance, createInstance, and
// deleteInstance will simply use the instance (or its keys) from
// getInstance.
//
// Some operations are expected to fail with NOT_SUPPORTED, so
// any other behavior is a provider failure.
// ==========================================================================

// ==========================================================================
// Includes
// ==========================================================================

#include <Pegasus/Client/CIMClient.h>
#include "TestProcessProvider.h"

// ==========================================================================
// Miscellaneous defines
// ==========================================================================

#define NAMESPACE "root/cimv2"
#define HOST      "localhost:5988"
#define TIMEOUT   10000                    // timeout value in milliseconds

// ==========================================================================
// Class names.  These values are the names of the classes that
// are common for all of the providers.
// ==========================================================================

#define CLASS_CIM_UNITARY_COMPUTER_SYSTEM  "CIM_UnitaryComputerSystem"
#define CLASS_CIM_OPERATING_SYSTEM         "CIM_OperatingSystem"
#define CLASS_CIM_PROCESS                  "CIM_Process"

// Use PG_UnixProcess until DMTF finalizes CIM_UnixProcess
#define CLASS_UNIX_PROCESS                 "PG_UnixProcess"
#define CLASS_UNIX_PROCESS_STAT          "PG_UnixProcessStatisticalInformation"

// ==========================================================================
// The number of keys for the classes.
// ==========================================================================

#define NUMKEYS_UNIX_PROCESS                 6
#define NUMKEYS_UNIX_PROCESS_STAT            7

// ==========================================================================
// Property names.  These values are returned by the provider as
// the property names.
// ==========================================================================

#define PROPERTY_CS_CREATION_CLASS_NAME      "CSCreationClassName"
#define PROPERTY_CS_NAME                     "CSName"
#define PROPERTY_OS_CREATION_CLASS_NAME      "OSCreationClassName"
#define PROPERTY_OS_NAME                     "OSName"
#define PROPERTY_CREATION_CLASS_NAME         "CreationClassName"
#define PROPERTY_HANDLE                      "Handle"

#define PROPERTY_CAPTION                     "Caption"
#define PROPERTY_DESCRIPTION                 "Description"
#define PROPERTY_INSTALL_DATE                "InstallDate"
#define PROPERTY_STATUS                      "Status"

// Properties in CIM_Process

#define PROPERTY_NAME                        "Name"
#define PROPERTY_PRIORITY                    "Priority"
#define PROPERTY_EXECUTION_STATE             "ExecutionState"
#define PROPERTY_OTHER_EXECUTION_DESCRIPTION "OtherExecutionDescription"
#define PROPERTY_CREATION_DATE               "CreationDate"
#define PROPERTY_TERMINATION_DATE            "TerminationDate"
#define PROPERTY_KERNEL_MODE_TIME            "KernelModeTime"
#define PROPERTY_USER_MODE_TIME              "UserModeTime"
#define PROPERTY_WORKING_SET_SIZE            "WorkingSetSize"

// Properties in UnixProcess

#define PROPERTY_PARENT_PROCESS_ID           "ParentProcessID"
#define PROPERTY_REAL_USER_ID                "RealUserID"
#define PROPERTY_PROCESS_GROUP_ID            "ProcessGroupID"
#define PROPERTY_PROCESS_SESSION_ID          "ProcessSessionID"
#define PROPERTY_PROCESS_TTY                 "ProcessTTY"
#define PROPERTY_MODULE_PATH                 "ModulePath"
#define PROPERTY_PARAMETERS                  "Parameters"
#define PROPERTY_PROCESS_NICE_VALUE          "ProcessNiceValue"
#define PROPERTY_PROCESS_WAITING_FOR_EVENT   "ProcessWaitingForEvent"

// Properties in UnixProcessStatisticalInformation

#define PROPERTY_CPU_TIME                    "CPUTime"
#define PROPERTY_REAL_TEXT                   "RealText"
#define PROPERTY_REAL_DATA                   "RealData"
#define PROPERTY_REAL_STACK                  "RealStack"
#define PROPERTY_VIRTUAL_TEXT                "VirtualText"
#define PROPERTY_VIRTUAL_DATA                "VirtualData"
#define PROPERTY_VIRTUAL_STACK               "VirtualStack"
#define PROPERTY_VIRTUAL_MEMORY_MAPPED_FILESIZE  "VirtualMemoryMappedFileSize"
#define PROPERTY_VIRTUAL_SHARED_MEMORY       "VirtualSharedMemory"
#define PROPERTY_CPU_TIME_DEAD_CHILDREN      "CpuTimeDeadChildren"
#define PROPERTY_SYSTEM_TIME_DEAD_CHILDREN   "SystemTimeDeadChildren"
#define PROPERTY_REAL_SPACE                  "RealSpace"


PEGASUS_USING_STD;
PEGASUS_USING_PEGASUS;

CIMClient c;

void errorExit(CIMClientException& e)
{
  cout << "Error: Failed" << endl << e.getMessage() << endl;
  exit(1);
}

int testClass(const String& className)
{
  Array<CIMReference> refs;

  // =======================================================================
  // enumerateInstanceNames
  // =======================================================================

  cout << "+++++ enumerateInstanceNames(" << className << ") ";
  try
  {
    refs = c.enumerateInstanceNames(NAMESPACE,className);
  }
  catch (CIMClientException& e)
  {
    cout << endl;
    errorExit(e);
  }

  cout << refs.size() << " instances" << endl;
  // if zero instances, not an error, but can't proceed
  if (refs.size() == 0)
  {
    cout << "+++++ test completed early" << endl;
    return 0;
  }

  // =======================================================================
  // getInstance
  // =======================================================================

  // -------------------- First do normal getInstance() --------------------

  // pick the middle instance of the bunch
  int i = (refs.size()-1) >> 1;  // This is a shift right, not streamio!
  CIMReference ref = refs[i];
  CIMInstance inst;
  cout << "+++++ getInstance #" << i << endl;
  try
  {
    inst = c.getInstance(NAMESPACE,ref);
  }
  catch (CIMClientException& e)
  {
    errorExit(e);
  }

  // ATTN-MG-20020501: Can add some property value checks here

  // ------------------ do getInstance() with bad key ----------------------
  
  Array<KeyBinding> kb = ref.getKeyBindings();
  // mess up first key name
  kb[0].setName("foobar");
  ref.setKeyBindings(kb);
  
  int status = 0;

  cout << "+++++ getInstance with bad key" << endl;
  try
  {
    inst = c.getInstance(NAMESPACE,ref);
  }
  catch (CIMClientCIMException& e)
  {
    if (e.getCode() == CIM_ERR_INVALID_PARAMETER) status = 1;
  }
  catch (CIMClientException& e)
  {
    // any other exception is a failure; leave status alone
  }
  if (status == 0)
  {
    cout << "+++++ Error: bad instance name not rejected" << endl;
    return 1;
  }

  // =======================================================================
  // createInstance
  // =======================================================================

  CIMReference ref2;
  cout << "+++++ createInstance" << endl;
  status = 0;
  try
  {
    ref2 = c.createInstance(NAMESPACE,inst);
  }
  catch (CIMClientCIMException& e)
  {
    if (e.getCode() == CIM_ERR_NOT_SUPPORTED) status = 1;
  }
  catch (CIMClientException& e)
  {
    // any other CIMClientException is a problem; leave status alone
  }
  if (status == 0)
  {
    cout << "+++++ Error: createInstance didn't throw exception" << endl;
    return 1;
  }

  // =======================================================================
  // deleteInstance
  // =======================================================================

  cout << "+++++ deleteInstance" << endl;
  status = 0;
  try
  {
    c.deleteInstance(NAMESPACE,ref);
  }
  catch (CIMClientCIMException& e)
  {
    if (e.getCode() == CIM_ERR_NOT_SUPPORTED) status = 1;
  }
  catch (CIMClientException& e)
  {
    // any other CIMClientException is a problem; leave status alone
  }
  if (status == 0)
  {
    cout << "+++++ Error: deleteInstance didn't throw exception" << endl;
    return 1;
  }

  // =======================================================================
  // enumerateInstances
  // =======================================================================

  cout << "+++++ enumerateInstances(" << className << ")" << endl;
  
  Array<CIMNamedInstance> ia;
  try
  {
    ia = c.enumerateInstances(NAMESPACE,className);
  }
  catch (CIMClientException& e)
  {
    errorExit(e);
  }

  // There should be several instances
  if (ia.size() == 0)
  {
    cout << "+++++ Error: enumerateInstances on " << className << " returned too few instances" << endl;
    cout << "+++++ Test failed" << endl;
    return 1;
  }

  // =======================================================================
  // modifyInstance
  // =======================================================================

  // We do modifyInstance after enumerateInstances, because
  // modifyInstance requires a CIMNamedInstance argument, which
  // is conveniently what was returned by enumerateInstances

  CIMNamedInstance ni = ia[(ia.size()-1) >> 1]; // pick the middle one

  cout << "+++++ modifyInstance" << endl;
  status = 0;
  try
  {
    c.modifyInstance(NAMESPACE,ni);
  }
  catch (CIMClientCIMException& e)
  {
    if (e.getCode() == CIM_ERR_NOT_SUPPORTED) status = 1;
  }
  catch (CIMClientException& e)
  {
    // any other CIMClientException is a problem; leave status alone
  }
  if (status == 0)
  {
    cout << "+++++ Error: modifyInstance didn't throw exception" << endl;
    return 1;
  }

  // =======================================================================
  // Tests completed
  // =======================================================================

  return 0;
}

int main()
{
  cout << "+++++ Testing ProcessProviders" << endl;
  
  // Connect
  try
  {
    c.connect(HOST);
  }
  catch (CIMClientException& e)
  {
    errorExit(e);
  }

  int rc;
  if ((rc = testClass(CLASS_CIM_PROCESS)) != 0) return rc;
  if ((rc = testClass(CLASS_UNIX_PROCESS)) != 0) return rc;
  if ((rc = testClass(CLASS_UNIX_PROCESS_STAT)) != 0) return rc;
  cout << "+++++ passed all tests" << endl;
  return 0;
}
