//%2003////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002  BMC Software, Hewlett-Packard Development
// Company, L. P., IBM Corp., The Open Group, Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L. P.;
// IBM Corp.; EMC Corporation, The Open Group.
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
// Authors: Alagaraja Ramasubramanian, IBM Corporation 
//          Seema Gupta, IBM Corporation
//          Subodh Soni, IBM Corporation
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef SLPProv_H
#define SLPProv_H

#include <Pegasus/Common/Config.h>
#include <Pegasus/Provider/CIMInstanceProvider.h>
#include <Pegasus/../slp/peg_slp_agent.h>

PEGASUS_NAMESPACE_BEGIN

Boolean initFlag = false;

class SLPProvider: public CIMInstanceProvider
{
	public:
            SLPProvider(void);
	    ~SLPProvider(void);
		          
	    void initialize(CIMOMHandle & cimom);
	    void terminate(void);

	    void getInstance(
	       const OperationContext & context,
	       const CIMObjectPath & ref,
	       const Boolean includeQualifiers,
	       const Boolean includeClassOrigin,
	       const CIMPropertyList & propertyList,
	       InstanceResponseHandler & handler);

	    void enumerateInstances(
	       const OperationContext & context,
               const CIMObjectPath & ref,
               const Boolean includeQualifiers,
	       const Boolean includeClassOrigin,
	       const CIMPropertyList & propertyList,
	       InstanceResponseHandler & handler);

	    void enumerateInstanceNames(
	       const OperationContext & context,
	       const CIMObjectPath & ref,
	       ObjectPathResponseHandler & handler);

	    void modifyInstance(
               const OperationContext & context,
	       const CIMObjectPath & ref,
	       const CIMInstance & obj,
	       const Boolean includeQualifiers,
	       const CIMPropertyList & propertyList,
	       ResponseHandler & handler);

	    void createInstance(
	       const OperationContext & context,
	       const CIMObjectPath & ref,
	       const CIMInstance & obj,
	       ObjectPathResponseHandler & handler);

	    void deleteInstance(
	       const OperationContext & context,
	       const CIMObjectPath & ref,
	       ResponseHandler & handler);

	    Boolean tryterminate(void);

	
	protected:
	  
	    Array<CIMObjectPath> _instanceNames;
	    Array<CIMInstance> _instances;
	    Array<String> PropertyArray;
	    slp_service_agent slp_agent;
	    CIMOMHandle _ch;
	    int i;

            void populateData(void);
			   
   };

PEGASUS_NAMESPACE_END

#endif
