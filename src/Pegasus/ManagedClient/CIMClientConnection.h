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
//         Josephine Eskaline Joyce, IBM (jojustin@in.ibm.com) PEP#101
//
//%/////////////////////////////////////////////////////////////////////////////
#ifndef Pegasus_CIMClientConnection_h
#define Pegasus_CIMClientConnection_h

#include <Pegasus/Client/CIMClientRep.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Client/Linkage.h>
#include <Pegasus/Common/AutoPtr.h>

PEGASUS_NAMESPACE_BEGIN

class PEGASUS_CLIENT_LINKAGE CIMClientConnection
{

public:

	// class constructor
	CIMClientConnection();
	
	CIMClientConnection(const String& host, const String& port, const String& userid, const String& passwd);
	CIMClientConnection(const String& host, const String& port, const String& userid, const String& passwd, const SSLContext& sslcontext);

	Boolean equals(Uint32 ipAddress, const String& port);

	CIMClientRep *  getConnectionHandle(void);

	String getUser(void);
	String getPass(void);
	SSLContext* getSSLContext(void);

private:
    AutoPtr<CIMClientRep> _connectionHandle;
	String	_hostname;
	String	_port;
	String  _userid;
	String  _passwd;
    AutoPtr<SSLContext> _sslcontext;
	
	Uint32  _resolvedIP;
};

PEGASUS_NAMESPACE_END

#endif  // Pegasus_CIMClientConnection_h
