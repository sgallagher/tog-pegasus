//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 BMC Software, Hewlett-Packard Company, IBM, 
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
// Author: Warren Otsuka (warren_otsuka@hp.com) 
//
// Modified By:  Carol Ann Krug Graves, Hewlett-Packard Company
//               (carolann_graves@hp.com)
//        
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <cassert>
#include <Pegasus/Client/CIMClient.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

const String NAMESPACE = "root/SampleProvider";
const String INSTANCE0 = "instance 0Sample_InstanceProviderClass";
const String INSTANCE1 = "instance 1Sample_InstanceProviderClass";
const String INSTANCE2 = "instance 2Sample_InstanceProviderClass";
const String CLASSNAME = "Sample_InstanceProviderClass";

int main(int argc, char** argv)
{
    try
    {
	CIMClient client;
	client.connectLocal();

	const String classname = CLASSNAME;
	Boolean deepInheritance = true;
	Boolean localOnly = true;
	Boolean includeQualifiers = false;
	Boolean includeClassOrigin = false;
	Array<CIMInstance> cimNInstances = 
	  client.enumerateInstances(NAMESPACE,  classname, deepInheritance,
				    localOnly,  includeQualifiers,
				    includeClassOrigin );

	assert( cimNInstances.size() == 3);
	for (Uint32 i = 0; i < cimNInstances.size(); i++)
	  {
	    String instanceRef = cimNInstances[i].getPath().toString();

	    //ATTN P2 WO 4 April 2002
	    // Test for INSTANCE0..2 when getInstanceName returns
            // the full reference string

	    if( !(String::equal(  instanceRef, CLASSNAME ) ) )
	      {
		PEGASUS_STD(cerr) << "Error: EnumInstances failed" <<
		  PEGASUS_STD(endl);
		exit(1);
	      }
	  }
    }
    catch(CIMClientException& e)
    {
	PEGASUS_STD(cerr) << "Error: " << e.getMessage() << PEGASUS_STD(endl);
	exit(1);
    }

    PEGASUS_STD(cout) << "EnumInstances +++++ passed all tests" << PEGASUS_STD(endl);














    return 0;
}
