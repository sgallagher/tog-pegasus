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
// Author: Chip Vincent (cvincent@us.ibm.com)
//
// Modified By:
//         Brian G. Campbell, EMC (campbell_brian@emc.com) - PEP140/phase2
//
//%/////////////////////////////////////////////////////////////////////////////

#include "OperationResponseHandler.h"

PEGASUS_NAMESPACE_BEGIN

void SimpleResponseHandler::send(Boolean isComplete)
{
	// If this was NOT instantiated as a derived OperationResponseHandle class, 
	// then this will be null but is NOT an error. In this case, there is no 
	// response attached, hence no data,so there is nothing to send. else we have
	// a valid "cross-cast" to the operation side

	OperationResponseHandler *operation = 
		dynamic_cast<OperationResponseHandler*>(this);

	if (operation)
		operation->send(isComplete);
}


ContentLanguages SimpleResponseHandler::getLanguages(void)
{
	Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
							"SimpleResponseHandler: getLanguages()");
	
	ContentLanguages langs;
	try
	{
		// Try to get the ContentLanguages out of the OperationContext
		// in the base ResponseHandler.
		OperationContext context = getContext();
		ContentLanguageListContainer cntr = context.get
			(ContentLanguageListContainer::NAME);
		langs = cntr.getLanguages();
	}
	catch (const Exception &)
	{
		// The content language container must not exist.
		// Return the empty ContentLanguages.
	}
	
	return langs;
}

PEGASUS_NAMESPACE_END
