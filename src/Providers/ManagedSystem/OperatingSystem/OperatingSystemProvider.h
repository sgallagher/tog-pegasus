//%////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2002 BMC Software, Hewlett-Packard Company, IBM,
// The Open Group, Tivoli Systems
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
//==========================================================================
//
// Author: Christopher Neufeld <neufeld@linuxcare.com>
// 	   Al Stone <ahs3@fc.hp.com>
//
// Modified By: Al Stone <ahs3@fc.hp.com>
//
//%/////////////////////////////////////////////////////////////////////////
#ifndef _OPERATINGSYSTEMPROVIDER_H
#define _OPERATINGSYSTEMPROVIDER_H

#include <Pegasus/Common/Config.h>
#include <Pegasus/Provider/CIMInstanceProvider.h>
#include <Pegasus/Provider/CIMMethodProvider.h>
#include <Pegasus/Common/OperationContext.h>
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
		       const CIMReference& ref, 
		       const Uint32 flags, 
		       const CIMPropertyList& propertyList,
		       ResponseHandler<CIMInstance>& handler);

      /** Returns filled instances for all instances of the CIM class detected
       *  on the system. */
      void enumerateInstances(const OperationContext& context, 
			      const CIMReference& ref, 
			      const Uint32 flags,
			      const CIMPropertyList& propertyList,
			      ResponseHandler<CIMInstance>& handler);

      /** Produces a list of references to all instances of the CIM class
       *  detected on the system, but does not fill the instances
       *  themselves. */
      void enumerateInstanceNames(const OperationContext& context, 
			          const CIMReference& ref,
			          ResponseHandler<CIMReference>& handler);
   
      /** Currently unimplemented in the Pegasus source, this is a no-op
       *  here. */
      void modifyInstance(const OperationContext& context,
		          const CIMReference& ref,
		          const CIMInstance& instanceObject,
		          const Uint32 flags,
		          const CIMPropertyList& propertyList,
		          ResponseHandler<CIMInstance>& handler );

      /** Currently unimplemented in the Pegasus source, this is a no-op
       *  here. */
      void createInstance(const OperationContext& context,
		          const CIMReference& ref,
		          const CIMInstance& instanceObject,
		          ResponseHandler<CIMReference>& handler );
 
      /** Currently unimplemented in the Pegasus source, this is a no-op
       *  here. */
      void deleteInstance(const OperationContext& context,
		          const CIMReference& ref,
		          ResponseHandler<CIMInstance>& handler );
 
      void initialize(CIMOMHandle& handle);
      void terminate(void);

      void invokeMethod(const OperationContext& context,
	                const CIMReference& objectReference,
			const String& methodName,
			const Array<CIMParamValue>& inParameters,
			Array<CIMParamValue>& outParameters,
			ResponseHandler<CIMValue>& handler);

   private:
      // store off for future use
      CIMOMHandle _cimomhandle;
      
      // Builds a filled-in instance. 
      CIMInstance _build_instance(const CIMReference &objectReference);

      // Builds a reference (a set of Key,Value pairs) 
      CIMReference _fill_reference(const String &nameSpace, 
			           const String &className);
};

PEGASUS_NAMESPACE_END

#endif
