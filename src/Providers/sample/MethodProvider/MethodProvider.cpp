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
// Author: Chip Vincent (cvincent@us.ibm.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include "MethodProvider.h"

PEGASUS_NAMESPACE_BEGIN

MethodProvider::MethodProvider(void)
{
}

MethodProvider::~MethodProvider(void)
{
}

void MethodProvider::initialize(CIMOMHandle & cimom)
{
}

void MethodProvider::terminate(void)
{
}

void MethodProvider::invokeMethod(
	const OperationContext & context,
	const CIMObjectPath & objectReference,
	const CIMName & methodName,
	const Array<CIMParamValue> & inParameters,
	Array<CIMParamValue> & outParameters,
	ResponseHandler<CIMValue> & handler)
{
	// convert a fully qualified reference into a local reference
	// (class name and keys only).
	CIMObjectPath localReference = CIMObjectPath(
		String(),
		String(),
		objectReference.getClassName(),
		objectReference.getKeyBindings());

	handler.processing();

        if(String::equalNoCase(objectReference.getClassName(), 
                               "Sample_MethodProviderClass"))
	{
		if(String::equalNoCase(methodName, "SayHello"))
		{
		  String outString = "Hello";
		  if( inParameters.size() > 0 )
		    {
		      String replyName = String::EMPTY;

		      CIMValue paramVal = inParameters[0].getValue();
		      paramVal.get( replyName );
		      if( replyName != String::EMPTY )
			{
			  outString.append(", " + replyName + "!");
			}
		      outParameters.append( CIMParamValue( "Place", CIMValue( 
					     String("From Neverland") ) ) );
		      handler.deliver( CIMValue( outString ) );
		     
		    }
		  else
		    {
			handler.deliver(CIMValue( outString ));
		    }
		}
	}
	
	handler.complete();
}

PEGASUS_NAMESPACE_END
