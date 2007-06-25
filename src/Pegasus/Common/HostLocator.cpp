//%2007////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
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
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/HostLocator.h>

PEGASUS_NAMESPACE_BEGIN

void HostLocator::_init()
{
    _isValid = false;
    _portNumber = PORT_UNSPECIFIED;
}

void HostLocator::_parseLocator(const String &locator)
{
    if (locator == String::EMPTY)
    {
        return;
    }
    int i = 0;
    String host;
    Boolean enclosed =  false;
    if (locator[0] == '[')
    {   
        i = 1;
        while (locator[i] && locator[i] != ']')
        {
            ++i;
        }
        if (locator[i] != ']')
        {
            return;
        }
        host = locator.subString (1, i - 1);
        ++i;
        enclosed = true;
    }
    else
    {
        while (locator[i] && locator[i] != ':')
        {
            ++i;
        }
        host = locator.subString(0, i);
    }
    _hostAddr.setHostAddress(host);
    if (!_hostAddr.isValid() ||
        _hostAddr.getAddressType() == HostAddress::AT_IPV6 && !enclosed ||
        _hostAddr.getAddressType() != HostAddress::AT_IPV6 && enclosed)
    {
        return;
    }

    // Check for Port number.
    Uint32 port = 0;  
    if (locator[i] == ':')
    {
        ++i;
        _portNumber = PORT_INVALID;
        while (isascii(locator[i]) && isdigit(locator[i]))
        {
            port = port * 10 + ( locator[i] - '0' );
            if (port > MAX_PORT_NUMBER)
            {
                return;  
            }
            ++i;
        }
        if (locator[i-1] != ':' && locator[i] == char(0))
        {
            _portNumber = port;
        }
        else
        {
            return;
        }
    }
    if (!locator[i])
    {
        _isValid = true;
    }
}

HostLocator::HostLocator()
{
    _init();
}

HostLocator::HostLocator(const String &locator)
{
    _init();
    _parseLocator(locator);
}

HostLocator& HostLocator::operator =(const HostLocator &rhs)
{
    if (this != &rhs)
    {
        _hostAddr = rhs._hostAddr;
        _portNumber = rhs._portNumber;
        _isValid = rhs._isValid;
    }

    return *this;
}

HostLocator::HostLocator(const HostLocator &rhs)
{
    *this = rhs;
}

HostLocator::~HostLocator()
{
}

void HostLocator::setHostLocator(const String &locator)
{
    _init();
    _parseLocator(locator);
}

Uint32 HostLocator::getPort()
{
    return _portNumber;
}

String HostLocator::getPortString()
{
    char portStr[20];
    sprintf(portStr, "%u", _portNumber);
    return String(portStr);
}

Uint16 HostLocator::getAddressType()
{
    return _hostAddr.getAddressType();
}

Boolean HostLocator::isValid()
{
    return _isValid;
}

String HostLocator::getHost()
{
    return _hostAddr.getHost();
}

Boolean HostLocator::isPortSpecified()
{
    return _portNumber != PORT_UNSPECIFIED && _portNumber != PORT_INVALID;
}

PEGASUS_NAMESPACE_END
