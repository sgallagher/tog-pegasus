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
// Author: Marek Szermutzky (MSzermutzky@de.ibm.com) PEP#139 Stage2
//
//%/////////////////////////////////////////////////////////////////////////////
#ifndef Pegasus_CIMDefaultClientConnectionManager_h
#define Pegasus_CIMDefaultClientConnectionManager_h

#include "CIMClientConnectionManager.h"
#include "CIMClientConnection.h"
#include <Pegasus/Common/Array.h>
	
PEGASUS_NAMESPACE_BEGIN

class PEGASUS_CLIENT_LINKAGE CIMDefaultClientConnectionManager : public CIMClientConnectionManager
{

public:

	// class constructor
	CIMDefaultClientConnectionManager();
	
	// virtual class destructor has to be implemented by specific implementation
	~CIMDefaultClientConnectionManager();

	// this function returns the specified connection
	CIMClientRep* getConnection(
								const String& host,
								const String& port,
								const CIMNamespaceName& nameSpace);


	void addConnection(const String& host, const String& port, const String& userid, const String& passwd);
	void addConnection(const String& host, const String& port, const String& userid, const String& passwd, const SSLContext& sslcontext);

private:
	
	Array<CIMClientConnection*>	_cccm_container;

};

PEGASUS_NAMESPACE_END

#endif  // Pegasus_CIMDefaultClientConnectionManager_h
