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

#ifndef PG_PROCESS_STAT_PROVIDER_H
#define PG_PROCESS_STAT_PROVIDER_H

/* ==========================================================================
   Includes.
   ========================================================================== */

#include "Process.h"

PEGASUS_NAMESPACE_BEGIN

/* ==========================================================================
   Class names.  These values are the names of the classes that
   are common for all of the providers.
   ========================================================================== */
#define CLASS_CIM_UNITARY_COMPUTER_SYSTEM "CIM_UnitaryComputerSystem"
#define CLASS_CIM_OPERATING_SYSTEM        "CIM_OperatingSystem"
#define CLASS_PG_UNIX_PROCESS_STAT        "PG_UnixProcessStatisticalInformation"

/* ==========================================================================
   The number of keys for the classes.
   ========================================================================== */
#define KEYS_PG_UNIX_PROCESS_STAT                    7

/* ==========================================================================
   Property names.  These values are returned by the provider as
   the property names.
   ========================================================================== */

// inherited from ManagedElement

#define PROPERTY_CAPTION                     "Caption"
#define PROPERTY_DESCRIPTION                 "Description"

// Keys

#define PROPERTY_CS_CREATION_CLASS_NAME      "CSCreationClassName"
#define PROPERTY_CS_NAME                     "CSName"
#define PROPERTY_OS_CREATION_CLASS_NAME      "OSCreationClassName"
#define PROPERTY_OS_NAME                     "OSName"
#define PROPERTY_PROCESS_CREATION_CLASS_NAME "ProcessCreationClassName"
#define PROPERTY_HANDLE                      "Handle"
#define PROPERTY_NAME                        "Name"

// Local properties

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


class ProcessStatProvider : public CIMInstanceProvider
{

public:

  ProcessStatProvider();

  ~ProcessStatProvider();

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
	const CIMPropertyList & propertyList,
	ResponseHandler<CIMInstance> & handler);

  void enumerateInstanceNames(const OperationContext        &context,
                            const CIMReference            &ref,
                            ResponseHandler<CIMReference> &handler);

  void getInstance(const OperationContext       &context,
                 const CIMReference           &instanceName,
                 const Uint32                  flags,
                 const CIMPropertyList        &propertyList,
                 ResponseHandler<CIMInstance> &handler);
                 
  void modifyInstance(const OperationContext       &context,
                    const CIMReference           &instanceName,
                    const CIMInstance            &instanceObject,
		    const Uint32                 flags,
		    const CIMPropertyList        &propertyList,
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

#endif  /* #ifndef PG_PROCESS_STAT_PROVIDER_H */
