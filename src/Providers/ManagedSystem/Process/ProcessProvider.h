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
// Author: Christopher Neufeld <neufeld@linuxcare.com>
//         David Kennedy       <dkennedy@linuxcare.com>
//
// Modified By: David Kennedy       <dkennedy@linuxcare.com>
//              Christopher Neufeld <neufeld@linuxcare.com>
//              Al Stone            <ahs3@fc.hp.com>
//              Jim Metcalfe
//              Carlos Bonilla
//              Mike Glantz         <michael_glantz@hp.com>
//
//%////////////////////////////////////////////////////////////////////////////

#ifndef PG_PROCESS_PROVIDER_H
#define PG_PROCESS_PROVIDER_H

/* ==========================================================================
   Includes.
   ========================================================================== */

#include <Pegasus/Provider/CIMInstanceProvider.h>
#include "Process.h"

PEGASUS_NAMESPACE_BEGIN

/* ==========================================================================
   Class names.  These values are the names of the classes that
   are common for all of the providers.
   ========================================================================== */
#define CLASS_CIM_UNITARY_COMPUTER_SYSTEM  "CIM_UnitaryComputerSystem"
#define CLASS_CIM_OPERATING_SYSTEM         "CIM_OperatingSystem"
#define CLASS_CIM_PROCESS                  "CIM_Process"
#define CLASS_PG_UNIX_PROCESS              "PG_UnixProcess"

/* ==========================================================================
   The number of keys for the classes.
   ========================================================================== */
#define KEYS_PG_UNIX_PROCESS                    6

/* ==========================================================================
   Property names.  These values are returned by the provider as
   the property names.
   ========================================================================== */
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

#define PROPERTY_NAME                        "Name"
#define PROPERTY_PRIORITY                    "Priority"
#define PROPERTY_EXECUTION_STATE             "ExecutionState"
#define PROPERTY_OTHER_EXECUTION_DESCRIPTION "OtherExecutionDescription"
#define PROPERTY_CREATION_DATE               "CreationDate"
#define PROPERTY_TERMINATION_DATE            "TerminationDate"
#define PROPERTY_KERNEL_MODE_TIME            "KernelModeTime"
#define PROPERTY_USER_MODE_TIME              "UserModeTime"
#define PROPERTY_WORKING_SET_SIZE            "WorkingSetSize"

#define PROPERTY_PARENT_PROCESS_ID           "ParentProcessID"
#define PROPERTY_REAL_USER_ID                "ReadUserID"
#define PROPERTY_PROCESS_GROUP_ID            "ProcessGroupID"
#define PROPERTY_PROCESS_SESSION_ID          "ProcessSessionID"
#define PROPERTY_PROCESS_TTY                 "ProcessTTY"
#define PROPERTY_MODULE_PATH                 "ModulePath"
#define PROPERTY_PARAMETERS                  "Parameters"
#define PROPERTY_PROCESS_NICE_VALUE          "ProcessNiceValue"
#define PROPERTY_PROCESS_WAITING_FOR_EVENT   "ProcessWaitingForEvent"


class ProcessProvider : public CIMInstanceProvider
{

public:

  ProcessProvider();

  ~ProcessProvider();

  void createInstance(const OperationContext       &context,
                    const CIMReference           &instanceName,
                    const CIMInstance            &instanceObject,
                    ResponseHandler<CIMReference> &handler);

  void deleteInstance(const OperationContext       &context,
                    const CIMReference           &instanceReference,
                    ResponseHandler<CIMInstance> &handler);

  void enumerateInstances(
	const OperationContext & context,
	const CIMReference & classReference,
	const Uint32 flags,
	const Array<String> & propertyList,
	ResponseHandler<CIMInstance> & handler);

  void enumerateInstanceNames(const OperationContext        &context,
                            const CIMReference            &ref,
                            ResponseHandler<CIMReference> &handler);

  void getInstance(const OperationContext       &context,
                 const CIMReference           &instanceName,
                 const Uint32                  flags,
                 const Array<String>          &propertyList,
                 ResponseHandler<CIMInstance> &handler);
                 
  void modifyInstance(const OperationContext       &context,
                    const CIMReference           &instanceName,
                    const CIMInstance            &instanceObject,
		    const Uint32                 flags,
		    const Array<String>          &propertyList,
                    ResponseHandler<CIMInstance> &handler);

  void initialize(CIMOMHandle &ch);

  void terminate(void);

private:

  CIMOMHandle _ch;

  CIMInstance _constructInstance(const String &, const Process &);

  void _checkClass(String&);

  String _getCSName(void);

  String _getOSName(void);

  String _hostName;

  String _osName;

  void _chkInit(const OperationContext &);

  Boolean _initialized;
};

PEGASUS_NAMESPACE_END

#endif  /* #ifndef PG_PROCESS_PROVIDER_H */
