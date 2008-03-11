//%2006////////////////////////////////////////////////////////////////////////
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
//=============================================================================
//
//%////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/String.h>
#include <Pegasus/Common/PegasusAssert.h>
#include "SoapUtils.h"

PEGASUS_NAMESPACE_BEGIN


XmlNamespace SoapNamespaces::supportedNamespaces[] = 
{
    {"SOAP-ENV", "http://www.w3.org/2003/05/soap-envelope",
     SOAP_ENVELOPE,       0},
    {"SOAP-ENC", "http://www.w3.org/2003/05/soap-encoding",
     SOAP_ENCODING,       0}, 
    {"xsi",      "http://www.w3.org/2001/XMLSchema-instance",
     XML_SCHEMA_INSTANCE, 0},
    {"xsd",      "http://www.w3.org/2001/XMLSchema",
     XML_SCHEMA,          0}, 
    {"wsman",    "http://schemas.dmtf.org/wbem/wsman/1/wsman.xsd",
     WS_MAN,              0},
    {"wsmb",     "http://schemas.dmtf.org/wbem/wsman/1/cimbinding.xsd", 
     WS_CIM_BINDING,      0},
    {"wsa",      "http://schemas.xmlsoap.org/ws/2004/08/addressing",
     WS_ADDRESSING,       0},
    {"wxf",      "http://schemas.xmlsoap.org/ws/2004/09/transfer",
     WS_TRANSFER,         0},
    {"wsen",     "http://schemas.xmlsoap.org/ws/2004/09/enumeration",
     WS_ENUMERATION,      0}, 
    {"wse",      "http://schemas.xmlsoap.org/ws/2004/08/eventing",
     WS_EVENTING,         0},
    {"wsp",      "http://schemas.xmlsoap.org/ws/2004/09/policy",
     WS_POLICY,           0},
    {"wsdl",     "http://schemas.xmlsoap.org/wsdl",
     WSDL,                0},
    {"wscim",    "http://schemas.dmtf.org/wbem/wscim/1/cim-schema/2",
     WS_CIM_SCHEMA,       0},
    {0, 0, LAST, 0}
};



String SoapUtils::getSoapActionName(
    const SoapNamespaces::Type nsType, const String& name)
{
    return (String) SoapNamespaces::supportedNamespaces[nsType].extendedName + 
        (String) "/" + name;
}


String SoapUtils::getMessageId()
{
    // ATTN WSMAN: Is this really unique?
    char uuid[50];
    sprintf(uuid, "uuid:%08X-%04X-%04X-%04X-%08X%04X",
        rand(),
        rand() & 0xFFFF,
        rand() & 0xFFFF,
        rand() & 0xFFFF,
        rand(),
        rand() & 0xFFFF);
    return String(uuid);
}

PEGASUS_NAMESPACE_END
