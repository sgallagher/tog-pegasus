//%2005////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
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
// Author: Christopher Neufeld <neufeld@linuxcare.com>
// 	   Al Stone <ahs3@fc.hp.com>
//
// Modified By: Al Stone <ahs3@fc.hp.com>
//
//%/////////////////////////////////////////////////////////////////////////

#ifndef _OPERATINGSYSTEMPROVIDER_H
#define _OPERATINGSYSTEMPROVIDER_H

#define OPERATINGSYSTEMCLASSNAME "Linux_OperatingSystem"
#define OPERATINGSYSTEMPROVIDERNAME "LinuxOperatingSystemProvider"

#ifndef UTMP_FILENAME
#   ifdef UTMP_FILE
#      define UTMP_FILENAME UTMP_FILE
#   else
#      define UTMP_FILENAME "/etc/utmp"
#   endif
#endif

#include <Pegasus/Common/Config.h>
#include <Pegasus/Provider/CIMInstanceProvider.h>
#include <Pegasus/Provider/CIMMethodProvider.h>
#include <Pegasus/Common/OperationContext.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/CIMName.h>
#include <Pegasus/Common/CIMObjectPath.h>
#include <Pegasus/Common/CIMInstance.h>

#include <iostream>

PEGASUS_NAMESPACE_BEGIN

class OperatingSystemProvider: public CIMInstanceProvider,
                               public CIMMethodProvider
{
   public:
      OperatingSystemProvider(void);
      ~OperatingSystemProvider(void);

      //-- CIMInstanceProvider methods
      /** Given a reference to an instance of the CIM class, fills in the data
       *  elements of the class with the details gleaned from the system. */
      void getInstance(const OperationContext& context, 
		       const CIMObjectPath& ref, 
		       const Boolean includeQualifiers,
		       const Boolean includeClassOrigin,
		       const CIMPropertyList& propertyList,
		       InstanceResponseHandler& handler);

      /** Returns filled instances for all instances of the CIM class detected
       *  on the system. */
      void enumerateInstances(const OperationContext& context, 
			      const CIMObjectPath& ref, 
			      const Boolean includeQualifiers,
			      const Boolean includeClassOrigin,
			      const CIMPropertyList& propertyList,
			      InstanceResponseHandler& handler);

      /** Produces a list of references to all instances of the CIM class
       *  detected on the system, but does not fill the instances
       *  themselves. */
      void enumerateInstanceNames(const OperationContext& context, 
			          const CIMObjectPath& ref,
			          ObjectPathResponseHandler& handler);
   
      /** Currently unimplemented in the Pegasus source, this is a no-op
       *  here. */
      void modifyInstance(const OperationContext& context,
		          const CIMObjectPath& ref,
		          const CIMInstance& instanceObject,
			  const Boolean includeQualifiers,
		          const CIMPropertyList& propertyList,
		          ResponseHandler& handler );

      /** Currently unimplemented in the Pegasus source, this is a no-op
       *  here. */
      void createInstance(const OperationContext& context,
		          const CIMObjectPath& ref,
		          const CIMInstance& instanceObject,
		          ObjectPathResponseHandler& handler );
 
      /** Currently unimplemented in the Pegasus source, this is a no-op
       *  here. */
      void deleteInstance(const OperationContext& context,
		          const CIMObjectPath& ref,
		          ResponseHandler& handler );
 
      void initialize(CIMOMHandle& handle);
      void terminate(void);

      //-- CIMMethodProvider methods
      Uint32 Reboot();
      Uint32 Shutdown();

      void invokeMethod(const OperationContext& context,
	                const CIMObjectPath& objectReference,
			const CIMName& methodName,
			const Array<CIMParamValue>& inParameters,
			MethodResultResponseHandler& handler);

   protected:
      /** Builds a filled-in instance. */
      CIMInstance _build_instance(const String &className);

      /** Builds a reference (a set of Key,Value pairs) */
      CIMObjectPath _fill_reference(const CIMNamespaceName &nameSpace, 
			           const String &className);
    
      /** Get the values needed */
      String _hostName();
      String _osName();
      Uint16 _osType();
      String _otherTypeDesc();
      String _version();
      CIMDateTime _lastBoot();
      CIMDateTime _localDateTime();
      Sint16 _timeZone();
      Uint32 _licensedUsers();
      Uint32 _users();
      Uint32 _processes();
      Uint32 _maxProcesses();
      Uint64 _totalSwap();
      Uint64 _totalVM();
      Uint64 _freeVM();
      Uint64 _freePhysical();
      Uint64 _totalVisible();
      Uint64 _totalPaging();
      Uint64 _freePaging();
      Uint64 _maxProcessMem();
      Boolean _distributed();
      Uint32 _maxUserProcs();
};

PEGASUS_NAMESPACE_END

#endif
