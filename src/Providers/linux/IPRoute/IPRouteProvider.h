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
//         David Kennedy       <dkennedy@linuxcare.com>
//
// Modified By: David Kennedy       <dkennedy@linuxcare.com>
//              Christopher Neufeld <neufeld@linuxcare.com>
//              Al Stone            <ahs3@fc.hp.com>
//
//%////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_IPRouteProvider_h
#define Pegasus_IPRouteProvider_h

#define IPROUTEPROVIDERNAME "LinuxIPRouteProvider"
#define IPROUTECLASSNAME "Linux_IPRoute"

#include <Pegasus/Common/Config.h>
#include <Pegasus/Provider/CIMInstanceProvider.h>
#include <Pegasus/Provider/CIMMethodProvider.h>
#include <Pegasus/Common/OperationContext.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/CIMObjectPath.h>
#include <Pegasus/Common/CIMInstance.h>

#include <iostream>
#include <vector>

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

/** The CIM data regarding routes includes information not returned by the
 *  Linux /sbin/route binary.  Instead, entries from both /sbin/route and
 *  /sbin/ifconfig must be correlated.  For efficiency, all instances of
 *  the route objects are loaded at once, and the results parsed and stored
 *  in this structure. */
struct route_data {
   String IPDestinationAddress;
   String IPDestinationMask;
   String NextHop;
   String AddressType;
   bool IsStatic;
};

/** Each named interface has an associated network address representation
 *  which we store here. */
struct interface_to_address {
   String interface_name;
   String address;
};


/** The actual work of answering provider queries for the Pegasus_Route
 *  class is done by this class. */
class LinuxIPRouteProvider : public CIMInstanceProvider
{
   public:
      LinuxIPRouteProvider();
      ~LinuxIPRouteProvider();

      /** Given a reference to an instance of the CIM class, fills in the data
       *  elements of the class with the details gleaned from the system. */
      void getInstance(const OperationContext& context,
		       const CIMObjectPath& ref,
		       const Boolean includeQualifiers,
		       const Boolean includeClassOrigin,
		       const CIMPropertyList& propertyList,
		       InstanceResponseHandler& handler );

      /** Returns filled instances for all instances of the CIM class detected
       *  on the system. */
      void enumerateInstances(const OperationContext& context,
			      const CIMObjectPath& ref,
			      const Boolean includeQualifiers,
			      const Boolean includeClassOrigin,
			      const CIMPropertyList& propertyList,
			      InstanceResponseHandler& handler );

      /** Produces a list of references to all instances of the CIM class
       *  detected on the system, but does not fill the instances
       *  themselves. */
      void enumerateInstanceNames(const OperationContext& context,
			          const CIMObjectPath& ref,
			          ObjectPathResponseHandler& handler );

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

   private:
      /** Takes the route_data structure and builds a reference (a set of
       *  Key,Value pairs) for it. */
      CIMObjectPath fill_reference(const CIMNamespaceName& nameSpace, 
	    			  const CIMName& className,
			          const struct route_data* ptr);

      /** Takes the route_data structure and builds a filled-in instance for
       *  its data. */
      CIMInstance build_instance(const CIMName& className, 
			         const struct route_data* ptr);

   private:
      /** This is loaded during every call to an instance enumeration function.
       *  Subsequent queries are answered from this local repository. */
      vector<struct route_data> allroutes;

      /** Loads the data from all existing routes into a local repository. */
      void load_all_route_data(void);

      /** Given the search parameters of the route (based on keys in the
       *  reference), returns a pointer to the data for that route. */
      const struct route_data* LocateRoute(String const& dest_sought, 
				           String const& mask_sought, 
				           String const& hop_sought, 
				           String const& type_sought) const;
};


PEGASUS_NAMESPACE_END

#endif  /* Pegasus_LinuxIPRouteProvider_h */
