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
//==============================================================================
//
//
//%/////////////////////////////////////////////////////////////////////////////

#include "IPInfo.h"

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

static const CIMName        CLASS_NAME = CIMName          ("PG_NextHopIPRoute");
static const CIMNamespaceName NAMESPACE  = CIMNamespaceName ("root/cimv2");
#define HeaderFormat "%-16s %-10s %-16s %-16s"


////////////////////////////////////////////////////////////////////////////////
//  Constructor for Next Hop IP Route Info
////////////////////////////////////////////////////////////////////////////////
NextHopIPRouteInfo::NextHopIPRouteInfo(
    CIMClient &client, 
    Boolean enableDebug,
    ostream& outPrintWriter, 
    ostream& errPrintWriter)
{
    _enableDebug = enableDebug;

    try
    {
        Boolean deepInheritance = true;
        Boolean localOnly = true;
        Boolean includeQualifiers = false;
        Boolean includeClassOrigin = false;
          
        Array<CIMInstance> cimInstances = client.enumerateInstances(
            NAMESPACE, 
            CLASS_NAME,
            deepInheritance,
            localOnly,
            includeQualifiers,
            includeClassOrigin);
 
        Uint32 numberInstances = cimInstances.size();

        if (_enableDebug)
        {
            outPrintWriter << numberInstances << " instances of " <<
                CLASS_NAME.getString() << endl;
        }

        if (numberInstances > 0)
        {
            _gatherProperties(cimInstances[0]);
            _outputHeader(outPrintWriter);

            for (Uint32 i = 0; i < numberInstances; i++)
            {
                _gatherProperties(cimInstances[i]);
                _outputInstance(outPrintWriter);

            }   // end for looping through instances.
        }
        else
        {
            outPrintWriter << "No instances of class " 
                << CLASS_NAME.getString() << endl;
        }

    }  // end try .
    catch(Exception&)
    {
        errPrintWriter << "Error getting instances of class " <<
            CLASS_NAME.getString() << endl;
    }

}

////////////////////////////////////////////////////////////////////////////////
//  Destructor for Next Hop IP Route Info
////////////////////////////////////////////////////////////////////////////////
NextHopIPRouteInfo::~NextHopIPRouteInfo(void)
{
}

////////////////////////////////////////////////////////////////////////////////
//  Gather Properities for Next Hop IP Route Info
////////////////////////////////////////////////////////////////////////////////
void NextHopIPRouteInfo::_gatherProperties(CIMInstance &inst)
{
    _ipInstanceID = String::EMPTY;

    _ipIPDestAddr = String::EMPTY;
    _ipIPDestMask = String::EMPTY;
    _ipAddrType = 0;  // Unknown
    _ipCaption = String::EMPTY;
    _ipDescription = String::EMPTY;
    _ipName = String::EMPTY;

    for (Uint32 j=0; j < inst.getPropertyCount(); j++)
    {
        CIMName propertyName = inst.getProperty(j).getName();

        // Properties that are also keys
        if (propertyName.equal("InstanceID"))
        {
            inst.getProperty(j).getValue().get(_ipInstanceID);
        }
        // Other properties
        else if (propertyName.equal("Caption"))
        {
            inst.getProperty(j).getValue().get(_ipCaption); 
        }
        else if (propertyName.equal("Description"))
        {
            inst.getProperty(j).getValue().get(_ipDescription); 
        }
        else if (propertyName.equal("Name"))
        {
            inst.getProperty(j).getValue().get(_ipName); 
        }
        else if (propertyName.equal("DestinationAddress"))
        {
            inst.getProperty(j).getValue().get(_ipIPDestAddr); 
        }
        else if (propertyName.equal("DestinationMask"))
        {
            inst.getProperty(j).getValue().get(_ipIPDestMask); 
        }
        else if (propertyName.equal("AddressType"))
        {
            inst.getProperty(j).getValue().get(_ipAddrType); 
        }
        else if (propertyName.equal("PrefixLength"))
        {
            inst.getProperty(j).getValue().get(_ipPrefixLength); 
        }
   } // end for loop through properties

}


////////////////////////////////////////////////////////////////////////////////
//  Header Section for Next Hop IP Route Info
////////////////////////////////////////////////////////////////////////////////
void NextHopIPRouteInfo::_outputHeader(ostream &outPrintWriter)
{

    outPrintWriter << endl << ">>>> Next Hop IP Route Information <<<<" 
        << endl << endl;

    if (_ipInstanceID.size() > 0)
    {
        outPrintWriter << "InstanceID         : " << _ipInstanceID << endl;
    }

    char header[81];

    sprintf(header, HeaderFormat, "Route", "AddrType", "IP Dest Addr",
        "IP Dest Mask/Prefix Length", "Next Hop");

    outPrintWriter << endl << header << endl;
    
}

////////////////////////////////////////////////////////////////////////////////
//  Output an instance of an Next Hop IP Route Info
////////////////////////////////////////////////////////////////////////////////
void NextHopIPRouteInfo::_outputInstance(ostream &outPrintWriter)
{
    char row[81];

    if (_ipAddrType == 1)
    {
        sprintf(
            row, 
            HeaderFormat, 
            (const char *)_ipName.getCString(),
            "IPv4",
            (const char *)_ipIPDestAddr.getCString(),
            (const char *)_ipIPDestMask.getCString());
    }
    else 
    {
        if (_ipAddrType == 2)
        {
            String _ipt = "IPv6";
            char _pl[10];
            sprintf(_pl,"%d",_ipPrefixLength);

            if (_ipName.size() > 15)
            {
                sprintf(
                    row, 
                    HeaderFormat, 
                    (const char *)_ipName.getCString(),
                    "",
                    "",
                    "");
                outPrintWriter << row << endl;
                _ipName.clear();
            }

            if (_ipIPDestAddr.size() > 15)
            {
                sprintf(
                    row, 
                    HeaderFormat, 
                    (const char *)_ipName.getCString(),
                    (const char *)_ipt.getCString(),
                    (const char *)_ipIPDestAddr.getCString(),
                    "");
                outPrintWriter << row << endl;
                _ipIPDestAddr.clear();
                _ipt.clear();
            }
     
            sprintf(
                row, 
                HeaderFormat, 
                (const char *)_ipName.getCString(),
                (const char *)_ipt.getCString(),
                (const char *)_ipIPDestAddr.getCString(),
                _pl);
        }
        else
        {
            sprintf(
                row, 
                HeaderFormat, 
                (const char *)_ipName.getCString(),
                "Unk",
                (const char *)_ipIPDestAddr.getCString(),
                (const char *)_ipIPDestMask.getCString());
        }
    }


    outPrintWriter << row << endl;
    
}
