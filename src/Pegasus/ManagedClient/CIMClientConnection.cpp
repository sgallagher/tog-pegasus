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
// Modified by: Robert Kieninger, IBM (kieningr@de.ibm.com) Bug#667
//         Josephine Eskaline Joyce,IBM (jojustin@in.ibm.com) PEP#101
//
//%/////////////////////////////////////////////////////////////////////////////

#include "CIMClientConnection.h"

PEGASUS_NAMESPACE_BEGIN

// class constructor
CIMClientConnection::CIMClientConnection()
{
	_hostname = String::EMPTY;
	_port = String::EMPTY;
	_userid = String::EMPTY;
	_passwd = String::EMPTY;
    _connectionHandle.reset(new CIMClientRep());
	_resolvedIP = 0xFFFFFFFF;
}

CIMClientConnection::CIMClientConnection(const String& host, const String& port, const String& userid, const String& passwd)
{
	_hostname = String(host);
	_port = String(port);
	_userid = String(userid);
	_passwd = String(passwd);
	
    _connectionHandle.reset(new CIMClientRep());
	_resolvedIP = System::_acquireIP((const char*)host.getCString());
	if (_resolvedIP == 0x7F000001)
	{
		// localhost or ip address of 127.0.0.1
		// still for compare we need the real ip address
		_resolvedIP = System::_acquireIP((const char *) System::getHostName().getCString());
	}
}

CIMClientConnection::CIMClientConnection(const String& host, const String& port, const String& userid, const String& passwd, const SSLContext& sslcontext)
{
	_hostname = String(host);
	_port = String(port);
	_userid = String(userid);
	_passwd = String(passwd);

    _sslcontext.reset(new SSLContext(sslcontext));

    _connectionHandle.reset(new CIMClientRep());
	_resolvedIP = System::_acquireIP((const char*)host.getCString());
	if (_resolvedIP == 0x7F000001)
	{
		// localhost or ip address of 127.0.0.1
		// still for compare we need the real ip address
		_resolvedIP = System::_acquireIP((const char *) System::getHostName().getCString());
	}
}
	
Boolean CIMClientConnection::equals(Uint32 ipAddress, const String& port)
{
	// only if port and resolved ip address are equal we have the same connection/CIMOM
	if ((ipAddress == _resolvedIP) && (String::equalNoCase(_port,port)))
	{
		return true;
	}
	return false;
}

CIMClientRep* CIMClientConnection::getConnectionHandle()
{
    return _connectionHandle.get();
}

String CIMClientConnection::getUser()
{
	return _userid;
}

String CIMClientConnection::getPass()
{
	return _passwd;
}

SSLContext* CIMClientConnection::getSSLContext()
{
    return _sslcontext.get();
}


PEGASUS_NAMESPACE_END
